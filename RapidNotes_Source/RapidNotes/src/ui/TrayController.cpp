#include "TrayController.h"
#include <QApplication>
#include <QIcon>
#include <QDebug>
#include <QCursor>
#include "../meta/DatabaseManager.h"
#include "UiHelper.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace QuarkMeta {

TrayController::TrayController(QMainWindow* mainWindow)
    : QObject(mainWindow), m_mainWindow(mainWindow) {
    m_trayIcon = new QSystemTrayIcon(this);

    // 2026-04-14 物理加固：锁定图标来源为 Qt 资源系统中的标准 ico
    m_trayIcon->setIcon(QIcon(":/app_icon.ico"));
    m_trayIcon->setToolTip("QuarkMeta");

    m_trayMenu = new QMenu(mainWindow);
    UiHelper::applyMenuStyle(m_trayMenu);

    QAction* showAction = m_trayMenu->addAction(UiHelper::getIcon("window", QColor("#EEEEEE"), 18), "显示主界面");
    m_trayMenu->addSeparator();
    QAction* quitAction = m_trayMenu->addAction(UiHelper::getIcon("power", QColor("#EEEEEE"), 18), "退出 QuarkMeta");

    connect(showAction, &QAction::triggered, this, &TrayController::onShowMainWindow);
    connect(quitAction, &QAction::triggered, this, &TrayController::onQuitApp);

    m_trayIcon->setContextMenu(m_trayMenu);

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &TrayController::onTrayActivated);
}

TrayController::~TrayController() {
    if (m_trayIcon) {
        m_trayIcon->hide();
    }
    if (m_trayMenu) {
        delete m_trayMenu;
        m_trayMenu = nullptr;
    }
}

void TrayController::show() {
    m_trayIcon->show();
}

void TrayController::hide() {
    m_trayIcon->hide();
}

void TrayController::onTrayActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        if (m_mainWindow && m_mainWindow->isVisible() && !m_mainWindow->isMinimized()) {
            m_mainWindow->hide();
        } else {
            onShowMainWindow();
        }
    }
}

void TrayController::onShowMainWindow() {
    if (!m_mainWindow) return;
    if (m_mainWindow->isMinimized()) {
        m_mainWindow->showNormal();
    }
    m_mainWindow->show();
    m_mainWindow->raise();
    m_mainWindow->activateWindow();
}

void TrayController::onQuitApp() {
    if (m_trayIcon) {
        m_trayIcon->hide();
    }
    if (m_mainWindow) {
        m_mainWindow->close();
    }
    // 强制通知 Qt 主事件循环以状态码 0 顺畅退出，触发现发 aboutToQuit 数据库与线程池清场机制
    QApplication::exit(0);
}

} // namespace QuarkMeta
