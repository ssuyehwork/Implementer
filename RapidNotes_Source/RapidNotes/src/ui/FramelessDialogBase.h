#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QPoint>
#include <QColor>
#include <QShowEvent>
#include <QHideEvent>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QEvent>

namespace QuarkMeta {

class FramelessDialog : public QDialog {
    Q_OBJECT
public:
    enum DialogButton { Pin = 1, Min = 2, Max = 4, Close = 8, All = 15 };
    explicit FramelessDialog(const QString& title, QWidget* parent = nullptr);
    ~FramelessDialog() override;

    QWidget* getContentArea() const { return m_contentArea; }
    void setVisibleButtons(int flags);

protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

    QWidget* m_contentArea;
    QVBoxLayout* m_mainLayout;
    QVBoxLayout* m_outerLayout;
    QHBoxLayout* m_titleLayout;
    QWidget* m_container;
    QLabel* m_titleLabel;
    QPushButton* m_pinBtn;
    QPushButton* m_minBtn;
    QPushButton* m_maxBtn;
    QPushButton* m_closeBtn;

private:
    QPoint m_dragPos;
    bool m_isDragging = false;
};

} // namespace QuarkMeta
