#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStringList>

namespace RapidNotes {

/**
 * @brief 结构化面包屑节点按钮
 */
class BreadcrumbNodeButton : public QPushButton {
    Q_OBJECT
public:
    explicit BreadcrumbNodeButton(int id, const QString& name, const QString& fullUrl, QWidget* parent = nullptr);
    int categoryId() const { return m_id; }
    const QString& fullUrl() const { return m_fullUrl; }

private:
    int m_id = -1;
    QString m_fullUrl;
};

/**
 * @brief 纯分类结构化面包屑导航部件
 * 彻底剔除盘符拆分与物理分隔符切割，自底向上递归根据 parent_id 链构建结构化层级节点
 */
class BreadcrumbBar : public QWidget {
    Q_OBJECT

public:
    struct PathNode {
        int id = -1;
        QString name;
        QString url;

        PathNode(int i, const QString& n, const QString& u) : id(i), name(n), url(u) {}
    };

    explicit BreadcrumbBar(QWidget* parent = nullptr);
    ~BreadcrumbBar() override = default;

    /**
     * @brief 设置当前显示 URL (如 library://1 或 library://-1) 并递归构建结构化层级
     */
    void setPath(const QString& url);

    /**
     * @brief 当前路径是否因超长而被截断省略（显示了 "..."）
     */
    bool isPathElided() const { return m_isElided; }

signals:
    /**
     * @brief 用户点击某个分类层级按钮时发出
     * @param url 该层级对应的协议 URL (如 library://<id>)
     */
    void pathClicked(const QString& url);

    /**
     * @brief 点击空白区域触发（用于外部弹出历史记录面板等）
     */
    void blankAreaClicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void clearButtons();
    void addLevel(int id, const QString& name, const QString& url);
    void rebuildBreadcrumbs();

    QHBoxLayout* m_layout = nullptr;
    QString m_currentUrl;
    QList<PathNode> m_nodes;
    bool m_isElided = false;
};

} // namespace RapidNotes
