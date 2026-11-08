#pragma once

#include <memory>

#include <QMenu>
#include <QModelIndex>
#include <functional>
#include "LibraryNodeKind.h"

namespace QuarkMeta {

/**
 * @brief 库树右键上下文菜单构建器
 * 统一构建空白处、三大分组标题、智能文件夹节点以及分类节点（含镜像节点）的上下文菜单。
 * 消除“排列”子菜单构造重复，本身不做业务，纯粹通过回调委托给宿主面板执行。
 */
class LibraryMenuBuilder {
public:
    struct MenuActions {
        std::function<void()> onNewCategory;
        std::function<void()> onNewSmartFolder;
        std::function<void(bool)> onSortByName;
        std::function<void()> onSortManual;
        std::function<void()> onClearQuickAccess;
        std::function<void(int)> onEditSmartFolderRule;
        std::function<void(int, const QString&)> onSmartFolderIconChanged;
        std::function<void(int, const QString&)> onSmartFolderColorChanged;
        std::function<void(int)> onDeleteSmartFolder;
        std::function<void(int, bool)> onPinCategory;
        std::function<void(int)> onNewSubCategory;
        std::function<void(int)> onSetPresetTags;
        std::function<void(const QPersistentModelIndex&, const QString&)> onCategoryIconChanged;
        std::function<void(const QPersistentModelIndex&, const QString&)> onCategoryColorChanged;
        std::function<void(const QModelIndex&)> onRename;
        std::function<void(int)> onDeleteCategory;
    };

    /**
     * @brief 构建“排列”子菜单单一真理源
     */
    static void buildSortSubMenu(QMenu* parentMenu, const std::function<void(bool)>& onSortByName, const std::function<void()>& onSortManual);

    /**
     * @brief 构建右键上下文菜单
     * @param index 当前右键命中的模型索引（若无效则代表点击在空白处）
     * @param actions 回调动作集合
     * @param parentWidget 菜单父控件
     * @return 组装完毕的 QMenu（调用方负责 exec）
     */
    static std::unique_ptr<QMenu> buildContextMenu(const QModelIndex& index, const MenuActions& actions, QWidget* parentWidget = nullptr);
};

} // namespace QuarkMeta
