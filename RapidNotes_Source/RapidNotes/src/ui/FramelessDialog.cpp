#include "FramelessDialog.h"
#include "FramelessWindowHelper.h"
#include "UiHelper.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QShortcut>
#include <QKeySequence>
#include <QCheckBox>
#include <QLineEdit>
#include <QSettings>
#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif

namespace RapidNotes {


FramelessDialog::FramelessDialog(const QString& title, QWidget* parent) 
    : QDialog(parent, Qt::FramelessWindowHint | Qt::Window) 
{
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);

    setWindowTitle(title);

#ifdef Q_OS_WIN
    DWORD attribute = 2; // DWMWCP_ROUND
    DwmSetWindowAttribute(reinterpret_cast<HWND>(winId()), 33, &attribute, sizeof(attribute));
#endif

    m_outerLayout = new QVBoxLayout(this);
    m_outerLayout->setContentsMargins(0, 0, 0, 0);

    m_container = new QWidget(this);
    m_container->setObjectName("DialogContainer");
    m_container->setAttribute(Qt::WA_StyledBackground);
    m_outerLayout->addWidget(m_container);

    m_mainLayout = new QVBoxLayout(m_container);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    auto* titleBar = new QWidget();
    titleBar->setObjectName("TitleBar");
    titleBar->setFixedHeight(34);
    titleBar->setObjectName("FramelessTitleBar");
    m_titleLayout = new QHBoxLayout(titleBar);
    m_titleLayout->setContentsMargins(12, 0, 5, 0);
    m_titleLayout->setSpacing(4);

    m_titleLabel = new QLabel(title);
    m_titleLabel->setObjectName("FramelessTitleLabel");
    m_titleLayout->addWidget(m_titleLabel);
    m_titleLayout->addStretch();

    auto createTitleBtn = [this](const QString& iconName, const QString& tooltip, const QString& hoverColor) {
        Q_UNUSED(hoverColor);
        QPushButton* btn = new QPushButton();
        btn->setFixedSize(20, 20);
        btn->setIcon(UiHelper::getIcon(iconName, QColor("#CCCCCC"), 16));
        btn->setIconSize(QSize(16, 16));
        btn->setAutoDefault(false);
        btn->setProperty("tooltipText", tooltip);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setObjectName("FramelessTitleBtn");
        btn->installEventFilter(this);
        return btn;
    };

    m_pinBtn = createTitleBtn("pin_tilted", "置顶", "#3E3E42");
    m_pinBtn->setCheckable(true);
    m_pinBtn->setObjectName("FramelessPinBtn");
    connect(m_pinBtn, &QPushButton::toggled, this, [this](bool checked) {
        m_pinBtn->setIcon(UiHelper::getIcon(checked ? "pin_vertical" : "pin_tilted", 
                                            checked ? QColor("#FF551C") : QColor("#CCCCCC"), 18));
#ifdef Q_OS_WIN
        HWND hwnd = reinterpret_cast<HWND>(winId());
        SetWindowPos(hwnd, checked ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
#else
        setWindowFlag(Qt::WindowStaysOnTopHint, checked);
        show();
#endif
    });

    m_minBtn = createTitleBtn("minimize", "最小化", "#3E3E42");
    connect(m_minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);

    m_maxBtn = createTitleBtn("maximize", "最大化", "#3E3E42");
    connect(m_maxBtn, &QPushButton::clicked, this, [this]() {
        if (isMaximized()) {
            showNormal();
            m_maxBtn->setIcon(UiHelper::getIcon("maximize", QColor("#CCCCCC"), 18));
        } else {
            showMaximized();
            m_maxBtn->setIcon(UiHelper::getIcon("restore_line", QColor("#CCCCCC"), 18));
        }
    });

    m_closeBtn = new QPushButton();
    m_closeBtn->setFixedSize(20, 20);
    m_closeBtn->setIcon(UiHelper::getIcon("close", QColor("#FFFFFF"), 16));
    m_closeBtn->setIconSize(QSize(16, 16));
    m_closeBtn->setAutoDefault(false);
    m_closeBtn->setProperty("tooltipText", "关闭");
    m_closeBtn->setCursor(Qt::PointingHandCursor);
    m_closeBtn->setObjectName("FramelessCloseBtn");
    m_closeBtn->installEventFilter(this);
    connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::reject);

    m_titleLayout->addWidget(m_pinBtn);
    m_titleLayout->addWidget(m_minBtn);
    m_titleLayout->addWidget(m_maxBtn);
    m_titleLayout->addWidget(m_closeBtn);

    m_mainLayout->addWidget(titleBar);
    m_mainLayout->addSpacing(4);

    auto* line = new QFrame();
    line->setFixedHeight(1);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    line->setObjectName("FramelessLine");
    m_mainLayout->addWidget(line);

    m_contentArea = new QWidget();
    m_contentArea->setObjectName("DialogContentArea");
    m_mainLayout->addWidget(m_contentArea, 1);

    // 默认仅显示关闭按钮，全局自动隐匿所有无边框对话框的置顶功能
    setVisibleButtons(Close);

    // [归一化] 8 向拖拽缩放统一接入 FramelessWindowHelper（SSOT），取代各子类 ResizeHandle 等私有实现。
    // 与 QuickWindow 同理：必须在全部界面构件构造完成后接入
    m_framelessHelper = FramelessWindowHelper::apply(this, nullptr);
}

void FramelessDialog::setVisibleButtons(int flags) {
    if (m_pinBtn) m_pinBtn->setVisible(flags & Pin);
    if (m_minBtn) m_minBtn->setVisible(flags & Min);
    if (m_maxBtn) m_maxBtn->setVisible(flags & Max);
    if (m_closeBtn) m_closeBtn->setVisible(flags & Close);
}

void FramelessDialog::loadWindowSettings() {
    if (objectName().isEmpty()) return;
    QSettings settings("RapidNotes", "WindowStates");
    bool stay = settings.value(objectName() + "/StayOnTop", false).toBool();
    if (m_pinBtn) {
        m_pinBtn->blockSignals(true);
        m_pinBtn->setChecked(stay);
        m_pinBtn->setIcon(UiHelper::getIcon(stay ? "pin_vertical" : "pin_tilted", 
                                            stay ? QColor("#FF551C") : QColor("#CCCCCC"), 18));
        m_pinBtn->blockSignals(false);
    }
#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(winId());
    SetWindowPos(hwnd, stay ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
#else
    setWindowFlag(Qt::WindowStaysOnTopHint, stay);
#endif
}

void FramelessDialog::saveWindowSettings() {
    if (objectName().isEmpty()) return;
    QSettings settings("RapidNotes", "WindowStates");
    bool stay = m_pinBtn ? m_pinBtn->isChecked() : false;
    settings.setValue(objectName() + "/StayOnTop", stay);
}

FramelessDialog::~FramelessDialog() = default;

void FramelessDialog::showEvent(QShowEvent* event) {
    QDialog::showEvent(event);
}

void FramelessDialog::hideEvent(QHideEvent* event) {
    QDialog::hideEvent(event);
}

void FramelessDialog::closeEvent(QCloseEvent* event) {
    QDialog::closeEvent(event);
}

namespace {
bool isInteractiveWidget(QWidget* widget) {
    while (widget) {
        if (qobject_cast<QAbstractButton*>(widget) ||
            qobject_cast<QLineEdit*>(widget) ||
            qobject_cast<QCheckBox*>(widget)) {
            return true;
        }
        widget = widget->parentWidget();
    }
    return false;
}
} // namespace

void FramelessDialog::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QWidget* child = childAt(event->pos());
        if (!child || !isInteractiveWidget(child)) {
#ifdef Q_OS_WIN
            ReleaseCapture();
            ::SendMessageW(reinterpret_cast<HWND>(winId()), WM_NCLBUTTONDOWN, HTCAPTION, 0);
            event->accept();
            return;
#else
            m_isDragging = true;
            m_dragPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
            return;
#endif
        }
    }
    QDialog::mousePressEvent(event);
}

void FramelessDialog::mouseMoveEvent(QMouseEvent* event) {
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragPos);
        event->accept();
        return;
    }
    QDialog::mouseMoveEvent(event);
}

void FramelessDialog::mouseReleaseEvent(QMouseEvent* event) {
    m_isDragging = false;
    QDialog::mouseReleaseEvent(event);
}

void FramelessDialog::keyPressEvent(QKeyEvent* event) {
    if ((event->key() == Qt::Key_W && (event->modifiers() & Qt::ControlModifier))) {
        reject();
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Escape) {
        reject();
    } else {
        QDialog::keyPressEvent(event);
    }
}

bool FramelessDialog::eventFilter(QObject* watched, QEvent* event) {
    return QDialog::eventFilter(watched, event);
}

bool FramelessDialog::nativeEvent(const QByteArray& eventType, void* message, qintptr* result) {
    // [归一化] WM_NCHITTEST/WM_NCCALCSIZE/WM_GETMINMAXINFO/WM_SETCURSOR 统一走 Helper（SSOT）
    if (m_framelessHelper && m_framelessHelper->handleNativeEvent(message, result)) {
        return true;
    }
    return QDialog::nativeEvent(eventType, message, result);
}

} // namespace RapidNotes
