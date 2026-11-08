#include "QuickLookController.h"
#include "ActivePaneCoordinator.h"
#include "QuickLookWindow.h"
#include "ToolTipOverlay.h"
#include "UiHelper.h"
#include "../meta/HistoryService.h"
#include "../core/CoreEngine.h"
#include "../core/TrashService.h"
#include <QFile>
#include <QFileInfo>
#include <QTimer>

namespace QuarkMeta {

QuickLookController::QuickLookController(ActivePaneCoordinator* activeCoordinator, MetaPanel* metaPanel, QObject* parent)
    : QObject(parent), m_activeCoordinator(activeCoordinator), m_metaPanel(metaPanel) {
}

QString QuickLookController::findAdjacentPreviewable(ContentPanel* target, const QString& currentPath, int direction) {
    if (!target || currentPath.isEmpty()) return QString();
    int step = (direction >= 0) ? 1 : -1;
    int delta = step;
    while (true) {
        QString cand = target->getAdjacentFilePath(currentPath, delta);
        if (cand.isEmpty()) break;
        if (!QFileInfo(cand).isDir() && UiHelper::canPreviewFile(cand)) {
            return cand;
        }
        delta += step;
    }
    return QString();
}

void QuickLookController::bindPane(ContentPanel* pane) {
    if (!pane) return;

    connect(pane, &ContentPanel::fileActivated, this, [](const QString& path) {
        if (path.isEmpty() || !QFile::exists(path)) {
            return;
        }
        HistoryService::instance().recordFileAccess(path);

        if (!UiHelper::canPreviewFile(path)) {
            return;
        }

        AppCommand cmd;
        cmd.type = AppCommandType::RecordAccess;
        cmd.targetPaths << path;
        CoreEngine::instance().executeCommand(cmd);

        QuickLookWindow::instance().preview(path);
    });
}

void QuickLookController::wire() {
    connect(&QuickLookWindow::instance(), &QuickLookWindow::prevRequested, this, [this]() {
        ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
        if (!target) return;
        QString curPath = QuickLookWindow::instance().currentPath();
        QString prev = findAdjacentPreviewable(target, curPath, -1);
        if (!prev.isEmpty()) {
            QuickLookWindow::instance().preview(prev);
            target->selectAndScrollToPath(prev);
        }
    });

    connect(&QuickLookWindow::instance(), &QuickLookWindow::nextRequested, this, [this]() {
        ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
        if (!target) return;
        QString curPath = QuickLookWindow::instance().currentPath();
        QString next = findAdjacentPreviewable(target, curPath, 1);
        if (!next.isEmpty()) {
            QuickLookWindow::instance().preview(next);
            target->selectAndScrollToPath(next);
        }
    });

    connect(&QuickLookWindow::instance(), &QuickLookWindow::ratingRequested, this, [this](int rating) {
        QString curPath = QuickLookWindow::instance().currentPath();
        if (curPath.isEmpty()) return;

        AppCommand cmd;
        cmd.type = AppCommandType::SetRating;
        cmd.targetPaths << curPath;
        cmd.params["rating"] = rating;
        CoreEngine::instance().executeCommand(cmd);

        if (m_metaPanel) m_metaPanel->setRating(rating, false);

        ToolTipOverlay::instance()->showRatingToast(
            rating, &QuickLookWindow::instance()
        );
    });

    connect(&QuickLookWindow::instance(), &QuickLookWindow::colorRequested, this, [this](const QString& color) {
        QString curPath = QuickLookWindow::instance().currentPath();
        if (curPath.isEmpty()) return;

        AppCommand cmd;
        cmd.type = AppCommandType::SetColor;
        cmd.targetPaths << curPath;
        cmd.params["color"] = color;
        CoreEngine::instance().executeCommand(cmd);

        if (m_metaPanel) m_metaPanel->setColor(color, false);

        ToolTipOverlay::instance()->showColorToast(
            color, &QuickLookWindow::instance()
        );
    });

    connect(&QuickLookWindow::instance(), &QuickLookWindow::deleteRequested, this, [this](const QString& path) {
        if (path.isEmpty()) return;
        ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
        if (!target) return;

        // 提前计算下一个有效文件（若末项则计算备选的上一个有效文件），防止物理删除后寻址失败
        QString nextTarget = findAdjacentPreviewable(target, path, 1);
        if (nextTarget.isEmpty()) {
            nextTarget = findAdjacentPreviewable(target, path, -1);
        }

        // 锁定 QuickLook 失焦关闭门禁，防止刷新与对话框导致预览被误关
        QuickLookWindow::instance().setIgnoreDeactivate(true);

        bool deleteOk = TrashService::instance().moveToTrash({path}, target);

        if (deleteOk) {
            target->refreshAll();

            if (!nextTarget.isEmpty() && QFile::exists(nextTarget)) {
                QuickLookWindow::instance().preview(nextTarget);
                target->selectAndScrollToPath(nextTarget);
            } else {
                QuickLookWindow::instance().closePreview();
            }
        }

        QTimer::singleShot(200, &QuickLookWindow::instance(), []() {
            QuickLookWindow::instance().setIgnoreDeactivate(false);
        });
    });
}

} // namespace QuarkMeta
