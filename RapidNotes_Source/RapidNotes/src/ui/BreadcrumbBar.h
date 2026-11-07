#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStringList>

namespace QuarkMeta {

/**
 * @brief 面包屑导航栏部件
 * 将路径拆分为层级按钮，支持点击跳转；自感知超长截断状态
 */
class BreadcrumbNodeButton : public QPushButton {
    Q_OBJECT
public:
    explicit BreadcrumbNodeButton(const QString& name, const QString& fullPath, QWidget* parent = nullptr);
    const QString& fullPath() const { return m_fullPath; }

signals:
    void pathsDroppedNode(const QStringList& paths, const QString& targetPath);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    QString m_fullPath;
};

class BreadcrumbBar : public QWidget {
    Q_OBJECT

public:
    explicit BreadcrumbBar(QWidget* parent = nullptr);
    ~BreadcrumbBar() override = default;

    /**
     * @brief 设置当前显示路径并刷新按钮
     */
    void setPath(const QString& path);

    /**
     * @brief 当前路径是否因超长而被截断省略（显示了 "..."）
     */
    bool isPathElided() const { return m_isElided; }

signals:
    /**
     * @brief 用户点击某个层级按钮时发出
     * @param path 该层级对应的完整物理路径
     */
    void pathClicked(const QString& path);

    /**
     * @brief 当用户点击空白区域时发出，用于告知外部切换到编辑模式
     */
    void blankAreaClicked();

    /**
     * @brief 用户右键点击某个层级按钮时发出，用于告知外部触发收藏菜单
     */
    void favoriteToggleRequested(const QString& fullPath, const QPoint& globalPos);
    void pathsDroppedNode(const QStringList& paths, const QString& targetPath);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    struct PathNode {
        QString name;
        QString fullPath;

        PathNode(const QString& n, const QString& p) : name(n), fullPath(p) {}
    };

    void clearButtons();
    void addLevel(const QString& name, const QString& fullPath);
    void rebuildBreadcrumbs();

    QHBoxLayout* m_layout = nullptr;
    QString m_currentPath;
    QList<PathNode> m_nodes;
    bool m_isElided = false; // 标记当前排版是否处于超长截断状态
};

} // namespace QuarkMeta