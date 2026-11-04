#include <QApplication>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QTimer>
#include <QDateTime>
#include <QShortcut>
#include <QClipboard>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>
#include <QVector>
#include <QSettings>
#include <QDebug>
#include <QLineEdit>
#include <QIcon>
#include <QKeyEvent>
#include <QSvgRenderer>
#include <QProcess>
#include <QRegularExpression>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

#include "SvgIcons.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <windowsx.h>
#include <winioctl.h>
#endif

// ============================================================================
// Icon Helper (严格遵循 SSOT: 唯一来源 SvgIcons.h)
// ============================================================================
static QIcon iconFromSvg(const QString& name, const QString& color = "", const QSize& size = QSize(32, 32)) {
    QString svg = SvgIcons::getSvg(name);
    if (svg.isEmpty()) return QIcon();

    if (!color.isEmpty() && svg.contains("currentColor")) {
        svg.replace("currentColor", color);
    }

    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QSvgRenderer renderer(svg.toUtf8());
    if (renderer.isValid()) {
        QPainter painter(&pixmap);
        renderer.render(&painter);
    }
    return QIcon(pixmap);
}

// ============================================================================
// KeyboardHook: 全局按键拦截 (支持组合键)
// ============================================================================
struct HotkeyConfig {
    int vk = 0;
    bool ctrl = false;
    bool alt = false;
    bool shift = false;
    QString name = "F1";

    bool operator==(const HotkeyConfig& other) const {
        return vk == other.vk && ctrl == other.ctrl && alt == other.alt && shift == other.shift;
    }
};
Q_DECLARE_METATYPE(HotkeyConfig)

class KeyboardHook : public QObject {
    Q_OBJECT
public:
    static KeyboardHook& instance() {
        static KeyboardHook inst;
        return inst;
    }

    void start() {
#ifdef Q_OS_WIN
        if (m_hHook) return;
        m_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, HookProc, GetModuleHandle(NULL), 0);
#endif
    }

    void stop() {
#ifdef Q_OS_WIN
        if (m_hHook) {
            UnhookWindowsHookEx(m_hHook);
            m_hHook = nullptr;
        }
#endif
    }

    void setDigitInterceptEnabled(bool enabled) { m_digitInterceptEnabled = enabled; }
    void setPaused(bool paused) { m_isPaused = paused; }
    bool isPaused() const { return m_isPaused; }
    
    void setHotkey(const HotkeyConfig& config) { m_hotkey = config; }
    HotkeyConfig hotkey() const { return m_hotkey; }

    void setRecordingMode(bool recording) { 
        m_isRecording = recording; 
        if (recording) {
            m_ctrlPressed = (GetAsyncKeyState(VK_CONTROL) & 0x8000);
            m_altPressed = (GetAsyncKeyState(VK_MENU) & 0x8000);
            m_shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
        }
    }

signals:
    void digitPressed(int digit);
    void pauseToggled();
    void hotkeyRecorded(HotkeyConfig config);
    void exitRequested();

private:
    KeyboardHook() : m_hHook(nullptr), m_digitInterceptEnabled(false), m_isPaused(false), m_isRecording(false),
                     m_ctrlPressed(false), m_altPressed(false), m_shiftPressed(false) {
        m_hotkey.vk = VK_F1;
        m_hotkey.name = "F1";
    }
    ~KeyboardHook() { stop(); }

#ifdef Q_OS_WIN
    static LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode == HC_ACTION) {
            KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
            bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);

            if (pKey->vkCode == VK_LCONTROL || pKey->vkCode == VK_RCONTROL || pKey->vkCode == VK_CONTROL) {
                KeyboardHook::instance().m_ctrlPressed = isKeyDown;
            } else if (pKey->vkCode == VK_LMENU || pKey->vkCode == VK_RMENU || pKey->vkCode == VK_MENU) {
                KeyboardHook::instance().m_altPressed = isKeyDown;
            } else if (pKey->vkCode == VK_LSHIFT || pKey->vkCode == VK_RSHIFT || pKey->vkCode == VK_SHIFT) {
                KeyboardHook::instance().m_shiftPressed = isKeyDown;
            }
            
            bool ctrl = KeyboardHook::instance().m_ctrlPressed;
            bool alt = KeyboardHook::instance().m_altPressed;
            bool shift = KeyboardHook::instance().m_shiftPressed;

            if (!(pKey->flags & LLKHF_INJECTED)) {
                if (KeyboardHook::instance().m_isRecording) {
                    if (isKeyDown) {
                        if (pKey->vkCode != VK_CONTROL && pKey->vkCode != VK_LCONTROL && pKey->vkCode != VK_RCONTROL &&
                            pKey->vkCode != VK_MENU && pKey->vkCode != VK_LMENU && pKey->vkCode != VK_RMENU &&
                            pKey->vkCode != VK_SHIFT && pKey->vkCode != VK_LSHIFT && pKey->vkCode != VK_RSHIFT) 
                        {
                            HotkeyConfig cfg;
                            cfg.vk = pKey->vkCode;
                            cfg.ctrl = ctrl;
                            cfg.alt = alt;
                            cfg.shift = shift;
                            
                            QStringList parts;
                            if (cfg.ctrl) parts << "Ctrl";
                            if (cfg.alt) parts << "Alt";
                            if (cfg.shift) parts << "Shift";
                            
                            QString name;
                            if (cfg.vk >= 'A' && cfg.vk <= 'Z') name = QString(QChar(cfg.vk));
                            else if (cfg.vk >= '0' && cfg.vk <= '9') name = QString(QChar(cfg.vk));
                            else if (cfg.vk >= VK_F1 && cfg.vk <= VK_F24) name = QString("F%1").arg(cfg.vk - VK_F1 + 1);
                            else {
                                switch(cfg.vk) {
                                    case VK_SPACE: name = "Space"; break;
                                    case VK_RETURN: name = "Enter"; break;
                                    case VK_ESCAPE: name = "Esc"; break;
                                    case VK_OEM_3: name = "~"; break;
                                    default: name = QString("Key_%1").arg(cfg.vk); break;
                                }
                            }
                            parts << name;
                            cfg.name = parts.join("+");
                            
                            emit KeyboardHook::instance().hotkeyRecorded(cfg);
                        }
                    }
                    return 1; 
                }

                if (ctrl && pKey->vkCode == 'Q' && isKeyDown) {
                    emit KeyboardHook::instance().exitRequested();
                    return 1;
                }

                const auto& target = KeyboardHook::instance().m_hotkey;
                if (pKey->vkCode == target.vk && ctrl == target.ctrl && alt == target.alt && shift == target.shift) {
                    if (isKeyDown) {
                        emit KeyboardHook::instance().pauseToggled();
                    }
                    return 1;
                }

                if (KeyboardHook::instance().m_digitInterceptEnabled && !KeyboardHook::instance().m_isPaused) {
                    if (pKey->vkCode >= 0x30 && pKey->vkCode <= 0x39 && !ctrl && !alt) {
                        if (isKeyDown) {
                            int digit = pKey->vkCode - 0x30;
                            emit KeyboardHook::instance().digitPressed(digit);
                        }
                        return 1;
                    }
                }
            }
        }
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }
    HHOOK m_hHook;
#endif
    bool m_digitInterceptEnabled;
    bool m_isPaused;
    HotkeyConfig m_hotkey;
    bool m_isRecording;
    bool m_ctrlPressed, m_altPressed, m_shiftPressed;
};

// ============================================================================
// StateIconBtn: 状态图标按钮 (标题栏专用)
// ============================================================================
class StateIconBtn : public QPushButton {
    Q_OBJECT
public:
    explicit StateIconBtn(QWidget* parent = nullptr) : QPushButton(parent), m_isPaused(false) {
        setFixedSize(24, 24);
        setCursor(Qt::PointingHandCursor);
        setStyleSheet("QPushButton { background-color: transparent; border: none; }");
    }

    void setPaused(bool paused) {
        m_isPaused = paused;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        if (isDown()) {
            painter.setBrush(QColor("#555555"));
        } else if (underMouse()) {
            painter.setBrush(QColor("#444444"));
        } else {
            painter.setBrush(Qt::transparent);
        }
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect(), 4, 4);

        int side = 18;
        int x = (width() - side) / 2;
        int y = (height() - side) / 2;

        painter.setBrush(QColor("#00BFA5"));
        if (m_isPaused) {
            QPainterPath path;
            path.moveTo(x, y);
            path.lineTo(x + side, y + side / 2);
            path.lineTo(x, y + side);
            path.closeSubpath();
            painter.drawPath(path);
        } else {
            painter.drawRect(x, y, 5, side);
            painter.drawRect(x + side - 5, y, 5, side);
        }
    }

private:
    bool m_isPaused;
};

// ============================================================================
// HotkeyEdit: 专业热键编辑器 (仅可读取，高度32px，平时无边框，点击时显示2px白边)
// ============================================================================
class HotkeyEdit : public QLineEdit {
    Q_OBJECT
public:
    explicit HotkeyEdit(QWidget* parent = nullptr) : QLineEdit(parent) {
        setReadOnly(true);
        setAlignment(Qt::AlignCenter);
        setFixedHeight(32); // 固定高度 32 像素
        setCursor(Qt::PointingHandCursor);
        setStyleSheet(
            "QLineEdit {"
            "  color: #00BFA5;"
            "  background-color: transparent;" // 平时无背景底色
            "  border: none;"                  // 平时完全不显示边框
            "  font-size: 13px;"
            "  font-weight: bold;"
            "}"
            "QLineEdit:focus {"
            "  border: 2px solid #FFFFFF;"     // 点击处于编辑态时，显示 2 像素白色边框
            "  border-radius: 4px;"
            "  background-color: #242424;"
            "}"
        );
        connect(&KeyboardHook::instance(), &KeyboardHook::hotkeyRecorded, this, &HotkeyEdit::onGlobalKeyRecorded);
    }

signals:
    void hotkeyChanged(HotkeyConfig config);

private slots:
    void onGlobalKeyRecorded(HotkeyConfig cfg) {
        if (!hasFocus()) return;
        setText(cfg.name); // 按下快捷键之后，显示框里显示相应的快捷键
        emit hotkeyChanged(cfg);
        clearFocus(); // 设定完毕自动失焦，边框恢复隐藏
    }

protected:
    void mousePressEvent(QMouseEvent* event) override {
        setFocus();
        QLineEdit::mousePressEvent(event);
    }

    void keyPressEvent(QKeyEvent* event) override {
        if (event->key() == Qt::Key_Escape) clearFocus();
    }

    void focusInEvent(QFocusEvent* event) override {
        QLineEdit::focusInEvent(event);
        // 显示“自定义快捷键”，字体为青色
        setText("自定义快捷键");
#ifdef Q_OS_WIN
        HWND hwnd = (HWND)window()->winId();
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, GetWindowLongPtr(hwnd, GWL_EXSTYLE) & ~WS_EX_NOACTIVATE);
        SetForegroundWindow(hwnd);
        SetActiveWindow(hwnd);
#endif
        KeyboardHook::instance().setRecordingMode(true);
    }

    void focusOutEvent(QFocusEvent* event) override {
        QLineEdit::focusOutEvent(event);
        KeyboardHook::instance().setRecordingMode(false);
        HotkeyConfig cfg = KeyboardHook::instance().hotkey();
        setText(cfg.name);
#ifdef Q_OS_WIN
        HWND hwnd = (HWND)window()->winId();
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, GetWindowLongPtr(hwnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);
#endif
    }
};

// ============================================================================
// FramelessDialog: 基础无边框窗口 (SSOT: 图标统一来自 SvgIcons.h)
// ============================================================================
class FramelessDialog : public QDialog {
    Q_OBJECT
public:
    explicit FramelessDialog(const QString& title, QWidget* parent = nullptr) 
        : QDialog(parent, Qt::FramelessWindowHint | Qt::Window) 
    {
        setAttribute(Qt::WA_TranslucentBackground);
        
        m_outerLayout = new QVBoxLayout(this);
        m_outerLayout->setContentsMargins(12, 12, 12, 12);

        m_container = new QWidget(this);
        m_container->setObjectName("DialogContainer");
        m_container->setStyleSheet(
            "#DialogContainer {"
            "  background-color: #1e1e1e;"
            "  border: 1px solid #333333;"
            "  border-radius: 8px;"
            "}"
        );
        m_outerLayout->addWidget(m_container);

        auto* shadow = new QGraphicsDropShadowEffect(this);
        shadow->setBlurRadius(15);
        shadow->setXOffset(0); shadow->setYOffset(2);
        shadow->setColor(QColor(0, 0, 0, 90));
        m_container->setGraphicsEffect(shadow);

        m_mainLayout = new QVBoxLayout(m_container);
        m_mainLayout->setContentsMargins(0, 0, 0, 0);
        m_mainLayout->setSpacing(0);

        auto* titleBar = new QWidget();
        titleBar->setFixedHeight(36);
        titleBar->setStyleSheet("background-color: transparent; border-bottom: 1px solid #2D2D2D;");
        auto* titleLayout = new QHBoxLayout(titleBar);
        titleLayout->setContentsMargins(10, 0, 5, 0);
        titleLayout->setSpacing(8);

        auto* logoLabel = new QLabel();
        logoLabel->setFixedSize(18, 18);
        logoLabel->setAlignment(Qt::AlignCenter);
        logoLabel->setStyleSheet("border: none; background: transparent;");
        
        // 遵循 SSOT 原则：从 SvgIcons.h 引用定制应用图标 app_logo
        logoLabel->setPixmap(iconFromSvg("app_logo", "", QSize(18, 18)).pixmap(18, 18));
        titleLayout->addWidget(logoLabel, 0, Qt::AlignVCenter);

        auto* titleLabel = new QLabel(title);
        titleLabel->setStyleSheet("color: #888; font-size: 12px; font-weight: bold; border: none;");
        titleLayout->addWidget(titleLabel);
        titleLayout->addStretch();

        m_stateBtn = new StateIconBtn();
        connect(m_stateBtn, &QPushButton::clicked, this, &FramelessDialog::onStateBtnClicked);
        titleLayout->addWidget(m_stateBtn);

        auto* closeBtn = new QPushButton("×");
        closeBtn->setFixedSize(24, 24);
        closeBtn->setCursor(Qt::PointingHandCursor);
        closeBtn->setStyleSheet(
            "QPushButton { background-color: #E81123; color: white; border: none; border-radius: 4px; font-size: 16px; } "
            "QPushButton:hover { background-color: #D71520; border-radius: 4px; }"
        );
        // 职责分明：窗口右上角关闭按钮隐藏窗口到托盘
        connect(closeBtn, &QPushButton::clicked, this, &QWidget::hide);
        titleLayout->addWidget(closeBtn);

        m_mainLayout->addWidget(titleBar);

        m_contentArea = new QWidget();
        m_mainLayout->addWidget(m_contentArea, 1);
        
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    }

protected:
    void onStateBtnClicked() { emit stateToggleRequested(); }

#ifdef Q_OS_WIN
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override {
        MSG* msg = static_cast<MSG*>(message);
        if (msg->message == WM_NCHITTEST) {
            int x = GET_X_LPARAM(msg->lParam);
            int y = GET_Y_LPARAM(msg->lParam);
            QPoint pos = mapFromGlobal(QPoint(x, y));
            
            QList<QPushButton*> btns = findChildren<QPushButton*>();
            for (auto* b : btns) {
                if (b->isVisible()) {
                    QPoint btnPos = b->mapTo(this, QPoint(0, 0));
                    QRect btnRect(btnPos, b->size());
                    if (btnRect.contains(pos)) return false; 
                }
            }

            if (pos.y() < 36) {
                *result = HTCAPTION;
                return true;
            }
        }
        return QDialog::nativeEvent(eventType, message, result);
    }
#endif

signals:
    void stateToggleRequested();

protected:
    QWidget* m_container;
    QVBoxLayout* m_outerLayout;
    QVBoxLayout* m_mainLayout;
    QWidget* m_contentArea;
    StateIconBtn* m_stateBtn;
};

// ============================================================================
// TimePasteWindow: 主窗口 (集成系统托盘管理)
// ============================================================================
class TimePasteWindow : public FramelessDialog {
    Q_OBJECT
public:
    explicit TimePasteWindow(QWidget* parent = nullptr) 
        : FramelessDialog("时间输出工具", parent), m_trayIcon(nullptr) 
    {
        setFixedSize(380, 380);

        // SSOT：窗口图标统一来自 SvgIcons.h 的 app_logo
        setWindowIcon(iconFromSvg("app_logo", "", QSize(32, 32)));

        initUI();
        initTrayIcon();

        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &TimePasteWindow::updateDateTime);
        m_timer->start(100);

        connect(&KeyboardHook::instance(), &KeyboardHook::digitPressed, this, &TimePasteWindow::onDigitPressed, Qt::QueuedConnection);
        connect(&KeyboardHook::instance(), &KeyboardHook::pauseToggled, this, &TimePasteWindow::togglePause, Qt::QueuedConnection);
        connect(&KeyboardHook::instance(), &KeyboardHook::exitRequested, qApp, &QApplication::quit, Qt::QueuedConnection);
        connect(this, &FramelessDialog::stateToggleRequested, this, &TimePasteWindow::togglePause);
        
        loadSettings();
    }

    ~TimePasteWindow() override {
        if (m_trayIcon) {
            m_trayIcon->hide();
        }
    }

protected:
    void showEvent(QShowEvent* event) override {
        FramelessDialog::showEvent(event);
        KeyboardHook::instance().setDigitInterceptEnabled(true);
#ifdef Q_OS_WIN
        HWND hwnd = (HWND)winId();
        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_NOACTIVATE | WS_EX_APPWINDOW);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
#endif
    }

    void hideEvent(QHideEvent* event) override {
        KeyboardHook::instance().setDigitInterceptEnabled(false);
        saveSettings();
        FramelessDialog::hideEvent(event);
    }

private slots:
    void onHotkeyChanged(HotkeyConfig cfg) {
        KeyboardHook::instance().setHotkey(cfg);
    }

    void togglePause() {
        bool newState = !KeyboardHook::instance().isPaused();
        KeyboardHook::instance().setPaused(newState);
        m_stateBtn->setPaused(newState);
        m_timeLabel->setStyleSheet(newState ? "color: #666; font-size: 32px; font-weight: bold; font-family: 'Consolas';" : "color: #00BFA5; font-size: 32px; font-weight: bold; font-family: 'Consolas';");
    }

    void updateDateTime() {
        QDateTime now = QDateTime::currentDateTime();
        m_dateLabel->setText(now.toString("yyyy-MM-dd"));
        m_timeLabel->setText(now.toString("HH:mm:ss"));
    }

    void onDigitPressed(int digit) {
        if (!isVisible() || KeyboardHook::instance().isPaused()) return;
        QDateTime target = QDateTime::currentDateTime().addSecs((m_radioPrev->isChecked() ? -digit : digit) * 60);
        QString timeStr = target.toString("HH:mm");
#ifdef Q_OS_WIN
        QTimer::singleShot(10, [timeStr]() {
            INPUT releaseInputs[8]; memset(releaseInputs, 0, sizeof(releaseInputs));
            BYTE keys[] = { VK_LCONTROL, VK_RCONTROL, VK_LSHIFT, VK_RSHIFT, VK_LMENU, VK_RMENU, VK_LWIN, VK_RWIN };
            for (int i = 0; i < 8; ++i) { releaseInputs[i].type = INPUT_KEYBOARD; releaseInputs[i].ki.wVk = keys[i]; releaseInputs[i].ki.dwFlags = KEYEVENTF_KEYUP; }
            SendInput(8, releaseInputs, sizeof(INPUT));
            int len = timeStr.length(); QVector<INPUT> inputs(len * 2);
            for (int i = 0; i < len; ++i) {
                inputs[i*2].type = INPUT_KEYBOARD; inputs[i*2].ki.wScan = timeStr[i].unicode(); inputs[i*2].ki.dwFlags = KEYEVENTF_UNICODE;
                inputs[i*2+1] = inputs[i*2]; inputs[i*2+1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
            }
            SendInput(inputs.size(), inputs.data(), sizeof(INPUT));
        });
#endif
    }

private:
    void initTrayIcon() {
        m_trayIcon = new QSystemTrayIcon(this);
        // SSOT：系统托盘图标统一使用 app_logo
        m_trayIcon->setIcon(iconFromSvg("app_logo", "", QSize(32, 32)));
        m_trayIcon->setToolTip("时间输出工具");

        auto* trayMenu = new QMenu(this);
        trayMenu->setStyleSheet(
            "QMenu {"
            "  background-color: #252526;"
            "  color: #CCCCCC;"
            "  border: 1px solid #3E3E42;"
            "  padding: 4px;"
            "  border-radius: 6px;"
            "}"
            "QMenu::item {"
            "  padding: 6px 22px 6px 12px;"
            "  border-radius: 4px;"
            "}"
            "QMenu::item:selected {"
            "  background-color: #3E3E42;"
            "  color: #FFFFFF;"
            "}"
            "QMenu::separator {"
            "  height: 1px;"
            "  background: #3E3E42;"
            "  margin: 4px 6px;"
            "}"
        );

        // 仅从 SvgIcons.h 引用图标："显示界面"引用 window，"退出程序"引用 power
        auto* showAction = new QAction(iconFromSvg("window", "#00BFA5", QSize(16, 16)), "显示界面", trayMenu);
        connect(showAction, &QAction::triggered, this, [this]() {
            showNormal();
            raise();
            activateWindow();
        });

        auto* quitAction = new QAction(iconFromSvg("power", "#E81123", QSize(16, 16)), "退出程序", trayMenu);
        connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

        trayMenu->addAction(showAction);
        trayMenu->addSeparator();
        trayMenu->addAction(quitAction);

        m_trayIcon->setContextMenu(trayMenu);

        connect(m_trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
                if (isVisible()) {
                    hide();
                } else {
                    showNormal();
                    raise();
                    activateWindow();
                }
            }
        });

        m_trayIcon->show();
    }

    void initUI() {
        auto* contentLayout = new QVBoxLayout(m_contentArea);
        contentLayout->setContentsMargins(25, 15, 25, 10);
        contentLayout->setSpacing(15);

        m_dateLabel = new QLabel(); m_dateLabel->setAlignment(Qt::AlignCenter);
        m_dateLabel->setStyleSheet("color: #888; font-size: 16px;");
        contentLayout->addWidget(m_dateLabel);

        m_timeLabel = new QLabel(); m_timeLabel->setAlignment(Qt::AlignCenter);
        m_timeLabel->setStyleSheet("color: #00BFA5; font-size: 32px; font-weight: bold; font-family: 'Consolas';");
        contentLayout->addWidget(m_timeLabel);

        auto* sep = new QLabel(); sep->setFixedHeight(1); sep->setStyleSheet("background-color: #333;");
        contentLayout->addWidget(sep);

        auto radioStyle = "QRadioButton, QCheckBox { color: #BBB; font-size: 14px; spacing: 10px; } "
                          "QRadioButton::indicator, QCheckBox::indicator { width: 16px; height: 16px; border-radius: 4px; border: 2px solid #444; background: #252525; } "
                          "QRadioButton::indicator:checked, QCheckBox::indicator:checked { background-color: #00BFA5; border: 2px solid #00BFA5; }";

        m_radioPrev = new QRadioButton("退 (往前 N 分钟)"); m_radioPrev->setChecked(true);
        m_radioPrev->setStyleSheet(radioStyle);
        contentLayout->addWidget(m_radioPrev);

        m_radioNext = new QRadioButton("进 (往后 N 分钟)");
        m_radioNext->setStyleSheet(radioStyle);
        contentLayout->addWidget(m_radioNext);

        m_buttonGroup = new QButtonGroup(this);
        m_buttonGroup->addButton(m_radioPrev, 0); m_buttonGroup->addButton(m_radioNext, 1);

        auto* tip = new QLabel("按 0-9 输出时间 | Ctrl+Q 退出程序");
        tip->setAlignment(Qt::AlignCenter);
        tip->setStyleSheet("color: #555; font-size: 11px;");
        contentLayout->addWidget(tip);

        // 底部自定义快捷键区域：上下间距精确为 5px，总高 42px (5 + 32 + 5)
        auto* hotkeyBar = new QWidget();
        hotkeyBar->setFixedHeight(42);
        hotkeyBar->setStyleSheet("background-color: #222; border-top: 1px solid #333; border-bottom-left-radius: 8px; border-bottom-right-radius: 8px;");
        auto* hbLayout = new QHBoxLayout(hotkeyBar);
        hbLayout->setContentsMargins(16, 5, 16, 5); // 左右16px，上下固定5px间距
        hbLayout->setSpacing(0);

        m_hotkeyEdit = new HotkeyEdit();
        connect(m_hotkeyEdit, &HotkeyEdit::hotkeyChanged, this, &TimePasteWindow::onHotkeyChanged);
        hbLayout->addWidget(m_hotkeyEdit);

        m_mainLayout->addWidget(hotkeyBar);
    }

    void loadSettings() {
        QSettings s("RapidTimeTool", "Window");
        if (s.contains("pos")) move(s.value("pos").toPoint());
        m_buttonGroup->button(s.value("mode", 0).toInt())->setChecked(true);
        HotkeyConfig cfg;
        cfg.vk = s.value("hotkeyVK", VK_F1).toInt();
        cfg.ctrl = s.value("hotkeyCtrl", false).toBool();
        cfg.alt = s.value("hotkeyAlt", false).toBool();
        cfg.shift = s.value("hotkeyShift", false).toBool();
        cfg.name = s.value("hotkeyName", "F1").toString();
        KeyboardHook::instance().setHotkey(cfg);
        m_hotkeyEdit->setText(cfg.name);
    }

    void saveSettings() {
        QSettings s("RapidTimeTool", "Window");
        s.setValue("pos", pos());
        s.setValue("mode", m_buttonGroup->checkedId());
        const auto& cfg = KeyboardHook::instance().hotkey();
        s.setValue("hotkeyVK", cfg.vk);
        s.setValue("hotkeyCtrl", cfg.ctrl);
        s.setValue("hotkeyAlt", cfg.alt);
        s.setValue("hotkeyShift", cfg.shift);
        s.setValue("hotkeyName", cfg.name);
    }

    QLabel *m_dateLabel, *m_timeLabel;
    QRadioButton *m_radioPrev, *m_radioNext;
    QButtonGroup* m_buttonGroup;
    QTimer* m_timer;
    HotkeyEdit* m_hotkeyEdit;
    QSystemTrayIcon* m_trayIcon;
};

#pragma pack(push, 1)
typedef struct _T_ATA_ID {
    unsigned short R1[10];
    unsigned short SN[10];
    unsigned short R2[235];
} T_ATA_ID;
#pragma pack(pop)

QString getDiskSerial(const QString& drivePath) {
    QString drive = drivePath.trimmed();
    if (drive.contains(":")) drive = drive.left(2);
    QString vol = "\\\\.\\" + drive;
    HANDLE hV = CreateFileW((LPCWSTR)vol.utf16(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hV == INVALID_HANDLE_VALUE) return "";
    STORAGE_DEVICE_NUMBER sdn; DWORD br = 0;
    if (!DeviceIoControl(hV, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &sdn, sizeof(sdn), &br, NULL)) { CloseHandle(hV); return ""; }
    CloseHandle(hV);
    QString phys = QString("\\\\.\\PhysicalDrive%1").arg(sdn.DeviceNumber);
    HANDLE hD = CreateFileW((LPCWSTR)phys.utf16(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hD == INVALID_HANDLE_VALUE) return "";
    QString res;
    struct { STORAGE_PROPERTY_QUERY Q; STORAGE_PROTOCOL_SPECIFIC_DATA P; BYTE B[4096]; } nReq = {};
    nReq.Q.PropertyId = (STORAGE_PROPERTY_ID)51; nReq.Q.QueryType = PropertyStandardQuery;
    nReq.P.ProtocolType = (STORAGE_PROTOCOL_TYPE)17; nReq.P.DataType = 0; nReq.P.ProtocolDataRequestValue = 1;
    nReq.P.ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA); nReq.P.ProtocolDataLength = 4096;
    if (DeviceIoControl(hD, IOCTL_STORAGE_QUERY_PROPERTY, &nReq, sizeof(nReq), &nReq, sizeof(nReq), &br, NULL)) {
        res = QString::fromLatin1((const char*)nReq.B + 4, 20).trimmed();
    }
    if (res.isEmpty()) {
        SENDCMDINPARAMS sci = {0}; sci.irDriveRegs.bCommandReg = 0xEC;
        struct { SENDCMDOUTPARAMS out; BYTE b[512]; } sOut = {0};
        if (DeviceIoControl(hD, SMART_RCV_DRIVE_DATA, &sci, sizeof(sci), &sOut, sizeof(sOut), &br, NULL)) {
            T_ATA_ID* p = (T_ATA_ID*)sOut.b; QByteArray ba;
            for (int i = 0; i < 10; ++i) { ba.append((char)(p->SN[i] >> 8)); ba.append((char)(p->SN[i] & 0xFF)); }
            res = QString::fromLatin1(ba).trimmed();
        }
    }
    CloseHandle(hD); return res;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 硬件序列号硬绑定校验
    const QStringList targetDisks = {
        "494000PA0D9L", "PHYS825203NX480BGN", "NA5360WJ", "NA7G89GQ", "03000210052122072519"
    };
    const QStringList targetMacs = {
        "8C:81:2B:60:19:ED", "00:15:5D:13:18:72", "A0:48:1C:86:32:DF",
        "5E:1B:66:55:5B:8B", "00:15:5D:99:09:5D", "00:15:5D:E9:6A:69"
    };

    auto getWmi = [](const QString& arg) -> QString {
        QProcess p; p.start("wmic", arg.split(' ', Qt::SkipEmptyParts));
        if (p.waitForFinished(3000)) {
            QString out = QString::fromLocal8Bit(p.readAllStandardOutput());
            QStringList lines = out.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
            if (lines.size() >= 2) return lines[1].trimmed();
        }
        return "";
    };

    bool pass = false;

    // 1. CPU ID 检查
    QString cpuId = getWmi("cpu get processorid");
    if (cpuId.compare("BFEBFBFF000306C3", Qt::CaseInsensitive) == 0) pass = true;

    // 2. 主板序列号检查
    if (!pass) {
        QString boardSn = getWmi("baseboard get serialnumber");
        if (boardSn.compare("SGH412RF00", Qt::CaseInsensitive) == 0) pass = true;
    }

    // 3. BIOS 序列号检查
    if (!pass) {
        QString biosSn = getWmi("bios get serialnumber");
        if (biosSn.compare("SGH412RF00", Qt::CaseInsensitive) == 0) pass = true;
    }

    // 4. 硬盘序列号检查 (C盘、程序盘、以及 wmic 获取的全量磁盘)
    if (!pass) {
        QString cDriveSn = getDiskSerial("C:");
        QString appPath = QCoreApplication::applicationDirPath();
        QString appDriveSn = appPath.contains(":") ? getDiskSerial(appPath.left(2)) : "";
        for (const QString& td : targetDisks) {
            if ((!cDriveSn.isEmpty() && cDriveSn.compare(td, Qt::CaseInsensitive) == 0) ||
                (!appDriveSn.isEmpty() && appDriveSn.compare(td, Qt::CaseInsensitive) == 0)) {
                pass = true;
                break;
            }
        }
    }

    if (!pass) {
        QProcess pDisk;
        pDisk.start("wmic", QStringList() << "diskdrive" << "get" << "serialnumber");
        if (pDisk.waitForFinished(3000)) {
            QString out = QString::fromLocal8Bit(pDisk.readAllStandardOutput());
            for (const QString& td : targetDisks) {
                if (out.contains(td, Qt::CaseInsensitive)) {
                    pass = true;
                    break;
                }
            }
        }
    }

    // 5. MAC 地址检查
    if (!pass) {
        QProcess pMac;
        pMac.start("getmac", QStringList() << "/fo" << "csv" << "/v");
        if (pMac.waitForFinished(5000)) {
            QString macOutput = QString::fromLocal8Bit(pMac.readAllStandardOutput());
            QStringList lines = macOutput.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
            for (const QString& line : lines) {
                QStringList parts = line.split(',');
                for (QString token : parts) {
                    token = token.remove('"').trimmed();
                    QString normalized = token;
                    normalized.remove('-').remove(':');
                    if (normalized.length() == 12 && normalized.contains(QRegularExpression("^[0-9A-Fa-f]{12}$"))) {
                        QString formattedMac = normalized.toUpper();
                        formattedMac.insert(2, ':');
                        formattedMac.insert(5, ':');
                        formattedMac.insert(8, ':');
                        formattedMac.insert(11, ':');
                        formattedMac.insert(14, ':');
                        for (const QString& target : targetMacs) {
                            if (formattedMac.compare(target, Qt::CaseInsensitive) == 0) {
                                pass = true;
                                break;
                            }
                        }
                    }
                    if (pass) break;
                }
                if (pass) break;
            }
        }
    }

    if (!pass) {
        QDialog dlg;
        dlg.setWindowTitle("环境初始化提示");
        dlg.setFixedSize(460, 230);
        dlg.setStyleSheet(
            "QDialog { background-color: #1e1e1e; border: 1px solid #333333; border-radius: 8px; }"
            "QLabel { color: #cccccc; font-size: 13px; line-height: 140%; }"
            "QPushButton { background-color: #3e3e42; color: #ffffff; border: 1px solid #555555; border-radius: 4px; padding: 6px 16px; font-weight: bold; font-size: 13px; }"
            "QPushButton:hover { background-color: #4e4e52; border-color: #777777; }"
            "QPushButton:pressed { background-color: #2d2d30; }"
        );

        QVBoxLayout* layout = new QVBoxLayout(&dlg);
        layout->setContentsMargins(24, 20, 24, 20);
        layout->setSpacing(16);

        QLabel* titleLabel = new QLabel("<span style='font-size: 15px; font-weight: bold; color: #f1c40f;'>当前硬件运行环境不兼容</span>", &dlg);
        layout->addWidget(titleLabel);

        QLabel* bodyLabel = new QLabel(
            "检测到当前计算机的主板总线架构、芯片组指令集或存储子系统未达到本应用的底层初始化要求。<br><br>"
            "本应用依赖专有的硬件底层加速与隔离调度指令，当前设备环境不适宜运行该程序。<br><br>"
            "<span style='color: #888888;'>程序初始化失败，即将安全退出。</span>", &dlg);
        bodyLabel->setWordWrap(true);
        layout->addWidget(bodyLabel);

        QHBoxLayout* btnLayout = new QHBoxLayout();
        btnLayout->addStretch();
        QPushButton* btnExit = new QPushButton("确认并退出", &dlg);
        btnExit->setCursor(Qt::PointingHandCursor);
        QObject::connect(btnExit, &QPushButton::clicked, &dlg, &QDialog::accept);
        btnLayout->addWidget(btnExit);
        layout->addLayout(btnLayout);

        dlg.exec();
        return 0;
    }

    qRegisterMetaType<HotkeyConfig>("HotkeyConfig");
    KeyboardHook::instance().start();

    TimePasteWindow w; 
    w.show();

    int r = a.exec();
    KeyboardHook::instance().stop();
    return r;
}

#include "main.moc"