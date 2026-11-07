#ifndef DISKITEMMODEL_H
#define DISKITEMMODEL_H

#include "ItemModelBase.h"
#include <QCache>
#include <QMap>
#include <QIcon>
#include <QMutex>
#include <QThreadPool>
#include <memory>
#include "../../meta/MetadataDefs.h"
#include "../../meta/QuarkMetaJson.h"

#include <unordered_map>
#include <QSet>
#include <QPointer>
#include <QTimer>
#include "../../core/CoreEngine.h"

namespace QuarkMeta {

class DiskItemModel : public ItemModelBase {
    Q_OBJECT
public:
    explicit DiskItemModel(QObject* parent = nullptr);
    virtual ~DiskItemModel() override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // 切换目录/清空数据时调用，使所有已派发的旧任务瞬间失效
    void incrementGeneration();
    uint64_t currentGeneration() const { return m_currentGen.load(std::memory_order_relaxed); }

    const std::vector<QuarkMeta::ItemRecord>& allRecords() const override { return m_allRecords; }
    void setRecords(const std::vector<QuarkMeta::ItemRecord>& records, RecordSource source = RecordSource::Disk) override;
    void addItemRecord(const QuarkMeta::ItemRecord& record);
    void appendRecord(const QuarkMeta::ItemRecord& record);
    void clear() override;
    void updateRecordMetadata(QString path) override;
    void updateRecordMetadataBatch(const QStringList& paths) override;
    void loadThumbnailsForRows(const QList<int>& rows) override;
    void migrateCache(const QString& oldPath, const QString& newPath) override;
    void clearCacheForFolder(const QString& folderPath) override;

    // 增量更新接口
    QStringList applyIncrementalChanges(const std::vector<QuarkMeta::ItemRecord>& records, const QStringList& removedPaths) override;

    // 异步全文件夹文件头极速尺寸提取流水线
    void preloadDimensionsAsync(const QStringList& targetPaths = QStringList());

    // 强制重载指定路径的内存缩略图与宽高比缓存，并触发视图重绘
    void reloadThumbnailForPath(const QString& path);

    static QThreadPool* thumbnailPool();

signals:
    void thumbnailLoaded(int rowIndex);

protected:
    std::vector<QuarkMeta::ItemRecord> m_allRecords;
    std::unordered_map<QString, int, QuarkMeta::QStringHash> m_pathToIndex;
    mutable QCache<QString, QIcon> m_iconCache;
    QSet<QString> m_requestedPaths; // 🚨 核心防爆锁：记录已经在排队/处理中的任务路径
    mutable QMap<QString, double> m_aspectRatios;

    std::atomic<uint64_t> m_currentGen{0};

    QMutex m_genTokenMutex;
    QHash<uint64_t, std::shared_ptr<CancellationToken>> m_genTokens;

private slots:
    void flushPendingThumbDataChanged();

private:
    QTimer* m_thumbBatchTimer = nullptr;
    QSet<int> m_pendingThumbRows;
};

} // namespace QuarkMeta

#endif // DISKITEMMODEL_H
