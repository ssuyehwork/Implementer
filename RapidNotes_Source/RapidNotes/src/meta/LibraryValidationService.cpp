#include "LibraryValidationService.h"
#include "LibrarySyncService.h"
#include "LibraryItemDao.h"
#include "MetadataManager.h"
#include "../ui/ContentPanel.h"
#include <QFileInfo>
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include <QCoreApplication>
#include <QPointer>
#include <QDebug>

namespace QuarkMeta {

LibraryValidationService& LibraryValidationService::instance() {
    static LibraryValidationService s_instance;
    return s_instance;
}

LibraryValidationService::LibraryValidationService(QObject* parent) : QObject(parent) {
    m_valPool.setMaxThreadCount(1);
}

LibraryValidationService::~LibraryValidationService() {
    cancelCurrentValidation();
    m_valPool.waitForDone();
}

void LibraryValidationService::cancelCurrentValidation() {
    std::lock_guard<std::mutex> lock(m_tokenMutex);
    if (m_currentToken) {
        m_currentToken->cancel();
        m_currentToken.reset();
    }
}

void LibraryValidationService::validateRecordsAsync(const std::vector<ItemRecord>& records, ContentPanel* panel, int reqId) {
    cancelCurrentValidation();

    if (records.empty() || !panel) return;

    std::shared_ptr<CancellationToken> token = std::make_shared<CancellationToken>();
    {
        std::lock_guard<std::mutex> lock(m_tokenMutex);
        m_currentToken = token;
    }

    QPointer<ContentPanel> weakPanel(panel);

    (void)QtConcurrent::run(&m_valPool, [this, records, weakPanel, reqId, token]() {
        const int batchSize = 200;
        int total = static_cast<int>(records.size());

        for (int i = 0; i < total; i += batchSize) {
            if (token->isCanceled()) return;

            int count = std::min(batchSize, total - i);
            QStringList missingPaths;
            QStringList modifiedPaths;

            for (int j = i; j < i + count; ++j) {
                if (token->isCanceled()) return;
                const auto& rec = records[j];
                QFileInfo fi(rec.path);

                if (!fi.exists()) {
                    // 1. 文件不存在：放入缺失列表
                    missingPaths.append(rec.path);
                } else {
                    // 2. 文件存在：检查大小或修改时间是否变化
                    if (rec.size != fi.size() || rec.mtime != fi.lastModified().toMSecsSinceEpoch()) {
                        modifiedPaths.append(rec.path);
                    }
                }
            }

            // 批量提交给 LibrarySyncService 处理写库与状态修正（校验服务本身不写库）
            if (!missingPaths.isEmpty()) {
                (void)QtConcurrent::run([missingPaths]() {
                    for (const QString& mp : missingPaths) {
                        LibraryItemDao::markSyncStateByPath(mp, 2);
                    }
                });

                // 通知 UI 单条擦除隐匿（不全局刷新，不改变顺序）
                QMetaObject::invokeMethod(QCoreApplication::instance(), [weakPanel, missingPaths, reqId]() {
                    if (weakPanel && weakPanel->loadRequestId() == reqId && weakPanel->model()) {
                        weakPanel->model()->applyIncrementalChanges({}, missingPaths);
                        weakPanel->recalculateAndEmitStats();
                    }
                }, Qt::QueuedConnection);
            }

            if (!modifiedPaths.isEmpty()) {
                LibrarySyncService::instance().syncPaths(modifiedPaths);
            }
        }
    });
}

} // namespace QuarkMeta
