#ifndef QUICKCONTEXTMENUBUILDER_H
#define QUICKCONTEXTMENUBUILDER_H

#include <QMenu>
#include <QModelIndexList>
#include <QWidget>

class QuickWindow;

namespace RapidNotes {

class QuickContextMenuBuilder {
public:
    static QMenu* buildListContextMenu(QuickWindow* window, const QModelIndexList& selected, QWidget* parent);
    static QMenu* buildSidebarContextMenu(QuickWindow* window, const QModelIndex& index, QWidget* parent);
};

} // namespace RapidNotes

#endif // QUICKCONTEXTMENUBUILDER_H
