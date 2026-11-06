#pragma once

#include <QWidget>
#include <QEvent>
#include <QPushButton>
#include "BreadcrumbBar.h"
#include "AddressHistoryPanel.h"

namespace RapidNotes {

/**
 * @brief 纯分类导航复合地址栏控件
 * 彻底废除 QLineEdit 文本输入态，纯粹作为结构化分类面包屑（BreadcrumbBar）的展示与交互容器；
 * 双击空白处弹出分类历史记录面板（AddressHistoryPanel）。
 */
class AddressBar : public QWidget {
    Q_OBJECT

public:
    explicit AddressBar(QWidget* parent = nullptr);
    ~AddressBar() override = default;

    void setPath(const QString& path);
    QString currentPath() const { return m_currentPath; }

signals:
    void pathChanged(const QString& path);
    void refreshRequested();

private slots:
    void onBreadcrumbBlankClicked();
    void onBreadcrumbClicked(const QString& path);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    QWidget*             m_addressContainer = nullptr;
    BreadcrumbBar*       m_breadcrumbBar    = nullptr;
    QPushButton*         m_btnRefresh       = nullptr;
    AddressHistoryPanel* m_historyPanel     = nullptr;
    QString              m_currentPath;
};

} // namespace RapidNotes
