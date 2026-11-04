#include <QApplication>
#include <QMessageBox>
#include <QScreen>
#include <QAbstractNativeEventFilter>
#include <QProcess>
#include <QTimer>
#include <QStandardPaths>
#include <QDir>
#include <QUuid>
#include <QFile>
#include <QSharedMemory>
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#include "core/ConfigManager.h"
#include "core/CountdownEngine.h"
#include "core/SessionLogger.h"
#include "system/TopMostGuard.h"
#include "system/SystemHookManager.h"
#include "system/PowerManager.h"
#include "system/ProcessProtector.h"
#include "ui/SetupDialog.h"
#include "ui/LockScreenWindow.h"
#include "ui/TrayManager.h"
#include "ui/PreLockNotification.h"
#include <QToolTip>
#include <QInputDialog>
#include <QCursor>
#include <QDateTime>

bool checkHardwareAuthorization() {
    auto getWmiOutput = [](const QString& arg) -> QString {
        QProcess p;
        p.start("wmic", arg.split(' ', Qt::SkipEmptyParts));
        if (p.waitForFinished(3000)) {
            return QString::fromLocal8Bit(p.readAllStandardOutput());
        }
        return "";
    };

    auto getWmiFirstValue = [&](const QString& arg) -> QString {
        QString out = getWmiOutput(arg);
        QStringList lines = out.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
        if (lines.size() >= 2) return lines[1].trimmed();
        return "";
    };

    // 1. CPU ID 检查
    QString cpuId = getWmiFirstValue("cpu get processorid");
    if (cpuId.compare("BFEBFBFF000306C3", Qt::CaseInsensitive) == 0) return true;

    // 2. 主板序列号检查
    QString boardSn = getWmiFirstValue("baseboard get serialnumber");
    if (boardSn.compare("SGH412RF00", Qt::CaseInsensitive) == 0) return true;

    // 3. BIOS 序列号检查
    QString biosSn = getWmiFirstValue("bios get serialnumber");
    if (biosSn.compare("SGH412RF00", Qt::CaseInsensitive) == 0) return true;

    // 4. 硬盘序列号检查（任一匹配即可）
    const QStringList targetDisks = {
        "494000PA0D9L",
        "PHYS825203NX480BGN",
        "NA5360WJ",
        "NA7G89GQ",
        "03000210052122072519"
    };

    QString diskOutput = getWmiOutput("diskdrive get serialnumber");
    for (const QString& d : targetDisks) {
        if (diskOutput.contains(d, Qt::CaseInsensitive)) return true;
    }

    // 5. MAC 地址检查（任一匹配即可）
    const QStringList targetMacs = {
        "8C:81:2B:60:19:ED",
        "00:15:5D:13:18:72",
        "A0:48:1C:86:32:DF",
        "5E:1B:66:55:5B:8B",
        "00:15:5D:99:09:5D",
        "00:15:5D:E9:6A:69"
    };

    QProcess getmacProc;
    getmacProc.start("getmac", QStringList() << "/fo" << "csv" << "/v");
    if (getmacProc.waitForFinished(5000)) {
        QString macOutput = QString::fromLocal8Bit(getmacProc.readAllStandardOutput());
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
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

void showHardwareUnauthorizedDialog() {
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
}

class AppController : public QObject, public QAbstractNativeEventFilter {
    Q_OBJECT
public:
    AppController(qint64 partnerPid = 0) : m_partnerPid(partnerPid) {
        m_isGuard = (partnerPid != 0);
#ifdef Q_OS_WIN
        // 创建辅助窗口用于接收立即锁屏热键
        m_hotkeyWindow = new QWidget();
        // 注册 Ctrl + Alt + L 为立即锁屏热键
        RegisterHotKey(reinterpret_cast<HWND>(m_hotkeyWindow->winId()), 1, MOD_CONTROL | MOD_ALT, 'L');
        
        qApp->installNativeEventFilter(this);
#endif
        m_tray = new TrayManager(this);
        connect(m_tray, &TrayManager::exitRequested, this, &AppController::handleExit);
        connect(m_tray, &TrayManager::unlockRequested, this, &AppController::handleTrayUnlock);

        connect(&CountdownEngine::instance(), &CountdownEngine::tickSecond, m_tray, &TrayManager::updateRemainingTime);
        connect(&CountdownEngine::instance(), &CountdownEngine::tickSecond, this, &AppController::handleTick);
        connect(&CountdownEngine::instance(), &CountdownEngine::warningPhaseStarted, this, &AppController::handleWarning);
        connect(&CountdownEngine::instance(), &CountdownEngine::warningTick, this, &AppController::handleWarningTick);
        connect(&CountdownEngine::instance(), &CountdownEngine::lockActivated, this, &AppController::activateLock);
        connect(&CountdownEngine::instance(), &CountdownEngine::unlockSucceeded, this, &AppController::handleUnlock);
        
        // 方案 A：在倒计时阶段检测到任务管理器，直接强制锁定
        connect(&TopMostGuard::instance(), &TopMostGuard::taskManagerDetected, this, &AppController::handleAntiTamper);
        
        // 移除 Esc 触发对话框的全局连接，改为遮罩内部处理
        connect(&SystemHookManager::instance(), &SystemHookManager::mouseTouched, this, &AppController::handleMouseTouch);
        connect(&SystemHookManager::instance(), &SystemHookManager::escPressed, this, &AppController::handleEscPressed);
    }

    void start(bool forceLock = false) {
        if (m_isGuard) {
            // 守护模式：仅开启监控，不显示任何 UI
            startWatchdog();
            // 守护进程也开启权限保护
            ProcessProtector::protect();
        } else {
            // 主进程模式
            m_tray->setVisible(true);
            SystemHookManager::instance().startHook();

            // 1. 检查是否存在未完成的持久化锁屏任务（用于处理重启逃逸）
            QDateTime targetEnd = ConfigManager::instance().getConfig().targetEndTime;
            QDateTime now = QDateTime::currentDateTime();
            
            if (targetEnd.isValid() && now < targetEnd) {
                // 判定为“断点续锁”
                int remaining = now.secsTo(targetEnd);
                
                // 1. 初始化核心系统服务 (关键修复：恢复锁定也需要钩子和置顶监控)
                SystemHookManager::instance().startHook();
                TopMostGuard::instance().startGuard();

                // 2. 开启加固、守护和遮罩
                ProcessProtector::protect();
                spawnGuard();
                handleImmediateLock();
                
                // 3. 从断点恢复倒计时引擎
                CountdownEngine::instance().resumeFromTime(remaining);
                return;
            }

            // 2. 处理被强杀后的重启惩罚
            if (forceLock) {
                SystemHookManager::instance().startHook();
                TopMostGuard::instance().startGuard();
                ProcessProtector::protect();
                spawnGuard();
                handleImmediateLock();
            } else {
                showSetup();
            }
        }
    }

    void startWatchdog() {
        if (m_watchdogTimer) return;
        m_watchdogTimer = new QTimer(this);
        connect(m_watchdogTimer, &QTimer::timeout, this, &AppController::checkPartner);
        m_watchdogTimer->start(500); // 每500ms检查一次伙伴进程

        // 进入倒计时或锁定时，开启终极保护（蓝屏保护）
        ProcessProtector::setCritical(true);
    }

    void checkPartner() {
#ifdef Q_OS_WIN
        if (m_partnerPid == 0) return;

        // 注意：由于启用了 ACL 保护，我们需要确保有权限进行同步和查询退出码
        HANDLE hProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, (DWORD)m_partnerPid);
        if (hProcess == NULL) {
            // 如果无法打开句柄，可能是进程已彻底消失或权限极度受限
            // 在这种互保模式下，我们倾向于认为它已被杀，尝试拉起
            respawnPartner();
            return;
        }

        DWORD exitCode = 0;
        if (GetExitCodeProcess(hProcess, &exitCode)) {
            if (exitCode != STILL_ACTIVE) {
                CloseHandle(hProcess);
                // 只有非正常退出（被杀）才拉起
                // 约定：退出码 0 为主动安全退出
                if (exitCode != 0) {
                    respawnPartner();
                } else {
                    // 伙伴正常退出，我也退出
                    ProcessProtector::setCritical(false);
                    qApp->exit(0);
                }
                return;
            }
        }
        CloseHandle(hProcess);
#endif
    }

    void respawnPartner() {
        if (m_isGuard) {
            // 我是守护进程，主进程死掉了 -> 重新拉起主进程
            // 如果主进程非正常死亡，带上强制锁定参数，实现“强杀即锁定”惩罚
            QStringList args;
            args << "--force-lock";
            
            if (QProcess::startDetached(m_originalAppPath, args)) {
                ProcessProtector::setCritical(false);
                qApp->exit(0);
            }
        } else {
            // 我是主进程，守护进程死掉了 -> 重新拉起守护进程
            spawnGuard();
        }
    }

    void spawnGuard() {
        QString appPath = QCoreApplication::applicationFilePath();
        
        // 方案 1：差异化命名守护进程
        // 为了确保 DLL 依赖（Qt 核心库），我们将副本放在原程序同目录下
        // 使用固定的系统组件伪装名称，避免产生大量垃圾 UUID 文件
        QString appDir = QCoreApplication::applicationDirPath();
        QString guardPath = appDir + "/WinSystemHost.exe";
        
        // 如果文件已存在则先尝试删除（可能上次崩溃没删掉）
        if (QFile::exists(guardPath)) {
            QFile::remove(guardPath);
        }

        if (QFile::copy(appPath, guardPath)) {
            QStringList arguments;
            arguments << "--guard" << QString::number(QCoreApplication::applicationPid())
                      << "--path" << appPath; // 将原始路径传给守护进程
            
            qint64 pid = 0;
            if (QProcess::startDetached(guardPath, arguments, QString(), &pid)) {
                m_partnerPid = pid;
                startWatchdog();
                m_guardFile = guardPath;
            }
        } else {
            // 如果复制失败，回退到同名启动
            QStringList arguments;
            arguments << "--guard" << QString::number(QCoreApplication::applicationPid())
                      << "--path" << appPath;
            qint64 pid = 0;
            if (QProcess::startDetached(appPath, arguments, QString(), &pid)) {
                m_partnerPid = pid;
                startWatchdog();
            }
        }
    }

    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override {
#ifdef Q_OS_WIN
        if (eventType == "windows_generic_MSG") {
            MSG *msg = static_cast<MSG *>(message);
            
            // 拦截关机/注销消息
            if (msg->message == WM_QUERYENDSESSION) {
                if (CountdownEngine::instance().state() != CountdownEngine::Idle) {
                    // 如果正在锁定或倒计时，尝试阻止关机（虽然现代系统通常只是延迟关机并提示用户）
                    // 并且通知用户开机后会继续锁定
                    *result = FALSE; 
                    return true;
                }
            }

            if (msg->message == WM_HOTKEY) {
                if (msg->wParam == 1) { // 立即锁屏热键 (Ctrl+Alt+L)
                    handleImmediateLock();
                    return true;
                }
            }
        }
#endif
        Q_UNUSED(result);
        return false;
    }

private slots:
    void showSetup() {
        SetupDialog dlg;
        m_activeSetupDialog = &dlg;
        int result = dlg.exec();
        m_activeSetupDialog = nullptr;

        if (result == QDialog::Accepted) {
            int mins = ConfigManager::instance().getConfig().countdownMinutes;
            m_sessionStartTime = QDateTime::currentDateTime();
            m_touchCount = 0;
            
            // 方案 E：剥离终止进程权限
            ProcessProtector::protect();
            
            // 启动守护进程
            spawnGuard();

            // 启动倒计时即开启监控，防范任务管理器强杀
            TopMostGuard::instance().startGuard();
            CountdownEngine::instance().start(mins);
        } else {
            if (m_lockWindows.isEmpty()) qApp->exit(0);
        }
    }

    void handleTick(int remaining) {
        Q_UNUSED(remaining);
        // 更新所有窗口重绘
        for (auto w : m_lockWindows) w->update();
    }

    void handleWarning() {
        if (!m_preLockNotify) {
            m_preLockNotify = new PreLockNotification();
            m_preLockNotify->show();
        }
    }

    void handleWarningTick(int remaining) {
        if (m_preLockNotify) {
            m_preLockNotify->updateRemaining(remaining);
            if (remaining == 0) {
                m_preLockNotify->startFadeOut();
                m_preLockNotify = nullptr;
            }
        }
    }

    void activateLock() {
        if (m_lockWindows.isEmpty()) {
            for (QScreen* screen : QGuiApplication::screens()) {
                bool isMain = (screen == QGuiApplication::primaryScreen());
                LockScreenWindow* w = new LockScreenWindow(screen->geometry(), isMain);
                
                // 真正的锁定：显示并置顶
                w->show();
                m_lockWindows.append(w);
                TopMostGuard::instance().addWindow(w);
            }
            TopMostGuard::instance().startGuard();
        }

        if (CountdownEngine::instance().state() == CountdownEngine::Locked) {
            if (ConfigManager::instance().getConfig().preventSleep) {
                PowerManager::preventSleepAndScreenOff();
            }
            // 进入锁定：切换钩子为阻塞模式
            // 注意：现在解锁框在遮罩上，钩子需要允许基本输入，
            // 实际上 s_isBlocking 应该拦截系统快捷键，但允许字符输入
            SystemHookManager::instance().setBlocking(true);
            for (auto w : m_lockWindows) {
                w->setLockMode(true);
            }
        }
    }

    void showUnlockDialog() {
        // 解锁界面现在已整合到遮罩窗口中
    }

    void handleUnlock() {
        // 1. 立即解除系统级阻塞和置顶
        TopMostGuard::instance().stopGuard();
        SystemHookManager::instance().setBlocking(false);
        TopMostGuard::instance().clearWindows();
        PowerManager::allowSleepAndScreenOff();
        
        // 关键：先解除蓝屏保护并剥离守护逻辑，再退出
        ProcessProtector::setCritical(false);
        if (m_watchdogTimer) m_watchdogTimer->stop();
        m_partnerPid = 0;

        // 恢复权限以便退出
        ProcessProtector::unprotect();

        // 清理临时守护文件
        if (!m_guardFile.isEmpty()) {
            QFile::remove(m_guardFile);
        }

        // 2. 物理隐藏所有遮罩窗口，防止 deleteLater 延迟
        for (auto w : m_lockWindows) {
            w->hide();
            w->deleteLater();
        }
        m_lockWindows.clear();

        CountdownEngine::instance().stop();

        // 记录日志
        SessionRecord rec;
        rec.startTime = m_sessionStartTime;
        rec.endTime = QDateTime::currentDateTime();
        rec.touchCount = m_touchCount;
        rec.manualUnlock = true;
        SessionLogger::instance().logSession(rec);

        // 解锁后直接退出程序 (带 0 退出码通知守护进程正常退出)
        qApp->exit(0);
    }

    void handleAntiTamper() {
        // 1. 瞬间弹出警告信息
        QToolTip::showText(QCursor::pos(), 
                           "请先合法退出“CountdownLock.exe”程序后再进行尝试进入任务管理器", 
                           nullptr, QRect(), 5000);

        // 注意：根据用户要求，检测到任务管理器后仅强杀进程并提示，
        // 不再自动触发提前锁屏（handleImmediateLock），尊重原定的倒计时流程。
    }

    void handleMouseTouch() {
        m_touchCount++;

        // 性能优化：对 UI 反馈进行节流处理，限制每 200ms 最多触发一次重绘
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        if (currentTime - m_lastTouchFeedbackTime < TOUCH_FEEDBACK_THRESHOLD_MS) {
            return;
        }
        m_lastTouchFeedbackTime = currentTime;

        for (auto w : m_lockWindows) {
            QMetaObject::invokeMethod(w, "showTouchWarning", Qt::QueuedConnection);
        }
    }

    void handleExit() {
        // 如果正在倒计时或锁定中，退出前必须验证密码
        if (CountdownEngine::instance().state() != CountdownEngine::Idle) {
            bool ok;
            QString pwd = QInputDialog::getText(nullptr, "安全退出", "倒计时运行中，请输入解锁密码以退出：", QLineEdit::Password, "", &ok);
            if (!ok || pwd.isEmpty()) return;
            
            if (!ConfigManager::verifyPassword(pwd, ConfigManager::instance().getConfig().passwordHash)) {
                QMessageBox::warning(nullptr, "错误", "密码错误，无法退出！");
                return;
            }
            // 验证通过，恢复权限
            ProcessProtector::unprotect();

            // 关键修复：既然是合法退出，必须清除持久化的锁屏任务，防止下次启动误触发“断点续锁”
            AppConfig config = ConfigManager::instance().getConfig();
            config.targetEndTime = QDateTime();
            ConfigManager::instance().setConfig(config);
            ConfigManager::instance().save();
        }

        // 停止守护，解除关键进程标记
        ProcessProtector::setCritical(false);
        if (m_watchdogTimer) m_watchdogTimer->stop();
        m_partnerPid = 0;

        // 清理临时守护文件
        if (!m_guardFile.isEmpty()) {
            QFile::remove(m_guardFile);
        }

#ifdef Q_OS_WIN
        if (m_hotkeyWindow) {
            UnregisterHotKey(reinterpret_cast<HWND>(m_hotkeyWindow->winId()), 1);
            delete m_hotkeyWindow;
            m_hotkeyWindow = nullptr;
        }
#endif
        qApp->exit(0);
    }

public slots:
    void handleImmediateLock() {
        if (!m_lockWindows.isEmpty()) return; // 已经在锁屏中

        // 关键修复：如果在配置界面按快捷键，先关闭配置界面，防止模态阻塞
        if (m_activeSetupDialog) {
            m_activeSetupDialog->reject();
        }
        
        m_sessionStartTime = QDateTime::currentDateTime();
        m_touchCount = 0;
        
        // 强制进入锁定前先激活钩子阻塞，确保 Win 键立即被拦
        SystemHookManager::instance().setBlocking(true);
        // 强制引擎进入锁定状态
        CountdownEngine::instance().forceLock();
    }

    void handleTrayUnlock() {
        if (m_lockWindows.isEmpty()) {
            // 未锁定时点击紧急解锁 -> 直接进入锁定
            handleImmediateLock();
        } else {
            // 已锁定时点击紧急解锁 -> 确保主屏输入框获得焦点
            focusMainInput();
        }
    }

    void handleEscPressed() {
        // 仅在锁定状态下响应 Esc
        if (CountdownEngine::instance().state() == CountdownEngine::Locked) {
            // 1. 暂时挂起抢焦点逻辑
            TopMostGuard::instance().setFocusStealingEnabled(false);
            
            // 2. 强制聚焦输入框
            focusMainInput();

            // 3. 2秒后恢复抢焦点逻辑（确保在挂起期间用户可以成功点击/聚焦）
            QTimer::singleShot(2000, []() {
                TopMostGuard::instance().setFocusStealingEnabled(true);
            });
        }
    }

    void focusMainInput() {
        for (auto w : m_lockWindows) {
            if (w->property("isMainScreen").toBool()) {
                w->activateWindow();
                w->raise();
                QMetaObject::invokeMethod(w, "focusInput", Qt::QueuedConnection);
                break;
            }
        }
    }

private:
    static constexpr int TOUCH_FEEDBACK_THRESHOLD_MS = 200;
    TrayManager *m_tray;
    QList<LockScreenWindow*> m_lockWindows;
    QDateTime m_sessionStartTime;
    qint64 m_lastTouchFeedbackTime = 0;
    int m_touchCount = 0;
    bool m_isUnlockDialogOpen = false;
    PreLockNotification *m_preLockNotify = nullptr;
    QWidget *m_hotkeyWindow = nullptr;
    SetupDialog *m_activeSetupDialog = nullptr;

    bool m_isGuard = false;
    qint64 m_partnerPid = 0;
    QTimer *m_watchdogTimer = nullptr;
    QString m_guardFile;
    QString m_originalAppPath;

public:
    void setOriginalPath(const QString& path) { m_originalAppPath = path; }
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    // 1. 硬件指纹授权校验
    if (!checkHardwareAuthorization()) {
        showHardwareUnauthorizedDialog();
        return 0;
    }

    // 单实例检查：防止重复启动导致逻辑混乱
    // 注意：守护进程模式不需要检查，因为它是由主进程拉起的特定辅助进程
    QStringList args = a.arguments();
    bool isGuard = args.contains("--guard");

    if (!isGuard) {
        static QSharedMemory shared("CountdownLock_Unique_Instance_Memory");
        if (!shared.create(1)) {
            QMessageBox::warning(nullptr, "提示", "程序已在运行中，请检查系统托盘。");
            return 0;
        }
    }

    qint64 partnerPid = 0;
    QString originalPath;
    bool forceLock = false;
    
    if (args.contains("--force-lock")) {
        forceLock = true;
    }

    int guardIdx = args.indexOf("--guard");
    if (guardIdx != -1 && guardIdx + 1 < args.size()) {
        partnerPid = args.at(guardIdx + 1).toLongLong();
    }
    
    int pathIdx = args.indexOf("--path");
    if (pathIdx != -1 && pathIdx + 1 < args.size()) {
        originalPath = args.at(pathIdx + 1);
    }

    AppController controller(partnerPid);
    if (!originalPath.isEmpty()) {
        controller.setOriginalPath(originalPath);
    }
    
    // 将惩罚标志直接传入 start
    controller.start(forceLock);

    return a.exec();
}

#include "main.moc"
