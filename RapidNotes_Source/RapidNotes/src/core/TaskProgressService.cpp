#include "TaskProgressService.h"
#include <QMutexLocker>
#include <QCoreApplication>
#include <algorithm>

namespace QuarkMeta {

TaskProgressService& TaskProgressService::instance() {
    static TaskProgressService s_instance;
    return s_instance;
}

TaskProgressService::TaskProgressService(QObject* parent) : QObject(parent) {}

int TaskProgressService::createJob(const QString& title, int totalSteps) {
    int id = m_nextJobId.fetch_add(1);

    {
        QMutexLocker locker(&m_mutex);
        TaskJobInfo info;
        info.id = id;
        info.title = title;
        info.totalSteps = totalSteps > 0 ? totalSteps : 100;
        info.currentStep = 0;
        info.percent = 0;
        m_jobs.insert(id, info);
    }

    QMetaObject::invokeMethod(this, [this, id, title]() {
        emit jobStarted(id, title);
        recalculateAndEmit();
    }, Qt::QueuedConnection);

    return id;
}

void TaskProgressService::updateProgress(int jobId, int currentStep, int totalSteps, const QString& detail) {
    {
        QMutexLocker locker(&m_mutex);
        if (!m_jobs.contains(jobId)) return;

        auto& info = m_jobs[jobId];
        if (totalSteps > 0) info.totalSteps = totalSteps;
        info.currentStep = currentStep;
        if (!detail.isEmpty()) info.detail = detail;

        if (info.totalSteps > 0) {
            info.percent = std::clamp(static_cast<int>(static_cast<double>(info.currentStep) / info.totalSteps * 100.0), 0, 100);
        }
    }

    QMetaObject::invokeMethod(this, &TaskProgressService::recalculateAndEmit, Qt::QueuedConnection);
}

void TaskProgressService::finishJob(int jobId) {
    bool wasEmpty = false;
    {
        QMutexLocker locker(&m_mutex);
        m_jobs.remove(jobId);
        wasEmpty = m_jobs.isEmpty();
    }

    QMetaObject::invokeMethod(this, [this, jobId, wasEmpty]() {
        emit jobFinished(jobId);
        recalculateAndEmit();
        if (wasEmpty) {
            emit allJobsFinished();
        }
    }, Qt::QueuedConnection);
}

void TaskProgressService::cancelJob(int jobId) {
    finishJob(jobId);
}

int TaskProgressService::activeJobCount() const {
    QMutexLocker locker(&m_mutex);
    return m_jobs.size();
}

bool TaskProgressService::hasActiveJobs() const {
    QMutexLocker locker(&m_mutex);
    return !m_jobs.isEmpty();
}

void TaskProgressService::recalculateAndEmit() {
    int activeCount = 0;
    QString lastTitle;
    QString lastDetail;
    int averagePercent = 100;

    {
        QMutexLocker locker(&m_mutex);
        if (m_jobs.isEmpty()) {
            emit progressUpdated(100, "", "", 0);
            return;
        }

        const auto& lastJob = m_jobs.last();
        activeCount = m_jobs.size();
        lastTitle = lastJob.title;
        lastDetail = lastJob.detail;

        double totalPercentSum = 0.0;
        for (const auto& job : m_jobs) {
            totalPercentSum += job.percent;
        }
        averagePercent = static_cast<int>(totalPercentSum / activeCount);
    }

    emit progressUpdated(averagePercent, lastTitle, lastDetail, activeCount);
}

} // namespace QuarkMeta
