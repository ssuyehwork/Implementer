#pragma once

#include <Qt>
#include <QString>
#include <QStandardItem>
#include "../core/ModelContract.h"

namespace QuarkMeta {

/**
 * @brief 库节点类型枚举（唯一真理源）
 */
enum class LibraryNodeKind {
    SystemAll,              // 固定系统项：全部数据
    SystemUncategorized,    // 固定系统项：未分类
    SectionQuickAccess,     // 分组标头：快速访问
    SectionSmartFolder,     // 分组标头：智能文件夹
    SectionCustom,          // 分组标头：文件夹（自定义分类主分组）
    Category,               // 自定义分类真实节点
    SmartFolder,            // 智能文件夹节点
    PinnedMirror            // 快速访问分组下的置顶镜像节点
};

// 命名常量：仅用于调用 DAO/Service 的既有底层接口契约
constexpr int kSystemAllId = -1;
constexpr int kSystemUncategorizedId = -2;
constexpr int kParentIdSmartFolderSection = -20;

/**
 * @brief 库树模型独立数据角色（起始值 500，严格避开 ModelContract.h 中的所有已有角色）
 * 注：置顶状态与计数直接复用 ModelContract.h 中的 SSOT 角色定义（PinnedRole 与 CountRole），避免重定义冲突。
 */
enum LibraryItemRole {
    NodeKindRole       = Qt::UserRole + 500, // 节点种类 (int, 对应 static_cast<int>(LibraryNodeKind))
    CategoryIdRole     = Qt::UserRole + 501, // 分类及镜像节点的分类 ID (int)
    SmartFolderIdRole  = Qt::UserRole + 502, // 智能文件夹专属 ID (int)
    IconKeyRole        = Qt::UserRole + 503, // 矢量图标标识 (QString)
    ColorHexRole       = Qt::UserRole + 504  // 颜色代码 Hex (QString)
};

/**
 * @brief 是否为三大分组标头之一
 */
inline bool isSectionKind(LibraryNodeKind kind) {
    return kind == LibraryNodeKind::SectionQuickAccess ||
           kind == LibraryNodeKind::SectionSmartFolder ||
           kind == LibraryNodeKind::SectionCustom;
}

/**
 * @brief 是否为系统固定分类（全部数据、未分类）
 */
inline bool isSystemKind(LibraryNodeKind kind) {
    return kind == LibraryNodeKind::SystemAll ||
           kind == LibraryNodeKind::SystemUncategorized;
}

/**
 * @brief 是否为用户可交互的常规节点（等价于原来的 nodeId > 0：Category、SmartFolder、PinnedMirror）
 */
inline bool isUserNodeKind(LibraryNodeKind kind) {
    return kind == LibraryNodeKind::Category ||
           kind == LibraryNodeKind::SmartFolder ||
           kind == LibraryNodeKind::PinnedMirror;
}

/**
 * @brief 分组标头显示文本唯一映射
 */
inline QString sectionTitle(LibraryNodeKind kind, int count) {
    switch (kind) {
    case LibraryNodeKind::SectionQuickAccess:
        return QString("快速访问(%1)").arg(count);
    case LibraryNodeKind::SectionSmartFolder:
        return QString("智能文件夹(%1)").arg(count);
    case LibraryNodeKind::SectionCustom:
        return QString("文件夹(%1)").arg(count);
    default:
        return QString();
    }
}

/**
 * @brief 分组折叠状态持久化配置键唯一映射
 */
inline QString sectionCollapsedConfigKey(LibraryNodeKind kind) {
    switch (kind) {
    case LibraryNodeKind::SectionQuickAccess:
        return QStringLiteral("Library/QuickAccessSectionCollapsed");
    case LibraryNodeKind::SectionSmartFolder:
        return QStringLiteral("Library/SmartFolderSectionCollapsed");
    case LibraryNodeKind::SectionCustom:
        return QStringLiteral("Library/CustomSectionCollapsed");
    default:
        return QString();
    }
}

/**
 * @brief 库树节点查找统一入口
 * 通过 kind 与业务 ID 精确匹配节点，杜绝同 ID 冲突与类型误伤
 */
inline QStandardItem* findNode(QStandardItem* parent, LibraryNodeKind kind, int id = 0) {
    if (!parent) return nullptr;
    int targetKindInt = static_cast<int>(kind);
    for (int i = 0; i < parent->rowCount(); ++i) {
        QStandardItem* child = parent->child(i);
        if (!child) continue;

        int childKindInt = child->data(NodeKindRole).toInt();
        if (childKindInt == targetKindInt) {
            if (isSectionKind(kind)) {
                return child;
            } else if (kind == LibraryNodeKind::SystemAll || kind == LibraryNodeKind::SystemUncategorized) {
                return child;
            } else if (kind == LibraryNodeKind::SmartFolder) {
                if (child->data(SmartFolderIdRole).toInt() == id) {
                    return child;
                }
            } else if (kind == LibraryNodeKind::Category || kind == LibraryNodeKind::PinnedMirror) {
                if (child->data(CategoryIdRole).toInt() == id) {
                    return child;
                }
            }
        }

        QStandardItem* found = findNode(child, kind, id);
        if (found) return found;
    }
    return nullptr;
}

} // namespace QuarkMeta
