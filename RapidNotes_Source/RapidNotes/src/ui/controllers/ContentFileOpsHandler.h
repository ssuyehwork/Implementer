#pragma once

#include <QObject>
#include <QStringList>
#include <QModelIndex>

namespace QuarkMeta {

class ContentPanel;

class ContentFileOpsHandler : public QObject {
    Q_OBJECT
public:
    explicit ContentFileOpsHandler(ContentPanel* panel);
    ~ContentFileOpsHandler() override = default;

    void createNewItem(const QString& type);
    void performBatchRename();
    bool resolvePasteDestination();
    void onPathsDropped(const QStringList& paths, const QModelIndex& targetIndex, const QString& targetDirOverride = QString(), Qt::DropAction action = Qt::CopyAction);

private:
    ContentPanel* m_panel = nullptr;
};

} // namespace QuarkMeta
