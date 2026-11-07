#include "BatchQuickLookService.h"
#include "../ui/QuickLookWindow.h"
#include "../ui/UiHelper.h"
#include "../ui/ToolTipOverlay.h"
#include "../core/TaskProgressService.h"
#include "../core/AppConfig.h"
#include <QtConcurrent>
#include <QThreadPool>
#include <QCoreApplication>
#include <QDirIterator>
#include <QFileInfo>
#include <QDir>
#include <QCryptographicHash>
#include <QCursor>

namespace QuarkMeta {

namespace {
QThreadPool* batchQuickLookPool() {
    static QThreadPool pool;
    static std::once_flag flag;
    std::call_once(flag, []() { pool.setMaxThreadCount(1); });
    return &pool;
}

// 校验特定文件的 quicklook_previews 高清缓存是否存在且未过期
bool isQuickLookCacheValid(const QString& filePath) {
    QFileInfo srcInfo(filePath);
    if (!srcInfo.exists() || srcInfo.size() <= 0) return true; // 无效原文件，跳过

    QByteArray normalized = QDir::toNativeSeparators(filePath).toLower().toUtf8();
    QString hashStr = QString::fromUtf8(QCryptographicHash::hash(normalized, QCryptographicHash::Sha256).toHex());
    QString dirL1 = hashStr.left(2);
    QString dirL2 = hashStr.mid(2, 2);

    QString baseCacheDir = AppConfig::instance().getGlobalCacheDir() + "/quicklook_previews";
    QString cachePath = QString("%1/%2/%3/%4.png").arg(baseCacheDir, dirL1, dirL2, hashStr);
    QFileInfo cacheInfo(cachePath);

    return (cacheInfo.exists() && cacheInfo.size() > 0 && cacheInfo.lastModified() >= srcInfo.lastModified());
}
} // namespace

BatchQuickLookService& BatchQuickLookService::instance() {
    static BatchQuickLookService inst;
    return inst;
}

BatchQuickLookService::BatchQuickLookService(QObject* parent) : QObject(parent) {}

void BatchQuickLookService::cancelCurrentBatch() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_currentToken) {
        m_currentToken->cancel();
    }
}

bool BatchQuickLookService::isRunning() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentToken && !m_currentToken->isCanceled();
}

void BatchQuickLookService::extractBatchAsync(const QStringList& rootPaths) {
    if (rootPaths.isEmpty()) return;

    // 若有旧任务正在进行，先触发熔断取消
    cancelCurrentBatch();

    std::shared_ptr<CancellationToken> token = std::make_shared<CancellationToken>();
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_currentToken = token;
    }

    (void)QtConcurrent::run(batchQuickLookPool(), [rootPaths, token]() {
        // 1. 递归扫描 + 缓存 MTime 极速预检 (Pre-Scan Filter)
        QStringList targetFiles;
        for (const QString& rootPath : rootPaths) {
            if (token->isCanceled()) break;

            QFileInfo fi(rootPath);
            if (fi.isDir()) {
                QDirIterator it(rootPath, QDir::Files, QDirIterator::Subdirectories);
                while (it.hasNext()) {
                    if (token->isCanceled()) break;
                    it.next();
                    QString filePath = it.filePath();
                    QString ext = it.fileInfo().suffix().toLower();
                    if (UiHelper::isGraphicsFile(ext)) {
                        // 预检：只保留缓存缺失或需要更新的文件，避免重复生成浪费算力
                        if (!isQuickLookCacheValid(filePath)) {
                            targetFiles << filePath;
                        }
                    }
                }
            } else if (fi.isFile()) {
                QString ext = fi.suffix().toLower();
                if (UiHelper::isGraphicsFile(ext)) {
                    if (!isQuickLookCacheValid(rootPath)) {
                        targetFiles << rootPath;
                    }
                }
            }
        }

        if (token->isCanceled()) return;

        if (targetFiles.isEmpty()) {
            QMetaObject::invokeMethod(QCoreApplication::instance(), []() {
                ToolTipOverlay::instance()->showText(QCursor::pos(), "所有所选项目的高清大图预览已是最新，无需重新生成", 2000, QColor("#2ecc71"));
            });
            return;
        }

        // 2. 将真正的“缺失/过期增量文件列表”打包提交批量渲染
        int total = targetFiles.size();

        TaskJobOptions opts;
        opts.title = "正在批量生成大图预览";
        opts.total = total;
        opts.showCount = true;
        opts.showEta = true;
        opts.finishTextTemplate = "批量生成大图预览完成  数量：{total}  |  实际耗时：{elapsed}";
        opts.onTrip = [token]() {
            if (token) token->cancel();
        };

        TaskProgressJob job = TaskProgressService::instance().createJobHandle(opts);

        for (const QString& filePath : targetFiles) {
            if (token->isCanceled()) break;

            (void)QuickLookWindow::loadOrExtractQuickLookImage(filePath, 2048, token);
            job.advance(1);
        }

        job.finish();

        if (!token->isCanceled()) {
            QMetaObject::invokeMethod(QCoreApplication::instance(), [total]() {
                ToolTipOverlay::instance()->showText(
                    QCursor::pos(),
                    QString("大图预览批量生成完成，共计 %1 项").arg(total),
                    2000,
                    QColor("#2ecc71")
                );
            });
        }
    });
}

} // namespace QuarkMeta
