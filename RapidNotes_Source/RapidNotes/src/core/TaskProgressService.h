#pragma once

#include <QObject>
#include <QString>
#include <QMap>
#include <QMutex>
#include <atomic>

namespace QuarkMeta {

struct TaskJobInfo {
    int id = -1;
    QString title;
    QString detail;
    int currentStep = 0;
    int totalSteps = 100;
    int percent = 0;
};

class TaskProgressService : public QObject {
    Q_OBJECT

public:
    static TaskProgressService& instance();

    /**
     * @brief 注册新后台任务 (线程安全，支持任意子线程调用)
     * @param title 任务主标题 (如: "正在提取缩略图...")
     * @param totalSteps 任务总步数
     * @return 唯一任务 ID
     */
    int createJob(const QString& title, int totalSteps = 100);

    /**
     * @brief 更新指定任务进度 (线程安全)
     */
    void updateProgress(int jobId, int currentStep, int totalSteps = -1, const QString& detail = "");

    /**
     * @brief 标记任务完成 (线程安全)
     */
    void finishJob(int jobId);

    /**
     * @brief 强制取消/移除任务 (线程安全)
     */
    void cancelJob(int jobId);

    int activeJobCount() const;
    bool hasActiveJobs() const;

signals:
    /**
     * @brief 首个任务启动时发射 (驱动进度栏平滑展开)
     */
    void jobStarted(int jobId, const QString& title);

    /**
     * @brief 进度变动广播 (含当前聚焦任务的百分比与详情)
     */
    void progressUpdated(int percent, const QString& title, const QString& detail, int activeCount);

    /**
     * @brief 单个任务完成广播
     */
    void jobFinished(int jobId);

    /**
     * @brief 全量后台任务归零时发射 (驱动进度栏平滑收起隐藏)
     */
    void allJobsFinished();

private:
    explicit TaskProgressService(QObject* parent = nullptr);
    ~TaskProgressService() override = default;
    TaskProgressService(const TaskProgressService&) = delete;
    TaskProgressService& operator=(const TaskProgressService&) = delete;

    void recalculateAndEmit();

    mutable QMutex m_mutex;
    std::atomic<int> m_nextJobId{1};
    QMap<int, TaskJobInfo> m_jobs;
};

} // namespace QuarkMeta
