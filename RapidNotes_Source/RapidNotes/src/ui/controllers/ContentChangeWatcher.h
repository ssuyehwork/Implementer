#ifndef CONTENTCHANGEWATCHER_H
#define CONTENTCHANGEWATCHER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QSet>
#include <QTimer>
#include <atomic>

namespace QuarkMeta {

class ContentPanel;

class ContentChangeWatcher : public QObject {
    Q_OBJECT

public:
    explicit ContentChangeWatcher(ContentPanel* panel);
    ~ContentChangeWatcher() override;

    void updateTarget();
    void onLoadFinished();

    void beginSuppress();
    void endSuppress();

private slots:
    void onDirectoryChanged(const QString& dirPath, const QStringList& dirtyNames, bool overflow);
    void onWatchedDirectoryLost(const QString& dirPath);
    void processAccumulatedChanges();

private:
    void unwatchAll();
    bool shouldWatchPath(const QString& path) const;
    bool isDelayed() const;

    ContentPanel* m_panel = nullptr;
    QSet<QString> m_watchedPaths;
    bool m_watchedIncludeSubtree = false;

    QSet<QString> m_pendingDirtyNames;
    QString m_pendingDirPath;
    QTimer* m_delayedApplyTimer = nullptr;
    std::atomic<bool> m_isTaskRunning{false};
    int m_suppressCount = 0;
};

} // namespace QuarkMeta

#endif // CONTENTCHANGEWATCHER_H
