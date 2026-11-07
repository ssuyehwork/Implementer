#pragma once

#include <QObject>
#include <QColor>
#include <QPair>
#include <QVector>
#include <QThreadPool>
#include <deque>
#include <vector>
#include <string>
#include <mutex>
#include <atomic>
#include <memory>
#include "../util/DiskMediaExtractor.h"

namespace QuarkMeta {

class MediaExtractorPipeline : public QObject {
    Q_OBJECT
public:
    static MediaExtractorPipeline& instance();

    void enqueue(const std::wstring& path);
    void enqueueBatch(const std::vector<std::wstring>& paths);

    // 2026-07-27 按照 Plan-107：安全、平滑取消与中止接口
    void cancelAll();
    void cancelBatch(const std::vector<std::wstring>& paths);

private:
    MediaExtractorPipeline(QObject* parent = nullptr);
    ~MediaExtractorPipeline() override;

    void dispatchWorkersIfNeeded();
    void dispatchWorkerLoop();

    std::deque<std::wstring> m_queue;
    std::mutex m_queueMutex;
    std::atomic<int> m_activeCount{0}; // 正在处理解析中 of 任务数量
    std::atomic<int> m_activeWorkers{0}; // 活跃工作线程数
    std::shared_ptr<CancellationToken> m_token = std::make_shared<CancellationToken>(); // 当前代取消令牌，受 m_queueMutex 保护
    QThreadPool m_workerPool; // 后台提取专用线程池，不占全局线程池
};

} // namespace QuarkMeta
