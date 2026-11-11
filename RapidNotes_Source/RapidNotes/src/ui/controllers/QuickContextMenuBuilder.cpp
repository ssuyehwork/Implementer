#include "QuickContextMenuBuilder.h"
#include "../QuickWindow.h"
#include "../IconHelper.h"
#include "../models/NoteModel.h"
#include "../models/CategoryModel.h"
#include <QDesktopServices>
#include <QUrl>

namespace RapidNotes {

QMenu* QuickContextMenuBuilder::buildListContextMenu(QuickWindow* window, const QModelIndexList& selected, QWidget* parent) {
    if (!window) return nullptr;
    auto* menu = new QMenu(parent);
    IconHelper::setupMenu(menu);

    if (selected.isEmpty()) {
        menu->addAction(IconHelper::getIcon("add", "#3498db", 18), " 新建笔记", window, &QuickWindow::doNewIdea);
        return menu;
    }

    menu->addAction(IconHelper::getIcon("eye", "#41F2F2", 18), "预览", window, &QuickWindow::doPreview);
    menu->addAction(IconHelper::getIcon("star-001", "#f1c40f", 18), "收藏/取消收藏", window, &QuickWindow::doToggleFavorite);
    menu->addAction(IconHelper::getIcon("pin_vertical", "#e67e22", 18), "置顶/取消置顶", window, &QuickWindow::doTogglePin);
    menu->addSeparator();
    menu->addAction(IconHelper::getIcon("trash-svgrepo-com", "#e74c3c", 18), "移入回收站", [window]() {
        window->doDeleteSelected(false);
    });

    return menu;
}

QMenu* QuickContextMenuBuilder::buildSidebarContextMenu(QuickWindow* window, const QModelIndex& index, QWidget* parent) {
    if (!window || !index.isValid()) return nullptr;
    auto* menu = new QMenu(parent);
    IconHelper::setupMenu(menu);

    QString type = index.data(CategoryModel::TypeRole).toString();
    QString idxName = index.data(CategoryModel::NameRole).toString();

    if (type == "category") {
        menu->addAction(IconHelper::getIcon("edit", "#3498db", 18), QString("重命名分类 [%1]").arg(idxName), [window, index]() {
            // 就地重命名调用
        });
    }

    return menu;
}

} // namespace RapidNotes
