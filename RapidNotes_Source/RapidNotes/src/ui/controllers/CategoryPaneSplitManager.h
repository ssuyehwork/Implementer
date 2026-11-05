#pragma once

#include <QObject>
#include <QSplitter>
#include <QList>
#include <QPointer>
#include <QWidget>
#include "../CategoryTabBarWidget.h"

namespace RapidNotes {

class CategoryContentPane;

/**
 * @brief 面向数据库分类的多窗格分屏管理器
 * 负责横向/纵向拆分窗格，支持多分类并排浏览
 * 彻底排除物理文件夹与 Windows Shell 依赖
 */
class CategoryPaneSplitManager : public QObject {
    Q_OBJECT

public:
    explicit CategoryPaneSplitManager(QWidget* containerWidget, QObject* parent = nullptr);
    ~CategoryPaneSplitManager() override = default;

    bool isSplitMode() const { return m_panes.size() > 1; }
    Qt::Orientation splitOrientation() const { return m_orientation; }

    int paneCount() const { return m_panes.size(); }
    QList<QWidget*> panes() const { return m_panes; }
    QWidget* activePane() const { return m_activePane.data(); }
    void setActivePane(QWidget* pane);

    void split(Qt::Orientation orientation, int newCategoryId, const QString& title);
    void closeSecondaryPanes();

    CategoryTabSplitState captureSplitState() const;
    void restoreSplitState(const CategoryTabSplitState& state);

signals:
    void activePaneChanged(QWidget* pane);
    void paneCountChanged(int count);

private:
    QPointer<QWidget> m_containerWidget;
    QPointer<QSplitter> m_splitter;
    QList<QWidget*> m_panes;
    QPointer<QWidget> m_activePane;
    Qt::Orientation m_orientation = Qt::Horizontal;
};

} // namespace RapidNotes
