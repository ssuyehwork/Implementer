#include "AppShortcutController.h"
#include "SearchController.h"
#include "TitleBarWidget.h"
#include "TabBarWidget.h"
#include "LocalShortcutInterceptor.h"
#include "../core/NavigationService.h"
#include "../core/UndoManager.h"
#include <QApplication>
#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QAbstractSpinBox>
#include <QComboBox>
#include <QAbstractItemView>
#include <QKeyEvent>

namespace QuarkMeta {

AppShortcutController::AppShortcutController(QWidget* targetWindow,
                                             SearchController* searchController,
                                             QObject* parent)
    : QObject(parent),
      m_window(targetWindow),
      m_searchController(searchController) {
    if (qApp) {
        qApp->installEventFilter(this);
    }
    initShortcuts();
}

AppShortcutController::~AppShortcutController() {
    if (qApp) {
        qApp->removeEventFilter(this);
    }
}

bool AppShortcutController::isEditingFocus() {
    QWidget* focusW = QApplication::focusWidget();
    if (!focusW) return false;

    // 1. 标准单行/多行/数值输入框
    if (qobject_cast<QLineEdit*>(focusW) ||
        qobject_cast<QTextEdit*>(focusW) ||
        qobject_cast<QPlainTextEdit*>(focusW) ||
        qobject_cast<QAbstractSpinBox*>(focusW)) {
        return true;
    }

    // 2. 可编辑下拉框
    if (auto cb = qobject_cast<QComboBox*>(focusW)) {
        if (cb->isEditable()) return true;
    }

    // 3. 元对象继承检查 (涵盖继承 QLineEdit/QTextEdit 等的自定义控件)
    if (focusW->inherits("QLineEdit") ||
        focusW->inherits("QTextEdit") ||
        focusW->inherits("QPlainTextEdit") ||
        focusW->inherits("QAbstractSpinBox")) {
        return true;
    }

    // 4. ItemView 行内编辑器检查: 焦点控件属于 QAbstractItemView 内部的子控件（且既不是 view 本身也不是 viewport）
    QWidget* p = focusW->parentWidget();
    while (p) {
        if (auto view = qobject_cast<QAbstractItemView*>(p)) {
            if (focusW != view && focusW != view->viewport()) {
                return true;
            }
        }
        p = p->parentWidget();
    }

    return false;
}

bool AppShortcutController::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEv = static_cast<QKeyEvent*>(event);

        // 1. 全局 Ctrl + W 关闭激活窗口契约
        if (keyEv->key() == Qt::Key_W && (keyEv->modifiers() & Qt::ControlModifier)) {
            QWidget* activeWin = QApplication::activeWindow();
            if (activeWin) {
                activeWin->close();
                event->accept();
                return true;
            }
        }

        // 2. Tab 键沉浸模式切换
        if (keyEv->key() == Qt::Key_Tab && keyEv->modifiers() == Qt::NoModifier && m_window) {
            QWidget* watchedW = qobject_cast<QWidget*>(watched);
            if (watchedW && (watchedW == m_window || m_window->isAncestorOf(watchedW))) {
                if (!isEditingFocus()) {
                    emit toggleImmersiveRequested();
                    event->accept();
                    return true;
                }
            }
        }
    }
    return QObject::eventFilter(watched, event);
}

void AppShortcutController::initShortcuts() {
    if (!m_window) return;

    LocalShortcutInterceptor::instance().unregisterAllShortcuts();
    LocalShortcutInterceptor::instance().setEditFocusPredicate([]() {
        return isEditingFocus();
    });

    auto registerShortcut = [this](const QKeySequence& seq, std::function<void()> fn) {
#ifdef Q_OS_WIN
        // Windows 平台由防抢占低级键盘钩子统一接管拦截与分发，杜绝 QShortcut 双重激活风险
        LocalShortcutInterceptor::instance().registerProtectedShortcut(seq, fn);
#else
        // 非 Windows 平台降级使用 Qt 标准 QShortcut
        QShortcut* sc = new QShortcut(seq, m_window);
        sc->setContext(Qt::WindowShortcut);
        connect(sc, &QShortcut::activated, this, fn);
#endif
    };

    // 1. F5: 局内刷新当前目录
    registerShortcut(QKeySequence(Qt::Key_F5), []() {
        NavigationService::instance().refresh();
    });

    // 2. Ctrl+Z: 局内文件撤销
    registerShortcut(QKeySequence::Undo, []() {
        if (!isEditingFocus()) UndoManager::instance().undo();
    });

    // 3. Ctrl+Shift+Z / Ctrl+Y: 局内文件重做
    registerShortcut(QKeySequence::Redo, []() {
        if (!isEditingFocus()) UndoManager::instance().redo();
    });

    // 4. Alt+Q: 局内切换窗口置顶
    registerShortcut(QKeySequence(Qt::ALT | Qt::Key_Q), [this]() {
        emit togglePinRequested();
    });

    // 5. Ctrl+F: 局内聚焦搜索栏
    registerShortcut(QKeySequence::Find, [this]() {
        if (m_searchController) {
            m_searchController->focusSearch();
        }
    });

    // 6. Ctrl+W: 优先关闭当前活动标签页
    registerShortcut(QKeySequence(Qt::CTRL | Qt::Key_W), [this]() {
        if (m_window) {
            if (auto titleBar = m_window->findChild<TitleBarWidget*>()) {
                if (titleBar->tabBar()) {
                    if (titleBar->tabBar()->tabCount() > 1) {
                        titleBar->tabBar()->closeTab(titleBar->tabBar()->currentIndex());
                        return;
                    }
                }
            }
            m_window->close();
        }
    });

    // 6b. Ctrl+Tab: 标签页顺序切换
    registerShortcut(QKeySequence(Qt::CTRL | Qt::Key_Tab), [this]() {
        if (m_window) {
            if (auto titleBar = m_window->findChild<TitleBarWidget*>()) {
                if (titleBar->tabBar()) {
                    titleBar->tabBar()->selectNextTab();
                }
            }
        }
    });

    // 6c. Ctrl+Shift+Tab: 标签页逆序切换
    registerShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Backtab), [this]() {
        if (m_window) {
            if (auto titleBar = m_window->findChild<TitleBarWidget*>()) {
                if (titleBar->tabBar()) {
                    titleBar->tabBar()->selectPreviousTab();
                }
            }
        }
    });

    // 7. Ctrl+T: 新建标签页
    registerShortcut(QKeySequence(Qt::CTRL | Qt::Key_T), [this]() {
        if (m_window) {
            if (auto titleBar = m_window->findChild<TitleBarWidget*>()) {
                if (titleBar->tabBar()) {
                    titleBar->tabBar()->addTab("此电脑", "computer://", true);
                }
            }
        }
    });

    // 8. Ctrl+Shift+T: 恢复关闭的标签页
    registerShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_T), [this]() {
        if (m_window) {
            if (auto titleBar = m_window->findChild<TitleBarWidget*>()) {
                if (titleBar->tabBar()) {
                    titleBar->tabBar()->restoreLastClosedTab();
                }
            }
        }
    });

    // 9. Ctrl+Shift+N: 新建文件夹
    registerShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_N), [this]() {
        if (!isEditingFocus()) {
            emit createNewFolderRequested();
        }
    });

    // 9.1 Ctrl+N: 新建 Markdown 文件
    registerShortcut(QKeySequence(Qt::CTRL | Qt::Key_N), [this]() {
        if (!isEditingFocus()) {
            emit createNewMdRequested();
        }
    });

    // 10. Ctrl+Shift+E: 提取文件文本内容至剪贴板
    registerShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_E), [this]() {
        if (!isEditingFocus()) {
            emit extractContentRequested();
        }
    });

    // 11. Ctrl+R: 切换递归显示子文件夹中的项目
    registerShortcut(QKeySequence(Qt::CTRL | Qt::Key_R), [this]() {
        if (!isEditingFocus()) {
            emit toggleRecursiveRequested();
        }
    });

    // 12. Ctrl+D: 局内重置所有筛选条件
    registerShortcut(QKeySequence(Qt::CTRL | Qt::Key_D), [this]() {
        if (!isEditingFocus()) {
            emit resetFilterRequested();
        }
    });

    // Ctrl+Space: 移入回收站
    registerShortcut(QKeySequence(Qt::CTRL | Qt::Key_Space), [this]() {
        if (!isEditingFocus()) {
            emit moveToTrashRequested();
        }
    });

    // 13. Alt+B: 标记/取消标记当前选中的文件夹
    registerShortcut(QKeySequence(Qt::ALT | Qt::Key_B), [this]() {
        if (!isEditingFocus()) {
            emit toggleMarkedRequested();
        }
    });

    // 14. Alt+V: 迁移选中项目到已标记的目的地文件夹
    registerShortcut(QKeySequence(Qt::ALT | Qt::Key_V), [this]() {
        if (!isEditingFocus()) {
            emit transferToMarkedRequested();
        }
    });

    // 15. Alt+C: 复制所选项目名称（顿号分割）
    registerShortcut(QKeySequence(Qt::ALT | Qt::Key_C), [this]() {
        if (!isEditingFocus()) {
            emit copyNamesEnumeratedRequested();
        }
    });
}

} // namespace QuarkMeta
