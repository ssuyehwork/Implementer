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

namespace RapidNotes {

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
        LocalShortcutInterceptor::instance().registerProtectedShortcut(seq, fn);
#else
        QShortcut* sc = new QShortcut(seq, m_window);
        sc->setContext(Qt::WindowShortcut);
        connect(sc, &QShortcut::activated, this, fn);
#endif
    };

    // 1. F5: 全局刷新
    registerShortcut(QKeySequence(Qt::Key_F5), []() {
        NavigationService::instance().refresh();
    });

    // 2. Ctrl+Z: 事务级撤销
    registerShortcut(QKeySequence::Undo, []() {
        if (!isEditingFocus()) UndoManager::instance().undo();
    });

    // 3. Ctrl+Shift+Z / Ctrl+Y: 事务级重做
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

    // 7. Ctrl+T: 新建标签页（默认指向 library://-1 全部数据）
    registerShortcut(QKeySequence(Qt::CTRL | Qt::Key_T), [this]() {
        if (m_window) {
            if (auto titleBar = m_window->findChild<TitleBarWidget*>()) {
                if (titleBar->tabBar()) {
                    titleBar->tabBar()->addTab("全部数据", "library://-1", true);
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

    // 9. Ctrl+N: 新建灵感笔记
    registerShortcut(QKeySequence(Qt::CTRL | Qt::Key_N), [this]() {
        if (!isEditingFocus()) {
            emit createNoteRequested();
            emit createNewMdRequested();
        }
    });

    // 10. Ctrl+Shift+N: 新建笔记分类
    registerShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_N), [this]() {
        if (!isEditingFocus()) {
            emit createCategoryRequested();
            emit createNewFolderRequested();
        }
    });

    // 11. Ctrl+D: 清空筛选
    registerShortcut(QKeySequence(Qt::CTRL | Qt::Key_D), [this]() {
        if (!isEditingFocus()) {
            emit resetFilterRequested();
        }
    });

    // 12. Ctrl+Space: 移入回收站
    registerShortcut(QKeySequence(Qt::CTRL | Qt::Key_Space), [this]() {
        if (!isEditingFocus()) {
            emit moveToTrashRequested();
        }
    });

    // 13. Alt+B: 收藏标星
    registerShortcut(QKeySequence(Qt::ALT | Qt::Key_B), [this]() {
        if (!isEditingFocus()) {
            emit toggleFavoriteRequested();
        }
    });

    // 14. Alt+C: 提取标题
    registerShortcut(QKeySequence(Qt::ALT | Qt::Key_C), [this]() {
        if (!isEditingFocus()) {
            emit copyTitlesRequested();
            emit copyNamesEnumeratedRequested();
        }
    });
}

} // namespace RapidNotes
