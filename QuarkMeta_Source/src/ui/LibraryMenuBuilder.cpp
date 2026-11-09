#include "LibraryMenuBuilder.h"
#include "UiHelper.h"
#include "controllers/ContextMenuFactory.h"

namespace QuarkMeta {

void LibraryMenuBuilder::buildSortSubMenu(QMenu* parentMenu, const std::function<void(bool)>& onSortByName, const std::function<void()>& onSortManual) {
    if (!parentMenu) return;
    auto* sortMenu = parentMenu->addMenu(UiHelper::getIcon("list_ul", QColor("#AAAAAA")), "排列");
    UiHelper::applyMenuStyle(sortMenu);

    QAction* sortAsc = sortMenu->addAction("按名称 (A→Z)");
    QObject::connect(sortAsc, &QAction::triggered, [onSortByName]() {
        if (onSortByName) onSortByName(true);
    });

    QAction* sortDesc = sortMenu->addAction("按名称 (Z→A)");
    QObject::connect(sortDesc, &QAction::triggered, [onSortByName]() {
        if (onSortByName) onSortByName(false);
    });

    QAction* sortManual = sortMenu->addAction("手动");
    QObject::connect(sortManual, &QAction::triggered, [onSortManual]() {
        if (onSortManual) onSortManual();
    });
}

std::unique_ptr<QMenu> LibraryMenuBuilder::buildContextMenu(const QModelIndex& index, const MenuActions& actions, QWidget* parentWidget) {
    auto menu = std::make_unique<QMenu>(parentWidget);
    UiHelper::applyMenuStyle(menu.get());

    // 1. 空白处右键菜单
    if (!index.isValid()) {
        QAction* newCatAct = menu->addAction(UiHelper::getIcon("Data-Classification.svg", QColor("#EEEEEE")), "新建库分类");
        QObject::connect(newCatAct, &QAction::triggered, [actions]() {
            if (actions.onNewCategory) actions.onNewCategory();
        });

        buildSortSubMenu(menu.get(), actions.onSortByName, actions.onSortManual);
        return menu;
    }

    LibraryNodeKind kind = static_cast<LibraryNodeKind>(index.data(NodeKindRole).toInt());

    // 2. 固定系统分类（全部数据、未分类）：绝对禁止弹出修改菜单
    if (isSystemKind(kind)) {
        return nullptr;
    }

    // 3. 智能文件夹节点
    if (kind == LibraryNodeKind::SmartFolder) {
        int smartFolderId = index.data(SmartFolderIdRole).toInt();
        QString curIconKey = index.data(IconKeyRole).toString();
        QString curColorHex = index.data(ColorHexRole).toString();

        if (curIconKey.isEmpty() || curIconKey == "folder_filled" || curIconKey == "folder") curIconKey = "smart-folder-1.svg";
        if (curColorHex.isEmpty()) curColorHex = "#888888";

        QAction* editRuleAct = menu->addAction(UiHelper::getIcon("edit", QColor("#EEEEEE")), "编辑规则");
        QObject::connect(editRuleAct, &QAction::triggered, [actions, smartFolderId]() {
            if (actions.onEditSmartFolderRule) actions.onEditSmartFolderRule(smartFolderId);
        });
        menu->addSeparator();

        ContextMenuFactory::buildIconPickerMenu(menu.get(), curIconKey, curColorHex,
            [actions, smartFolderId](const QString& iconKey) {
                if (actions.onSmartFolderIconChanged) actions.onSmartFolderIconChanged(smartFolderId, iconKey);
            },
            [actions, smartFolderId](const QString& hexColor) {
                if (actions.onSmartFolderColorChanged) actions.onSmartFolderColorChanged(smartFolderId, hexColor);
            }
        );

        menu->addSeparator();

        QAction* deleteSmartAct = menu->addAction(UiHelper::getIcon("close", QColor("#EEEEEE")), "删除智能文件夹");
        QObject::connect(deleteSmartAct, &QAction::triggered, [actions, smartFolderId]() {
            if (actions.onDeleteSmartFolder) actions.onDeleteSmartFolder(smartFolderId);
        });

        return menu;
    }

    // 4. 三大分组标头
    if (kind == LibraryNodeKind::SectionQuickAccess) {
        QAction* clearAct = menu->addAction(UiHelper::getIcon("clear-2.svg", QColor("#EEEEEE")), "清空快速访问");
        QObject::connect(clearAct, &QAction::triggered, [actions]() {
            if (actions.onClearQuickAccess) actions.onClearQuickAccess();
        });
        return menu;
    }

    if (kind == LibraryNodeKind::SectionSmartFolder) {
        QAction* newSmartAct = menu->addAction(UiHelper::getIcon("smart-folder-1.svg", QColor("#EEEEEE")), "新建智能文件夹");
        QObject::connect(newSmartAct, &QAction::triggered, [actions]() {
            if (actions.onNewSmartFolder) actions.onNewSmartFolder();
        });
        return menu;
    }

    if (kind == LibraryNodeKind::SectionCustom) {
        QAction* newCatAct = menu->addAction(UiHelper::getIcon("Data-Classification.svg", QColor("#EEEEEE")), "新建库分类");
        QObject::connect(newCatAct, &QAction::triggered, [actions]() {
            if (actions.onNewCategory) actions.onNewCategory();
        });

        buildSortSubMenu(menu.get(), actions.onSortByName, actions.onSortManual);
        return menu;
    }

    // 5. 分类节点与置顶镜像节点
    if (kind == LibraryNodeKind::Category || kind == LibraryNodeKind::PinnedMirror) {
        int catId = index.data(CategoryIdRole).toInt();
        QString curIconKey = index.data(IconKeyRole).toString();
        QString curColorHex = index.data(ColorHexRole).toString();
        bool isPinned = index.data(PinnedRole).toBool();

        if (curIconKey.isEmpty() || curIconKey == "folder_filled" || curIconKey == "folder") curIconKey = "Data-Classification.svg";
        if (curColorHex.isEmpty()) curColorHex = "#888888";

        // 置顶 / 从快速访问移除
        if (isPinned) {
            QAction* unpinAct = menu->addAction(UiHelper::getIcon("pin_vertical", QColor("#EEEEEE")), "从“快速访问”移除");
            QObject::connect(unpinAct, &QAction::triggered, [actions, catId]() {
                if (actions.onPinCategory) actions.onPinCategory(catId, false);
            });
        } else {
            QAction* pinAct = menu->addAction(UiHelper::getIcon("pin_tilted", QColor("#EEEEEE")), "添加至“快速访问”");
            QObject::connect(pinAct, &QAction::triggered, [actions, catId]() {
                if (actions.onPinCategory) actions.onPinCategory(catId, true);
            });
        }

        menu->addSeparator();

        QAction* newSubCatAct = menu->addAction(UiHelper::getIcon("Data-Classification.svg", QColor("#EEEEEE")), "新建子分类");
        QObject::connect(newSubCatAct, &QAction::triggered, [actions, catId]() {
            if (actions.onNewSubCategory) actions.onNewSubCategory(catId);
        });

        QAction* presetTagAct = menu->addAction(UiHelper::getIcon("tag_filled", QColor("#EEEEEE")), "设置预设标签");
        QObject::connect(presetTagAct, &QAction::triggered, [actions, catId]() {
            if (actions.onSetPresetTags) actions.onSetPresetTags(catId);
        });

        menu->addSeparator();

        QPersistentModelIndex persistentIdx(index);
        ContextMenuFactory::buildIconPickerMenu(menu.get(), curIconKey, curColorHex,
            [actions, persistentIdx](const QString& iconKey) {
                if (actions.onCategoryIconChanged) actions.onCategoryIconChanged(persistentIdx, iconKey);
            },
            [actions, persistentIdx](const QString& hexColor) {
                if (actions.onCategoryColorChanged) actions.onCategoryColorChanged(persistentIdx, hexColor);
            }
        );

        QAction* renameAct = menu->addAction(UiHelper::getIcon("rename-1.svg", QColor("#EEEEEE")), "重命名");
        QObject::connect(renameAct, &QAction::triggered, [actions, index]() {
            if (actions.onRename) actions.onRename(index);
        });

        QAction* removeAct = menu->addAction(UiHelper::getIcon("close", QColor("#EEEEEE")), "删除分类");
        QObject::connect(removeAct, &QAction::triggered, [actions, catId]() {
            if (actions.onDeleteCategory) actions.onDeleteCategory(catId);
        });

        menu->addSeparator();

        buildSortSubMenu(menu.get(), actions.onSortByName, actions.onSortManual);
        return menu;
    }

    return nullptr;
}

} // namespace QuarkMeta
