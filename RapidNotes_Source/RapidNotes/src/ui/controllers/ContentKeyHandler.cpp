#include "ContentKeyHandler.h"
#include "../ContentPanel.h"
#include "../../core/CoreEngine.h"
#include "../ToolTipOverlay.h"
#include "../../core/ModelContract.h"

#include <QAbstractItemView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QApplication>
#include <QDir>

namespace RapidNotes {

ContentKeyHandler::ContentKeyHandler(ContentPanel* panel, QObject* parent)
    : QObject(parent), m_panel(panel) {
}

bool ContentKeyHandler::handleEvent(QObject* obj, QEvent* event) {
    if (!event || !m_panel) return false;

    switch (event->type()) {
    case QEvent::Wheel:
        return handleWheel(obj, event);
    case QEvent::MouseButtonPress:
        return handleMousePress(obj, event);
    case QEvent::KeyPress:
        return handleKeyPress(obj, event);
    default:
        break;
    }
    return false;
}

bool ContentKeyHandler::navigateViewFirstItem(QAbstractItemView* view) {
    if (!view || !view->model()) return false;
    QAbstractItemModel* model = view->model();
    int rowCount = model->rowCount();
    for (int r = 0; r < rowCount; ++r) {
        QModelIndex idx = model->index(r, 0);
        if (!idx.isValid() || idx.data(SectionHeaderRole).toBool()) continue;
        view->setCurrentIndex(idx);
        if (view->selectionModel()) {
            view->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        }
        view->scrollTo(idx);
        return true;
    }
    return false;
}

bool ContentKeyHandler::navigateViewLastItem(QAbstractItemView* view) {
    if (!view || !view->model()) return false;
    QAbstractItemModel* model = view->model();
    int rowCount = model->rowCount();
    for (int r = rowCount - 1; r >= 0; --r) {
        QModelIndex idx = model->index(r, 0);
        if (!idx.isValid() || idx.data(SectionHeaderRole).toBool()) continue;
        view->setCurrentIndex(idx);
        if (view->selectionModel()) {
            view->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        }
        view->scrollTo(idx);
        return true;
    }
    return false;
}

bool ContentKeyHandler::executeMoveToFolder(ContentPanel* panel, const QString& targetDir) {
    Q_UNUSED(panel);
    Q_UNUSED(targetDir);
    return false;
}

bool ContentKeyHandler::executeRepeatLastOp(ContentPanel* panel, QAbstractItemView* view) {
    Q_UNUSED(panel);
    Q_UNUSED(view);
    ToolTipOverlay::instance()->showText(QCursor::pos(), "尚未记录任何可重复的操作", 1500, QColor("#e81123"));
    return false;
}

bool ContentKeyHandler::handleWheel(QObject* obj, QEvent* event) {
    Q_UNUSED(obj);
    QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
    if (wheelEvent->modifiers() & Qt::ControlModifier) {
        int delta = wheelEvent->angleDelta().y();
        if (delta != 0) {
            int step = (delta > 0) ? 8 : -8;
            m_panel->setZoomLevel(m_panel->zoomLevel() + step);
            wheelEvent->accept();
            return true;
        }
    }
    return false;
}

bool ContentKeyHandler::handleMousePress(QObject* obj, QEvent* event) {
    Q_UNUSED(obj);
    Q_UNUSED(event);
    return false;
}

bool ContentKeyHandler::handleKeyPress(QObject* obj, QEvent* event) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
    if (qobject_cast<QLineEdit*>(QApplication::focusWidget())) return false;

    // 面板级全局快捷键
    if (keyEvent->modifiers() & Qt::ControlModifier) {
        if (keyEvent->key() == Qt::Key_S) {
            m_panel->toggleFolderSectionCollapse();
            return true;
        }
    }

    QAbstractItemView* view = qobject_cast<QAbstractItemView*>(obj);
    if (!view) view = qobject_cast<QAbstractItemView*>(obj->parent());
    if (!view) return false;

    // 1. Ctrl + 0~5: 评级
    if ((keyEvent->modifiers() & Qt::ControlModifier) && (keyEvent->key() >= Qt::Key_0 && keyEvent->key() <= Qt::Key_5)) {
        int rating = keyEvent->key() - Qt::Key_0;
        if (m_panel) {
            QStringList selectedPaths = m_panel->getSelectedPaths();
            if (!selectedPaths.isEmpty()) {
                AppCommand cmd;
                cmd.type = AppCommandType::SetRating;
                cmd.targetPaths = selectedPaths;
                cmd.params["rating"] = rating;
                CoreEngine::instance().executeCommand(cmd);

                for (const QString& p : selectedPaths) {
                    m_panel->updateItemMetadata(p);
                }
                m_panel->recalculateAndEmitStats();
            }
        }
        return true;
    }

    // 2. Alt + 1~9: 色标快速赋予
    if ((keyEvent->modifiers() & Qt::AltModifier) && (keyEvent->key() >= Qt::Key_1 && keyEvent->key() <= Qt::Key_9)) {
        static const QString colors[] = {
            "#E24B4A", "#EF9F27", "#FECF0E", "#639922",
            "#1D9E75", "#378ADD", "#7F77DD", "#5F5E5A", ""
        };
        QString colorValue = colors[keyEvent->key() - Qt::Key_1];
        if (m_panel) {
            QStringList selectedPaths = m_panel->getSelectedPaths();
            if (!selectedPaths.isEmpty()) {
                AppCommand cmd;
                cmd.type = AppCommandType::SetColor;
                cmd.targetPaths = selectedPaths;
                cmd.params["color"] = colorValue;
                CoreEngine::instance().executeCommand(cmd);

                for (const QString& p : selectedPaths) {
                    m_panel->updateItemMetadata(p);
                }
                m_panel->recalculateAndEmitStats();
            }
        }
        return true;
    }

    // 3. Ctrl + Home / Ctrl + End: 视图首项与末项导航
    if (keyEvent->modifiers() & Qt::ControlModifier) {
        if (keyEvent->key() == Qt::Key_Home) {
            if (navigateViewFirstItem(view)) return true;
        } else if (keyEvent->key() == Qt::Key_End) {
            if (navigateViewLastItem(view)) return true;
        }
    }

    // 4. F4: 重复上一次操作
    if (keyEvent->key() == Qt::Key_F4) {
        executeRepeatLastOp(m_panel, view);
        return true;
    }

    // 5. F2: 重命名
    if (keyEvent->key() == Qt::Key_F2) {
        QModelIndex idx = view->currentIndex();
        if (!idx.isValid() && view->selectionModel()) {
            auto selected = view->selectionModel()->selectedIndexes();
            if (!selected.isEmpty()) idx = selected.first();
        }
        if (idx.isValid()) {
            QModelIndex nameIdx = idx.sibling(idx.row(), 0);
            view->setCurrentIndex(nameIdx);
            view->edit(nameIdx);
        }
        return true;
    }

    // 6. 空格键与回车键: 激活当前项
    if ((keyEvent->key() == Qt::Key_Space && !(keyEvent->modifiers() & Qt::ControlModifier)) || keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
        QModelIndex idx = view->currentIndex();
        if (!idx.isValid() && view->selectionModel()) {
            auto selected = view->selectionModel()->selectedIndexes();
            if (!selected.isEmpty()) idx = selected.first();
        }
        if (idx.isValid()) {
            QModelIndex nameIdx = idx.sibling(idx.row(), 0);
            m_panel->activateIndex(nameIdx);
        }
        return true;
    }

    return false;
}

} // namespace RapidNotes
