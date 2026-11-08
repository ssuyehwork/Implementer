#pragma once

#include <QObject>
#include <QMap>
#include <QList>
#include <QTimer>
#include <cstdint>
#include "SmartFolderDao.h"

namespace QuarkMeta {

/**
 * @brief 智能文件夹计数与异步后台计算服务
 * 统一管理智能文件夹匹配数量的内存缓存、防抖调度与 QtConcurrent 异步批处理计算。
 */
class SmartFolderCountService : public QObject {
    Q_OBJECT
public:
    explicit SmartFolderCountService(QObject* parent = nullptr);
    ~SmartFolderCountService() override = default;

    /**
     * @brief 立即发起计算（直接使用传入的智能文件夹列表）
     */
    void requestCountsImmediately(const QList<SmartFolderRecord>& smartFolders);

    /**
     * @brief 防抖发起计算（150ms 聚合后在后台线程自行拉取全部智能文件夹记录）
     */
    void requestCountsDebounced();

    /**
     * @brief 失效内存缓存
     * @param smartFolderId 为 0 时清空全部缓存，否则移除指定智能文件夹缓存
     */
    void invalidate(int smartFolderId = 0);

    /**
     * @brief 读取缓存的匹配数，未缓存返回 -999
     */
    int cachedCount(int smartFolderId) const;

signals:
    /**
     * @brief 异步计算完成并回到主线程后发射此信号
     * @param counts 智能文件夹 ID 到匹配计数的映射
     */
    void countsReady(const QMap<int, int>& counts);

private:
    void executeCalculationAsync(uint64_t reqId, const QList<SmartFolderRecord>& smartFolders);

    QMap<int, int> m_smartFolderCountCache;
    uint64_t m_currentCountReqId = 0;
    QTimer* m_countDebounceTimer = nullptr;
};

} // namespace QuarkMeta
