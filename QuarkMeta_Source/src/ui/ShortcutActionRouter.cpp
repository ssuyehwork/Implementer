#include "ShortcutActionRouter.h"
#include "ActivePaneCoordinator.h"
#include "controllers/ContextMenuFactory.h"
#include "ToolTipOverlay.h"
#include "../core/TrashService.h"
#include "../core/PermanentDeleteService.h"
#include "../meta/MarkedFolderService.h"
#include "../core/AppConfig.h"

namespace QuarkMeta {

ShortcutActionRouter::ShortcutActionRouter(ActivePaneCoordinator* activeCoordinator,
                                           AppShortcutController* shortcutController,
                                           TitleBarWidget* titleBar,
                                           PanelLayoutManager* layoutManager,
                                           FilterPanel* filterPanel,
                                           QObject* parent)
    : QObject(parent),
      m_activeCoordinator(activeCoordinator),
      m_shortcutController(shortcutController),
      m_titleBar(titleBar),
      m_layoutManager(layoutManager),
      m_filterPanel(filterPanel) {
}

void ShortcutActionRouter::wire() {
    if (m_titleBar) {
        if (m_layoutManager) {
            connect(m_titleBar, &TitleBarWidget::layoutMenuRequested, m_layoutManager, [this](const QPoint& pos) {
                if (m_layoutManager) {
                    m_layoutManager->showPanelContextMenu(pos);
                }
            });
        }

        connect(m_titleBar, &TitleBarWidget::viewModeRequested, this, [this](TitleBarWidget::ViewModeOption option) {
            ContentPanel::ViewMode targetMode = ContentPanel::GridView;
            if (option == TitleBarWidget::JustifiedViewMode) targetMode = ContentPanel::JustifiedViewMode;
            else if (option == TitleBarWidget::GridViewMode) targetMode = ContentPanel::GridView;
            else if (option == TitleBarWidget::ListViewMode) targetMode = ContentPanel::ListView;
            else if (option == TitleBarWidget::ColumnViewMode) targetMode = ContentPanel::ColumnView;

            ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
            if (target) target->setViewMode(targetMode);
        });

        connect(m_titleBar, &TitleBarWidget::createItemRequested, this, [this](const QString& type) {
            ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
            if (target) target->createNewItem(type);
        });

        // 缩放级别初始化与双向同步 + 持久化
        int initZoom = AppConfig::instance().getValue("UI/GridZoomLevel", 96).toInt();
        int boundZoom = qBound(30, initZoom, 230);
        m_titleBar->setZoomLevel(boundZoom);
        ContentPanel* root = m_activeCoordinator ? m_activeCoordinator->rootPane() : nullptr;
        if (root) {
            root->setZoomLevel(boundZoom);
        }

        connect(m_titleBar, &TitleBarWidget::zoomLevelChanged, this, [this](int value) {
            ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
            if (target) target->setZoomLevel(value);
            AppConfig::instance().setValue("UI/GridZoomLevel", value);
        });

        if (root) {
            connect(root, &ContentPanel::zoomLevelChanged, this, [this](int level) {
                if (m_titleBar) m_titleBar->setZoomLevel(level);
                AppConfig::instance().setValue("UI/GridZoomLevel", level);
            });
        }
    }

    if (m_shortcutController) {
        if (m_layoutManager) {
            connect(m_shortcutController, &AppShortcutController::toggleImmersiveRequested, m_layoutManager, [this]() {
                if (m_layoutManager) {
                    m_layoutManager->toggleImmersiveMode();
                }
            });
        }

        connect(m_shortcutController, &AppShortcutController::createNewFolderRequested, this, [this]() {
            ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
            if (target) {
                target->createNewItem("folder");
            }
        });

        connect(m_shortcutController, &AppShortcutController::createNewMdRequested, this, [this]() {
            ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
            if (target) {
                target->createNewItem("md");
            }
        });

        connect(m_shortcutController, &AppShortcutController::extractContentRequested, this, [this]() {
            ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
            if (target) {
                QStringList selectedPaths = target->getSelectedPaths();
                if (!selectedPaths.isEmpty()) {
                    ContextMenuFactory::extractContentToClipboard(selectedPaths.first());
                } else {
                    ToolTipOverlay::instance()->showToast("未选择任何可提取内容的文件", 1200, QColor("#e81123"));
                }
            }
        });

        connect(m_shortcutController, &AppShortcutController::toggleRecursiveRequested, this, [this]() {
            ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
            if (target && !target->currentPath().isEmpty() && target->currentPath() != "computer://") {
                bool nextState = !target->isRecursive();
                target->setIsRecursive(nextState);
                target->loadDirectory(target->currentPath(), nextState);
            }
        });

        if (m_filterPanel) {
            connect(m_shortcutController, &AppShortcutController::resetFilterRequested, m_filterPanel, [this]() {
                if (m_filterPanel) {
                    m_filterPanel->clearAllFilters(true);
                }
            });
        }

        connect(m_shortcutController, &AppShortcutController::moveToTrashRequested, this, [this]() {
            ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
            if (target) {
                bool isTrashView = (target->getCurrentCategoryType() == "trash" || target->currentPath() == "trash://");
                if (isTrashView) {
                    PermanentDeleteService::instance().executeEmptyTrash(target);
                } else {
                    QStringList sel = target->getSelectedPaths();
                    if (!sel.isEmpty()) {
                        TrashService::instance().moveToTrash(sel, target);
                    }
                }
            }
        });

        connect(m_shortcutController, &AppShortcutController::toggleMarkedRequested, this, [this]() {
            ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
            if (target) {
                QStringList sel = target->getSelectedPaths();
                if (!sel.isEmpty()) {
                    QuarkMeta::MarkedFolderService::instance().toggleMarked(sel);
                }
            }
        });

        connect(m_shortcutController, &AppShortcutController::transferToMarkedRequested, this, []() {
            QuarkMeta::MarkedFolderService::instance().executeTransferToMarked({});
        });

        connect(m_shortcutController, &AppShortcutController::copyNamesEnumeratedRequested, this, [this]() {
            ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
            if (target) {
                QStringList sel = target->getSelectedPaths();
                if (!sel.isEmpty()) {
                    ContextMenuFactory::copyNamesToClipboard(sel, ContextMenuFactory::NameDelimiter::Enumeration, true);
                }
            }
        });
    }
}

} // namespace QuarkMeta
