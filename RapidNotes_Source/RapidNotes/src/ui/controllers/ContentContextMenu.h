#pragma once

#include <QObject>
#include <QPoint>
#include <QAbstractItemView>

namespace QuarkMeta {

class ContentPanel;

/**
 * @brief 内容面板右键菜单独立处理器
 * 封装“回收站”、“盘符根目录”、“普通文件/目录”、“空白处”及“排序”全部上下文菜单
 */
class ContentContextMenu : public QObject {
    Q_OBJECT
public:
    explicit ContentContextMenu(ContentPanel* parentPanel);
    ~ContentContextMenu() override = default;

    /**
     * @brief 弹出并执行右键菜单
     * @param view 触发菜单的视图（GridView 或 TreeView）
     * @param pos 视图视口内的点击物理坐标
     */
    void showMenu(QAbstractItemView* view, const QPoint& pos);

private:
    ContentPanel* m_panel = nullptr;
};

} // namespace QuarkMeta
