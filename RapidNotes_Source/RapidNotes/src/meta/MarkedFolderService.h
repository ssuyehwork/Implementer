#pragma once

#include <QObject>
#include <QStringList>
#include <QMenu>

namespace QuarkMeta {

class MarkedFolderService : public QObject {
    Q_OBJECT

public:
    static MarkedFolderService& instance();
    explicit MarkedFolderService(QObject* parent = nullptr);
    ~MarkedFolderService() override = default;

    bool isMarked(const QString& path) const;
    bool addMarked(const QString& path);
    bool removeMarked(const QString& path);
    bool toggleMarked(const QString& path);
    int markedCount() const;
    QStringList getAllMarked() const;
    QStringList getActiveMarked() const;
    bool removeFolderRecord(const QString& path);
    bool clearAllFolderRecords();
    bool clearAll();
    bool keepOnlyMarked(const QString& targetPath);

    /**
     * @brief 触发 Alt+V 一键同时分发/迁移选中项目到目的地文件夹
     * @param targetDirs 指定的目的地文件夹列表。若为空，则默认迁移分发至所有已激活的标记目的地文件夹
     */
    bool executeTransferToMarked(const QStringList& sourcePaths, const QStringList& targetDirs = {});

    QAction* buildMarkAction(QMenu* parentMenu, const QString& path, QObject* receiver);
    QAction* buildMarkAction(QMenu* parentMenu, const QStringList& paths, QObject* receiver);

    bool toggleMarked(const QStringList& paths);

signals:
    void markedFoldersChanged();

private:
    Q_DISABLE_COPY_MOVE(MarkedFolderService)
};

} // namespace QuarkMeta
