#pragma once

#include <QObject>
#include <QEvent>

class QAbstractItemView;

namespace QuarkMeta {

class ContentPanel;

/**
 * @brief 内容面板事件与热键独立拦截器
 * 负责缩放滚轮、星级/颜色快捷键、空格 QuickLook 准入过滤、委托 Hitbox 命中检测
 */
class ContentKeyHandler : public QObject {
    Q_OBJECT
public:
    explicit ContentKeyHandler(ContentPanel* panel, QObject* parent = nullptr);
    ~ContentKeyHandler() override = default;

    /**
     * @brief 核心事件路由总入口
     */
    bool handleEvent(QObject* obj, QEvent* event);

    /**
     * @brief 视图首项导航 (Ctrl+Home 选中并滚动，跳过分组头)
     */
    static bool navigateViewFirstItem(QAbstractItemView* view);

    /**
     * @brief 视图末项导航 (Ctrl+End 选中并滚动，跳过分组头)
     */
    static bool navigateViewLastItem(QAbstractItemView* view);

    /**
     * @brief 执行快捷移入/重复移入到目标文件夹
     */
    static bool executeMoveToFolder(ContentPanel* panel, const QString& targetDir);

    /**
     * @brief 重复上一次操作 SSOT 统一入口 (快捷键 F4 与 右键菜单 "重复上一次操作" 共同调用)
     */
    static bool executeRepeatLastOp(ContentPanel* panel, QAbstractItemView* view);

private:
    bool handleWheel(QObject* obj, QEvent* event);
    bool handleMousePress(QObject* obj, QEvent* event);
    bool handleKeyPress(QObject* obj, QEvent* event);
    bool handleTooltipHover(QObject* obj, QEvent* event);

    ContentPanel* m_panel = nullptr;
};

} // namespace QuarkMeta
