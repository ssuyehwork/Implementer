#include "TabBarCoordinator.h"
#include "ActivePaneCoordinator.h"
#include "controllers/ContentPaneSplitManager.h"
#include "ToolTipOverlay.h"
#include "../core/NavigationService.h"
#include "../core/CoreEngine.h"
#include "../meta/FavoriteDao.h"

namespace QuarkMeta {

TabBarCoordinator::TabBarCoordinator(ActivePaneCoordinator* activeCoordinator,
                                     TabBarWidget* tabBar,
                                     const std::function<void()>& refreshCurrentFunc,
                                     QObject* parent)
    : QObject(parent),
      m_activeCoordinator(activeCoordinator),
      m_tabBar(tabBar),
      m_refreshCurrentFunc(refreshCurrentFunc) {
}

void TabBarCoordinator::exportAndSaveSplitState() {
    if (!m_tabBar || !m_activeCoordinator) return;
    ContentPanel* root = m_activeCoordinator->rootPane();
    if (root && root->splitManager()) {
        TabSplitState state = root->splitManager()->exportSplitState();
        m_tabBar->setTabSplitState(m_tabBar->currentIndex(), state);
    }
}

void TabBarCoordinator::wire() {
    if (!m_tabBar || !m_activeCoordinator) return;
    ContentPanel* root = m_activeCoordinator->rootPane();

    // 切出旧 Tab 前：快照导出旧 Tab 的完整分屏树
    connect(m_tabBar, &TabBarWidget::tabAboutToChange, this, [this, root](int oldIndex) {
        if (root && root->splitManager() && m_tabBar) {
            TabSplitState state = root->splitManager()->exportSplitState();
            m_tabBar->setTabSplitState(oldIndex, state);
        }
    });

    // 切换到新 Tab：还原新 Tab 的专属分屏树并通知 NavigationService
    connect(m_tabBar, &TabBarWidget::currentTabChanged, this, [this, root](int index, const QString& url) {
        if (root && root->splitManager() && m_tabBar) {
            TabSplitState state = m_tabBar->tabSplitState(index);
            root->splitManager()->restoreSplitState(state);
        }
        NavigationService::instance().navigateTo(url);
    });

    // 分屏窗格结构或路径改变时，自动同步更新多窗格标签页标题
    if (root && root->splitManager()) {
        connect(root->splitManager(), &ContentPaneSplitManager::layoutChanged, this, [this, root]() {
            if (m_tabBar && root->splitManager()) {
                TabSplitState state = root->splitManager()->exportSplitState();
                m_tabBar->updateSplitTabTitle(state);
            }
        });
    }

    // 标签页刷新请求
    connect(m_tabBar, &TabBarWidget::refreshRequested, this, [this]() {
        if (m_refreshCurrentFunc) {
            m_refreshCurrentFunc();
        }
    });

    // 标签页合并请求
    connect(m_tabBar, &TabBarWidget::mergeRequested, this, [this, root](int sourceIndex, int targetIndex) {
        if (root && root->splitManager() && m_tabBar) {
            TabSplitState state = root->splitManager()->exportSplitState();
            m_tabBar->setTabSplitState(m_tabBar->currentIndex(), state);

            TabSplitState srcState = m_tabBar->tabSplitState(sourceIndex);
            TabSplitState tgtState = m_tabBar->tabSplitState(targetIndex);

            int srcCount = (srcState.isSplit && !srcState.panePaths.isEmpty()) ? srcState.panePaths.size() : 1;
            int tgtCount = (tgtState.isSplit && !tgtState.panePaths.isEmpty()) ? tgtState.panePaths.size() : 1;

            if (srcCount + tgtCount > ContentPanel::kMaxPanes) {
                ToolTipOverlay::instance()->showToast(QString("窗格数量超出上限%1窗格，不支持合并").arg(ContentPanel::kMaxPanes), 2000, QColor("#e81123"));
                return;
            }

            m_tabBar->mergeTab(sourceIndex, targetIndex);
        }
    });

    // 标签页拆分请求
    connect(m_tabBar, &TabBarWidget::splitRequested, this, [this, root](int tabIndex) {
        if (root && root->splitManager() && m_tabBar) {
            TabSplitState state = root->splitManager()->exportSplitState();
            m_tabBar->setTabSplitState(m_tabBar->currentIndex(), state);
            m_tabBar->splitTab(tabIndex);
        }
    });

    // 拖拽文件夹到标签页分屏
    connect(m_tabBar, &TabBarWidget::folderDroppedOnTab, this, [this, root](int tabIndex, const QString& folderPath) {
        if (!m_tabBar) return;
        m_tabBar->setCurrentIndex(tabIndex, true);
        if (root) {
            if (root->paneCount() >= ContentPanel::kMaxPanes) {
                ToolTipOverlay::instance()->showToast(QString("窗格数量超出上限%1窗格，不支持继续分屏").arg(ContentPanel::kMaxPanes), 2000, QColor("#e81123"));
                return;
            }
            root->splitPane(Qt::Horizontal, folderPath);
        }
    });

    // 标签页颜色与图标变更
    connect(m_tabBar, &TabBarWidget::tabColorChanged, this, [](int /*index*/, const QString& url, const QString& hexColor) {
        if (!url.isEmpty() && !url.startsWith("computer://")) {
            AppCommand cmd;
            cmd.type = AppCommandType::SetColor;
            cmd.targetPaths = {url};
            cmd.params["color"] = hexColor;
            CoreEngine::instance().executeCommand(cmd);
        }
    });

    connect(m_tabBar, &TabBarWidget::tabIconChanged, this, [](int /*index*/, const QString& url, const QString& iconKey) {
        if (!url.isEmpty()) {
            FavoriteDao::updateFavorite(url, iconKey, "");
        }
    });
}

} // namespace QuarkMeta
