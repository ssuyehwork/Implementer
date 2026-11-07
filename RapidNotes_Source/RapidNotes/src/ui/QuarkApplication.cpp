#include "QuarkApplication.h"
#include "UiHelper.h"
#include "LocalShortcutInterceptor.h"
#include "Logger.h"
#include <QEvent>
#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QContextMenuEvent>
#include <QWindow>
#include <QGuiApplication>
#include <QCursor>
#include <QTreeView>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace QuarkMeta {

QuarkApplication::QuarkApplication(int& argc, char** argv)
    : QApplication(argc, argv) {}

void QuarkApplication::performGlobalSelfHealing(QWidget* widget) {
    while (QGuiApplication::overrideCursor()) {
        QGuiApplication::restoreOverrideCursor();
    }

    if (QWidget* grabber = QWidget::mouseGrabber()) {
        if (!widget || grabber == widget || widget->isAncestorOf(grabber)) {
            grabber->releaseMouse();
        }
    }

#ifdef Q_OS_WIN
    if (widget && widget->testAttribute(Qt::WA_WState_Created)) {
        HWND hwnd = reinterpret_cast<HWND>(widget->winId());
        if (GetCapture() == hwnd) {
            ::ReleaseCapture();
        }
    }
#endif

    if (widget) {
        widget->unsetCursor();
    }

    QPoint currentPos = QCursor::pos();
    QCursor::setPos(currentPos);
}

bool QuarkApplication::notify(QObject* receiver, QEvent* event) {
    if (event) {
        if (event->type() == QEvent::ApplicationActivate) {
            LocalShortcutInterceptor::instance().updateFocusState(true);
        } else if (event->type() == QEvent::ApplicationDeactivate) {
            LocalShortcutInterceptor::instance().updateFocusState(false);
        }
    }

    if (event && receiver) {
        QEvent::Type type = event->type();

        // 1. 全局 ContextMenu 拦截并应用 QuarkMeta 专属暗色右键菜单
        if (type == QEvent::ContextMenu && receiver->isWidgetType()) {
            if (QLineEdit* edit = qobject_cast<QLineEdit*>(receiver)) {
                if (edit->contextMenuPolicy() == Qt::DefaultContextMenu) {
                    QContextMenuEvent* cme = static_cast<QContextMenuEvent*>(event);
                    UiHelper::showLineEditContextMenu(edit, cme->pos());
                    event->accept();
                    return true;
                }
            } else if (qobject_cast<QTextEdit*>(receiver) || qobject_cast<QPlainTextEdit*>(receiver)) {
                QWidget* textWidget = static_cast<QWidget*>(receiver);
                if (textWidget->contextMenuPolicy() == Qt::DefaultContextMenu) {
                    QContextMenuEvent* cme = static_cast<QContextMenuEvent*>(event);
                    UiHelper::showTextEditContextMenu(textWidget, cme->pos());
                    event->accept();
                    return true;
                }
            }
        }

        // 2. 全局 F2 与 Ctrl+W 按键拦截
        if (type == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_F2 && keyEvent->modifiers() == Qt::NoModifier) {
                QWidget* focusW = QApplication::focusWidget();

                // 如果焦点在树控件本身，或者因为焦点被偷走但鼠标悬停/选中的是 LibraryTreeView 或 FavoriteTreeView
                QWidget* candidateTree = focusW;
                if (!candidateTree || candidateTree->objectName() == "NavTreeView") {
                    // 尝试在主窗口下寻找当前可见且有选中项的 FavoriteTreeView 或 LibraryTreeView
                    QWidget* activeWin = QApplication::activeWindow();
                    if (activeWin) {
                        auto* libTree = activeWin->findChild<QTreeView*>("LibraryTreeView");
                        auto* favTree = activeWin->findChild<QTreeView*>("FavoriteTreeView");
                        if (libTree && libTree->isVisible() && libTree->currentIndex().isValid() && libTree->underMouse()) {
                            candidateTree = libTree;
                        } else if (favTree && favTree->isVisible() && favTree->currentIndex().isValid() && favTree->underMouse()) {
                            candidateTree = favTree;
                        }
                    }
                }

                if (candidateTree) {
                    if (candidateTree->objectName() == "LibraryTreeView") {
                        QTreeView* tv = qobject_cast<QTreeView*>(candidateTree);
                        if (tv && tv->currentIndex().isValid()) {
                            int nodeId = tv->currentIndex().data(Qt::UserRole + 1).toInt();
                            if (nodeId > 0) { // 仅自定义分类与智能文件夹
                                tv->setFocus();
                                tv->edit(tv->currentIndex());
                                event->accept();
                                return true;
                            }
                        }
                    } else if (candidateTree->objectName() == "FavoriteTreeView") {
                        QTreeView* tv = qobject_cast<QTreeView*>(candidateTree);
                        if (tv && tv->currentIndex().isValid()) {
                            tv->setFocus();
                            tv->edit(tv->currentIndex());
                            event->accept();
                            return true;
                        }
                    }
                }
            }
            if (keyEvent->key() == Qt::Key_W && (keyEvent->modifiers() & Qt::ControlModifier)) {
                QWidget* activeWin = QGuiApplication::focusWindow() ? QWidget::find(QGuiApplication::focusWindow()->winId()) : nullptr;
                if (!activeWin && receiver->isWidgetType()) {
                    activeWin = static_cast<QWidget*>(receiver)->window();
                }
                if (activeWin) {
                    activeWin->close();
                    event->accept();
                    return true;
                }
            }
        }

        // 3. 窗口关闭/隐藏事件驱动全局光标与抓取自愈
        if (type == QEvent::Close || type == QEvent::Hide) {
            QWidget* w = qobject_cast<QWidget*>(receiver);
            if (w && (w->isWindow() || w->inherits("QDialog"))) {
                performGlobalSelfHealing(w);
            }
        }
    }
    return QApplication::notify(receiver, event);
}

} // namespace QuarkMeta
