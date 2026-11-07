#pragma once

#include <QObject>
#include <QStringList>
#include <QList>
#include <QWidget>

namespace QuarkMeta {

class TrashService : public QObject {
    Q_OBJECT

public:
    static TrashService& instance();

    bool moveToTrash(const QStringList& paths, QWidget* parentWidget = nullptr);
    bool restoreItems(const QList<int>& trashIds, QWidget* parentWidget = nullptr);
    bool restoreAll(QWidget* parentWidget = nullptr);
    bool restoreToDirectory(const QStringList& trashPaths, const QString& targetDir, QWidget* parentWidget = nullptr);
    bool emptyTrash(QWidget* parentWidget = nullptr);

signals:
    void trashItemCountChanged(int totalCount);
    void trashOperationCompleted();

private:
    explicit TrashService(QObject* parent = nullptr);
    ~TrashService() override = default;
    TrashService(const TrashService&) = delete;
    TrashService& operator=(const TrashService&) = delete;
};

} // namespace QuarkMeta
