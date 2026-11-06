#ifndef DROPLISTVIEW_H
#define DROPLISTVIEW_H

#include <QListView>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QStringList>
#include <QModelIndex>

namespace RapidNotes {

class DropListView : public QListView {
    Q_OBJECT
public:
    explicit DropListView(QWidget* parent = nullptr);

protected:
    void startDrag(Qt::DropActions supportedActions) override;

signals:
    void pathsDropped(const QStringList& paths, const QModelIndex& targetIndex, Qt::DropAction action);
    void sectionHeaderClicked(const QModelIndex& index);
    void blankSpaceClicked();
    void blankSpaceDoubleClicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
};

} // namespace RapidNotes

#endif // DROPLISTVIEW_H
