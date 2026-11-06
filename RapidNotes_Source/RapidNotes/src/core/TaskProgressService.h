#pragma once

#include <QObject>
#include <QString>
#include <QMap>
#include <QMutex>
#include <atomic>

namespace RapidNotes {

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

    int createJob(const QString& title, int totalSteps = 100);
    void updateProgress(int jobId, int currentStep, int totalSteps = -1, const QString& detail = "");
    void finishJob(int jobId);
    void cancelJob(int jobId);

    int activeJobCount() const;
    bool hasActiveJobs() const;

signals:
    void jobStarted(int jobId, const QString& title);
    void progressUpdated(int percent, const QString& title, const QString& detail, int activeCount);
    void jobFinished(int jobId);
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

} // namespace RapidNotes
