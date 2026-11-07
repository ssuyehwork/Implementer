#ifndef ITEMMODELBASE_H
#define ITEMMODELBASE_H

#include <QAbstractTableModel>
#include <QHash>
#include <QSet>
#include <QTimer>
#include <algorithm>
#include <vector>
#include "src/core/ItemRecord.h" // 修正为正确的头文件路径
#include "WindowsShellThumbnailProvider.h"

namespace QuarkMeta {
    enum class RecordSource {
        Disk,      // 磁盘模式（需要 MetaCacheDecorator::decorate 装饰 JSON 元数据）
        Prepared   // 预装配/库模式（元数据已在数据库中装配完成，跳过 decorate）
    };

    struct QStringHash {
        size_t operator()(const QString& key) const {
            return qHash(key);
        }
    };

    class ItemModelBase : public QAbstractTableModel {
        Q_OBJECT
    public:
        explicit ItemModelBase(QObject* parent = nullptr) : QAbstractTableModel(parent) {
            // 系统图标异步加载完成后的刷新，统一由基类负责，所有派生模型（内容区/列视图每一列）自动生效。
            // 先在主线程确保提供者与通知器已创建，保证它们归属主线程。
            WindowsShellThumbnailProvider::instance();

            m_iconRefreshTimer = new QTimer(this);
            m_iconRefreshTimer->setSingleShot(true);
            m_iconRefreshTimer->setInterval(35);
            connect(m_iconRefreshTimer, &QTimer::timeout, this, &ItemModelBase::flushIconRefresh);
            connect(&IconLoadNotifier::instance(), &IconLoadNotifier::iconLoadedForKey,
                    this, &ItemModelBase::onSystemIconLoaded);
        }
        virtual ~ItemModelBase() override = default;

        virtual QString currentPath() const { return m_currentPath; }
        virtual void setCurrentPath(const QString& path) { m_currentPath = path; }

        // 暴露通用接口合约，由 DiskItemModel 实现
        virtual const std::vector<QuarkMeta::ItemRecord>& allRecords() const = 0;
        virtual void setRecords(const std::vector<QuarkMeta::ItemRecord>& records, RecordSource source = RecordSource::Disk) = 0;
        virtual void appendRecord(const QuarkMeta::ItemRecord& record) = 0;
        virtual void clear() = 0;
        virtual void updateRecordMetadata(QString path) = 0;
        virtual void updateRecordMetadataBatch(const QStringList& paths) {
            for (const QString& p : paths) updateRecordMetadata(p);
        }
        virtual void loadThumbnailsForRows(const QList<int>& rows) = 0;
        virtual void migrateCache(const QString& oldPath, const QString& newPath) = 0;
        virtual void clearCacheForFolder(const QString& folderPath) = 0;
        virtual QStringList applyIncrementalChanges(const std::vector<QuarkMeta::ItemRecord>& records, const QStringList& removedPaths) = 0;

    protected:
        QString m_currentPath;

    private slots:
        void onSystemIconLoaded(const QString& key) {
            m_pendingIconKeys.insert(key);
            if (m_iconRefreshTimer && !m_iconRefreshTimer->isActive()) {
                m_iconRefreshTimer->start();
            }
        }

        // 只刷新"使用了刚加载完成的系统图标"的行，合并为连续区间，仅标记 DecorationRole
        void flushIconRefresh() {
            if (m_pendingIconKeys.isEmpty()) return;
            const QSet<QString> keys = m_pendingIconKeys;
            m_pendingIconKeys.clear();

            const auto& records = allRecords();
            std::vector<int> rows;
            for (int i = 0; i < static_cast<int>(records.size()); ++i) {
                const auto& rec = records[i];
                const QString k = WindowsShellThumbnailProvider::iconKey(rec.path, rec.isDir, rec.suffix);
                if (keys.contains(k)) rows.push_back(i);
            }
            if (rows.empty()) return;

            const int lastCol = qMax(0, columnCount() - 1);
            int start = rows.front();
            int prev = start;
            for (size_t n = 1; n <= rows.size(); ++n) {
                if (n < rows.size() && rows[n] == prev + 1) {
                    prev = rows[n];
                    continue;
                }
                emit dataChanged(index(start, 0), index(prev, lastCol), {Qt::DecorationRole});
                if (n < rows.size()) {
                    start = rows[n];
                    prev = start;
                }
            }
        }

    private:
        QTimer* m_iconRefreshTimer = nullptr;
        QSet<QString> m_pendingIconKeys;
    };
}

using ItemModelBase = QuarkMeta::ItemModelBase;

#endif // ITEMMODELBASE_H