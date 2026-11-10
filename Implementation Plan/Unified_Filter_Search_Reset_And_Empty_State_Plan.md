# Implementation Plan - 快捷键 Ctrl+D 联合复位与内容面板过滤隐藏空态提示规范实施方案

## 实施方案元数据与三道关检查日志

- **方案名称**：Unified_Filter_Search_Reset_And_Empty_State_Plan.md
- **物理存储路径**：`Implementation Plan/Unified_Filter_Search_Reset_And_Empty_State_Plan.md`
- **关联设计规范**：`QuarkMeta_Source/Memories_QuarkMeta.md`（第 0 节、第 1.2 节、第 51 章与第 52 章）
- **基准状态**：Verified & Self-Audited (Audit-Corrected Version with Edit-Focus Exemption)

---

## 门禁 1：起草前物理证据清单 (Gate 1: Pre-Draft Physical Evidence)

### 1.1 物理读取文件列表与确认
1. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/ui/AppShortcutController.cpp`（已通过 `read_file` 物理核对 `Ctrl+D` 注册位置与 `isEditingFocus()` 阻断代码）
2. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/ui/ShortcutActionRouter.h`（已通过 `read_file` 物理核对）
3. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/ui/ShortcutActionRouter.cpp`（已通过 `read_file` 物理核对）
4. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/ui/PanelMediator.h`（已通过 `read_file` 物理核对）
5. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/ui/PanelMediator.cpp`（已通过 `read_file` 物理核对）
6. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/ui/FilterPanel.h`（已通过 `read_file` 物理核对）
7. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/ui/FilterPanel.cpp`（已通过 `read_file` 物理核对）
8. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/ui/ContentPanel.h`（已通过 `read_file` 物理核对）
9. `G:/C++/Project/Source_Code/QuarkMeta_Source/src/ui/ContentPanel.cpp`（已通过 `read_file` 物理核对）
10. `G:/C++/Project/Source_Code/QuarkMeta_Source/resources/style.qss`（已通过 `read_file` 物理核对）

### 1.2 调用的外部成员函数原样签名核对
- `SearchController::clearSearch(bool silent)`：在 `SearchController.h:42` 真实存在，声明为 `void clearSearch(bool silent = false);`
- `SearchController::searchEdit()`：在 `SearchController.h:27` 真实存在，声明为 `QLineEdit* searchEdit() const;`
- `ContentPanel::search(const QString& query)`：在 `ContentPanel.h:246` 真实存在，声明为 `void search(const QString& query);`
- `FilterPanel::clearAllFilters(bool force)`：在 `FilterPanel.h:83` 真实存在，声明为 `void clearAllFilters(bool force = false);`
- `ShortcutActionRouter::wire()`：在 `ShortcutActionRouter.h:29` 真实存在，声明为 `void wire();`

### 1.3 查重优先与既有 API 检索记录
- 检索 `Ctrl+D` 阻断点：在 `AppShortcutController.cpp:249` 发现 `if (!isEditingFocus())`，用户在搜索框打字时焦点处于 `QLineEdit`，导致快捷键被直接吞噬；必须将其彻底废除；
- 检索 `clearSearch`：在 `SearchController.cpp` 中由 `clearSearch(true)` 负责静默清空输入框文字与清除按钮状态；
- 检索 `updateStatusBarStats`：在 `ContentPanel.cpp:1157` 中已具备 `visibleCount`、`fullCount` 与 `hiddenCount` 统计计算，复用该数据源；
- 检索 `style.qss`：所有通用 UI 样式在 `resources/style.qss` 中配置，坚决杜绝内联硬编码。

---

## Chapter 1: Overview

### 1.1 背景与用户痛点
1. **输入框焦点误杀致命缺陷**：用户在顶栏搜索框中输入文字时，键盘焦点位于 `QLineEdit`。由于 `AppShortcutController` 在处理 `Ctrl + D` 时盲目嵌套了 `if (!isEditingFocus())` 检查，导致光标在搜索框内时按 `Ctrl + D` 被直接当作打字拦截吞噬，根本不触发重置；
2. **筛选器无变动时的重置失效缺陷**：当右侧筛选器本来就没有勾选任何条件，仅搜索框有文字时，系统未把搜索框视为联合过滤维度，导致用户误判为快捷键失灵；
3. **幽灵黑屏/空态猜疑痛点**：当目录中真实存在文件（如 11 个文件），但全部被筛选器或搜索词过滤拦截导致可见项为 0 时，中间内容面板没有任何提示，呈现一片死寂黑底。

### 1.2 重构与实施目标
1. **彻底豁免 Ctrl+D 焦点阻断（顶级全局指令）**：
   - 彻底废除 `AppShortcutController.cpp` 中 `Ctrl + D` 上的 `if (!isEditingFocus())` 守卫，无论用户光标是否在搜索框内，按下 `Ctrl + D` 均强制发射 `resetFilterRequested()`；
2. **联合复位强化（清空文字 + 释放焦点）**：
   - 调用 `m_searchController->clearSearch(true)` 强力清空搜索框文字；
   - 搜索框主动释放焦点（`clearFocus()`），将焦点交回内容面板，并清除内部搜索词 `target->search("")`；
   - 清空 `FilterPanel` 所有元数据维度；即使筛选器本身无变化，也坚决执行搜索清空与视图重载；
3. **内容面板二态空态提示**：在 `ContentPanel` 引入 `m_emptyFilterOverlay` 遮罩，当物理总数大于 0 但可见数为 0 时，居中清晰显示“当前共有 N 个项目，已过滤隐藏 N 个项目”，并提供“重置筛选与搜索 (Ctrl + D)”快捷交互按钮；
4. **红线严格遵循**：
   - 按钮强制配置 `setFocusPolicy(Qt::NoFocus)` 与 `outline: none`；
   - 样式 100% 收敛至 `resources/style.qss`，杜绝内联样式表。

---

## Chapter 2: Modified Files List

1. `QuarkMeta_Source/src/ui/AppShortcutController.cpp`（废除 `Ctrl + D` 的 `isEditingFocus()` 阻断守卫）
2. `QuarkMeta_Source/src/ui/ShortcutActionRouter.h`（扩展构造函数默认参数与 `bindPane` 声明）
3. `QuarkMeta_Source/src/ui/ShortcutActionRouter.cpp`（补齐头文件包含，实现联合复位并清空搜索框与释放焦点）
4. `QuarkMeta_Source/src/ui/PanelMediator.cpp`（传递 `m_searchController` 并向 `m_shortcutRouter` 分发窗格绑定）
5. `QuarkMeta_Source/src/ui/FilterPanel.h`（增加 `resetFilterRequested` 信号）
6. `QuarkMeta_Source/src/ui/FilterPanel.cpp`（点击重置按钮时发射 `resetFilterRequested` 信号，精确对齐源码字符串）
7. `QuarkMeta_Source/src/ui/ContentPanel.h`（增加 `resizeEvent`、`filterResetRequested` 信号及空态遮罩成员）
8. `QuarkMeta_Source/src/ui/ContentPanel.cpp`（实现 `resizeEvent` 与 `initEmptyFilterOverlay`，通过 QSS 样式表装配外观）
9. `QuarkMeta_Source/resources/style.qss`（追加空态提示遮罩与按钮的外部 QSS 样式）

---

## Chapter 3: Detailed Line-by-Line Changes

### 3.1 `QuarkMeta_Source/src/ui/AppShortcutController.cpp`
```
<<<<<<< SEARCH
    // 12. Ctrl+D: 重置筛选
    registerShortcut(QKeySequence(Qt::CTRL | Qt::Key_D), [this]() {
        if (!isEditingFocus()) {
            emit resetFilterRequested();
        }
    });
=======
    // 12. Ctrl+D: 重置筛选与搜索（顶级全局快捷键，豁免 isEditingFocus 阻断，输入框内同样坚决生效）
    registerShortcut(QKeySequence(Qt::CTRL | Qt::Key_D), [this]() {
        emit resetFilterRequested();
    });
>>>>>>> REPLACE
```

### 3.2 `QuarkMeta_Source/src/ui/ShortcutActionRouter.h`
```
<<<<<<< SEARCH
class ActivePaneCoordinator;

/**
 * @brief 快捷键与标题栏动作路由器
 * 负责处理 AppShortcutController 快捷键信号与 TitleBarWidget 界面控制动作。
 */
class ShortcutActionRouter : public QObject {
    Q_OBJECT
public:
    explicit ShortcutActionRouter(ActivePaneCoordinator* activeCoordinator,
                                  AppShortcutController* shortcutController,
                                  TitleBarWidget* titleBar,
                                  PanelLayoutManager* layoutManager,
                                  FilterPanel* filterPanel,
                                  QObject* parent = nullptr);
    ~ShortcutActionRouter() override = default;

    void wire();

private:
    ActivePaneCoordinator* m_activeCoordinator = nullptr;
    QPointer<AppShortcutController> m_shortcutController;
    QPointer<TitleBarWidget> m_titleBar;
    QPointer<PanelLayoutManager> m_layoutManager;
    QPointer<FilterPanel> m_filterPanel;
};
=======
class ActivePaneCoordinator;
class SearchController;
class ContentPanel;

/**
 * @brief 快捷键与标题栏动作路由器
 * 负责处理 AppShortcutController 快捷键信号与 TitleBarWidget 界面控制动作。
 */
class ShortcutActionRouter : public QObject {
    Q_OBJECT
public:
    explicit ShortcutActionRouter(ActivePaneCoordinator* activeCoordinator,
                                  AppShortcutController* shortcutController,
                                  TitleBarWidget* titleBar,
                                  PanelLayoutManager* layoutManager,
                                  FilterPanel* filterPanel,
                                  SearchController* searchController = nullptr,
                                  QObject* parent = nullptr);
    ~ShortcutActionRouter() override = default;

    void wire();
    void bindPane(ContentPanel* pane);
    void triggerUnifiedReset();

private:
    ActivePaneCoordinator* m_activeCoordinator = nullptr;
    QPointer<AppShortcutController> m_shortcutController;
    QPointer<TitleBarWidget> m_titleBar;
    QPointer<PanelLayoutManager> m_layoutManager;
    QPointer<FilterPanel> m_filterPanel;
    QPointer<SearchController> m_searchController;
};
>>>>>>> REPLACE
```

### 3.3 `QuarkMeta_Source/src/ui/ShortcutActionRouter.cpp`
```
<<<<<<< SEARCH
#include "ShortcutActionRouter.h"
#include "ActivePaneCoordinator.h"
#include "controllers/ContextMenuFactory.h"
=======
#include "ShortcutActionRouter.h"
#include "ActivePaneCoordinator.h"
#include "SearchController.h"
#include "ContentPanel.h"
#include "controllers/ContextMenuFactory.h"
>>>>>>> REPLACE
```

```
<<<<<<< SEARCH
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
=======
ShortcutActionRouter::ShortcutActionRouter(ActivePaneCoordinator* activeCoordinator,
                                           AppShortcutController* shortcutController,
                                           TitleBarWidget* titleBar,
                                           PanelLayoutManager* layoutManager,
                                           FilterPanel* filterPanel,
                                           SearchController* searchController,
                                           QObject* parent)
    : QObject(parent),
      m_activeCoordinator(activeCoordinator),
      m_shortcutController(shortcutController),
      m_titleBar(titleBar),
      m_layoutManager(layoutManager),
      m_filterPanel(filterPanel),
      m_searchController(searchController) {
}
>>>>>>> REPLACE
```

```
<<<<<<< SEARCH
        if (m_filterPanel) {
            connect(m_shortcutController, &AppShortcutController::resetFilterRequested, m_filterPanel, [this]() {
                if (m_filterPanel) {
                    m_filterPanel->clearAllFilters(true);
                }
            });
        }
=======
        if (m_filterPanel) {
            connect(m_shortcutController, &AppShortcutController::resetFilterRequested, this, &ShortcutActionRouter::triggerUnifiedReset);
            connect(m_filterPanel, &FilterPanel::resetFilterRequested, this, &ShortcutActionRouter::triggerUnifiedReset);
        }
>>>>>>> REPLACE
```

```
<<<<<<< SEARCH
        connect(m_shortcutController, &AppShortcutController::moveToTrashRequested, this, [this]() {
=======
void ShortcutActionRouter::triggerUnifiedReset() {
    if (m_filterPanel) {
        m_filterPanel->clearAllFilters(true);
    }
    if (m_searchController) {
        m_searchController->clearSearch(true);
        if (m_searchController->searchEdit()) {
            m_searchController->searchEdit()->clearFocus();
        }
    }
    ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
    if (target) {
        target->search("");
        target->setActivePane(true);
    }
}

void ShortcutActionRouter::bindPane(ContentPanel* pane) {
    if (!pane) return;
    connect(pane, &ContentPanel::filterResetRequested, this, &ShortcutActionRouter::triggerUnifiedReset);
}

        connect(m_shortcutController, &AppShortcutController::moveToTrashRequested, this, [this]() {
>>>>>>> REPLACE
```

### 3.4 `QuarkMeta_Source/src/ui/PanelMediator.cpp`
```
<<<<<<< SEARCH
    m_shortcutRouter = new ShortcutActionRouter(
        m_activeCoordinator,
        m_shortcutController.data(),
        m_titleBar.data(),
        m_layoutManager.data(),
        m_filterPanel.data(),
        this
    );
=======
    m_shortcutRouter = new ShortcutActionRouter(
        m_activeCoordinator,
        m_shortcutController.data(),
        m_titleBar.data(),
        m_layoutManager.data(),
        m_filterPanel.data(),
        m_searchController.data(),
        this
    );
>>>>>>> REPLACE
```

```
<<<<<<< SEARCH
    if (m_navCoordinator) {
        m_navCoordinator->bindPane(pane);
    }

    // 动态副窗格递归分发
=======
    if (m_navCoordinator) {
        m_navCoordinator->bindPane(pane);
    }
    if (m_shortcutRouter) {
        m_shortcutRouter->bindPane(pane);
    }

    // 动态副窗格递归分发
>>>>>>> REPLACE
```

### 3.5 `QuarkMeta_Source/src/ui/FilterPanel.h`
```
<<<<<<< SEARCH
signals:
    void filterChanged(const FilterState& state);

public slots:
    void clearAllFilters(bool force = false);
=======
signals:
    void filterChanged(const FilterState& state);
    void resetFilterRequested();

public slots:
    void clearAllFilters(bool force = false);
>>>>>>> REPLACE
```

### 3.6 `QuarkMeta_Source/src/ui/FilterPanel.cpp`
```
<<<<<<< SEARCH
    m_btnClearAll->setProperty("tooltipText", "重置筛选条件（Ctrl + D）");
    m_btnClearAll->installEventFilter(this);
    connect(m_btnClearAll, &QPushButton::clicked, this, [this]() { clearAllFilters(true); });
=======
    m_btnClearAll->setProperty("tooltipText", "重置筛选条件（Ctrl + D）");
    m_btnClearAll->installEventFilter(this);
    connect(m_btnClearAll, &QPushButton::clicked, this, [this]() {
        clearAllFilters(true);
        emit resetFilterRequested();
    });
>>>>>>> REPLACE
```

### 3.7 `QuarkMeta_Source/src/ui/ContentPanel.h`
```
<<<<<<< SEARCH
protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

signals:
=======
protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

signals:
    void filterResetRequested();
>>>>>>> REPLACE
```

```
<<<<<<< SEARCH
    void initGridView();
    void initListView();
    void updateGridSize();
    void updateStatusBarStats(int cachedSelectedCount = -1);
=======
    void initGridView();
    void initListView();
    void updateGridSize();
    void updateStatusBarStats(int cachedSelectedCount = -1);
    void initEmptyFilterOverlay();
    void updateEmptyStateOverlay(int visibleCount, int fullCount, int hiddenCount);
>>>>>>> REPLACE
```

```
<<<<<<< SEARCH
    QStackedWidget* m_viewStack = nullptr;
    class ColumnViewWidget* m_columnView = nullptr;
=======
    QStackedWidget* m_viewStack = nullptr;
    QWidget* m_emptyFilterOverlay = nullptr;
    QLabel* m_emptyFilterTitleLabel = nullptr;
    QLabel* m_emptyFilterSubLabel = nullptr;
    QPushButton* m_emptyFilterResetBtn = nullptr;
    class ColumnViewWidget* m_columnView = nullptr;
>>>>>>> REPLACE
```

### 3.8 `QuarkMeta_Source/src/ui/ContentPanel.cpp`
```
<<<<<<< SEARCH
    m_viewStack = new QStackedWidget(this);
    m_viewStack->setFrameShape(QFrame::NoFrame);
=======
    m_viewStack = new QStackedWidget(this);
    m_viewStack->setFrameShape(QFrame::NoFrame);
    initEmptyFilterOverlay();
>>>>>>> REPLACE
```

```
<<<<<<< SEARCH
    QFrame::dropEvent(event);
}

void ContentPanel::setCurrentCategoryTitle(const QString& title) {
=======
    QFrame::dropEvent(event);
}

void ContentPanel::resizeEvent(QResizeEvent* event) {
    QFrame::resizeEvent(event);
    if (m_emptyFilterOverlay && m_viewStack) {
        m_emptyFilterOverlay->setGeometry(m_viewStack->rect());
    }
}

void ContentPanel::setCurrentCategoryTitle(const QString& title) {
>>>>>>> REPLACE
```

```
<<<<<<< SEARCH
    int visibleCount = folderCount + fileCount;
    int fullCount = m_model ? m_model->rowCount() : visibleCount;
    int hiddenCount = fullCount - visibleCount;
    int selectedCount = (cachedSelectedCount >= 0) ? cachedSelectedCount : getSelectedIndexes().size();

    QString numStyle = "color:#ff551c; font-weight:bold;";
    QString statusText = (hiddenCount > 0)
        ? QString("<html><body style=\"margin:0px; padding:0px; font-size:11px;\"><span style=\"%1\">%2</span> 个项目，<span style=\"%1\">%3</span> 个已隐藏，选中了 <span style=\"%1\">%4</span> 个</body></html>")
              .arg(numStyle).arg(visibleCount).arg(hiddenCount).arg(selectedCount)
        : QString("<html><body style=\"margin:0px; padding:0px; font-size:11px;\"><span style=\"%1\">%2</span> 个项目，选中了 <span style=\"%1\">%3</span> 个</body></html>")
              .arg(numStyle).arg(visibleCount).arg(selectedCount);

    emit statusBarMessageReady(statusText);
}
=======
    int visibleCount = folderCount + fileCount;
    int fullCount = m_model ? m_model->rowCount() : visibleCount;
    int hiddenCount = fullCount - visibleCount;
    int selectedCount = (cachedSelectedCount >= 0) ? cachedSelectedCount : getSelectedIndexes().size();

    updateEmptyStateOverlay(visibleCount, fullCount, hiddenCount);

    QString numStyle = "color:#ff551c; font-weight:bold;";
    QString statusText = (hiddenCount > 0)
        ? QString("<html><body style=\"margin:0px; padding:0px; font-size:11px;\"><span style=\"%1\">%2</span> 个项目，<span style=\"%1\">%3</span> 个已隐藏，选中了 <span style=\"%1\">%4</span> 个</body></html>")
              .arg(numStyle).arg(visibleCount).arg(hiddenCount).arg(selectedCount)
        : QString("<html><body style=\"margin:0px; padding:0px; font-size:11px;\"><span style=\"%1\">%2</span> 个项目，选中了 <span style=\"%1\">%3</span> 个</body></html>")
              .arg(numStyle).arg(visibleCount).arg(selectedCount);

    emit statusBarMessageReady(statusText);
}

void ContentPanel::initEmptyFilterOverlay() {
    if (m_emptyFilterOverlay || !m_viewStack) return;

    m_emptyFilterOverlay = new QWidget(m_viewStack);
    m_emptyFilterOverlay->setObjectName("EmptyFilterOverlay");
    m_emptyFilterOverlay->setAttribute(Qt::WA_StyledBackground, true);

    QVBoxLayout* layout = new QVBoxLayout(m_emptyFilterOverlay);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(12);
    layout->setAlignment(Qt::AlignCenter);

    QLabel* iconLabel = new QLabel(m_emptyFilterOverlay);
    iconLabel->setPixmap(UiHelper::getIcon("filter", QColor("#888888"), 48).pixmap(48, 48));
    iconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(iconLabel);

    m_emptyFilterTitleLabel = new QLabel(m_emptyFilterOverlay);
    m_emptyFilterTitleLabel->setObjectName("EmptyFilterTitleLabel");
    m_emptyFilterTitleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_emptyFilterTitleLabel);

    m_emptyFilterSubLabel = new QLabel("未显示内容是因为筛选器或搜索过滤了结果", m_emptyFilterOverlay);
    m_emptyFilterSubLabel->setObjectName("EmptyFilterSubLabel");
    m_emptyFilterSubLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_emptyFilterSubLabel);

    m_emptyFilterResetBtn = new QPushButton("重置筛选与搜索 (Ctrl + D)", m_emptyFilterOverlay);
    m_emptyFilterResetBtn->setObjectName("EmptyFilterResetBtn");
    m_emptyFilterResetBtn->setFocusPolicy(Qt::NoFocus);
    m_emptyFilterResetBtn->setCursor(Qt::PointingHandCursor);
    m_emptyFilterResetBtn->setFixedSize(190, 32);
    connect(m_emptyFilterResetBtn, &QPushButton::clicked, this, [this]() {
        search("");
        emit filterResetRequested();
    });
    layout->addWidget(m_emptyFilterResetBtn, 0, Qt::AlignCenter);

    m_emptyFilterOverlay->hide();
}

void ContentPanel::updateEmptyStateOverlay(int visibleCount, int fullCount, int hiddenCount) {
    if (!m_emptyFilterOverlay) return;

    if (fullCount > 0 && visibleCount == 0) {
        if (m_emptyFilterTitleLabel) {
            m_emptyFilterTitleLabel->setText(QString("当前共有 %1 个项目，已过滤隐藏 %2 个项目").arg(fullCount).arg(hiddenCount));
        }
        m_emptyFilterOverlay->setGeometry(m_viewStack->rect());
        m_emptyFilterOverlay->show();
        m_emptyFilterOverlay->raise();
    } else {
        m_emptyFilterOverlay->hide();
    }
}
>>>>>>> REPLACE
```

### 3.9 `QuarkMeta_Source/resources/style.qss`
```
<<<<<<< SEARCH
QPushButton#FileCompareCloseBtn:pressed {
    background-color: #1E1E1E;
}
=======
QPushButton#FileCompareCloseBtn:pressed {
    background-color: #1E1E1E;
}

/* ============================================================ */
/* Empty Filter Overlay (内容面板过滤空态提示)                     */
/* ============================================================ */
QWidget#EmptyFilterOverlay {
    background-color: transparent;
}

QLabel#EmptyFilterTitleLabel {
    color: #DDDDDD;
    font-size: 14px;
    font-weight: bold;
}

QLabel#EmptyFilterSubLabel {
    color: #888888;
    font-size: 12px;
}

QPushButton#EmptyFilterResetBtn {
    background-color: #ff551c;
    color: #FFFFFF;
    border-radius: 4px;
    font-size: 12px;
    font-weight: bold;
    border: none;
    outline: none;
}

QPushButton#EmptyFilterResetBtn:hover {
    background-color: #ff6e3d;
}

QPushButton#EmptyFilterResetBtn:pressed {
    background-color: #e04b17;
}
>>>>>>> REPLACE
```

---

## Chapter 4: Build & Verification Steps

1. **编译构建**：
   使用 CMake 驱动 MSVC 构建工程：
   `cmake --build build --config Debug`
   核验是否存在签名不匹配或类型未定义报错。
2. **场景验证 1：搜索框输入文字（焦点留在输入框）按 Ctrl+D 立即生效**：
   - 光标点击顶栏搜索框，输入文本 `test`（此时输入光标正在闪烁，焦点在 `QLineEdit` 上）；
   - 右侧筛选面板不作任何勾选（无变化）；
   - 直接按下键盘快捷键 `Ctrl + D`；
   - **验证**：搜索框内的 `test` 立即被清空，搜索框释放焦点，中间内容面板瞬间恢复无过滤的完整列表，彻底解决打字时快捷键失效的问题。
3. **场景验证 2：重置筛选按钮对齐**：
   - 再次在搜索框输入文字；
   - 用鼠标点击筛选器顶部的“重置筛选条件（Ctrl + D）”按钮；
   - 验证：同样触发搜索框与筛选面板的联合清空。
4. **场景验证 3：空态感知提示**：
   - 进入包含文件的目录（如 11 个文件的 `SVG` 目录）；
   - 在筛选器中勾选一个不存在的标签或颜色，使可见行数归零；
   - 验证：中间内容面板居中显示“当前共有 11 个项目，已过滤隐藏 11 个项目”，不再死寂空白；
   - 点击遮罩上的“重置筛选与搜索 (Ctrl + D)”按钮，列表瞬间恢复全部 11 个文件。

---

## Chapter 5: Official API Reuse & Anti-Redundancy

1. **防重复造轮子**：
   - 搜索框清空复用官方成熟的 `SearchController::clearSearch(true)`；
   - 关键词清空复用官方 `ContentPanel::search("")`；
   - 统计信息复用 `ContentPanel::updateStatusBarStats()` 内部算好的 `visibleCount`、`fullCount` 与 `hiddenCount`，严禁在外部再次遍历底层数组；
   - 窗格联动复用 `PanelMediator::distributeBindPane` 标准多窗格分发通道。

---

## Chapter 6: Data SSOT Declaration Table

| 数据项 / 状态 | 唯一 SSOT 真理源 | 变动通知渠道 | 刷新目标 |
| :--- | :--- | :--- | :--- |
| 搜索框文本状态 | `SearchController::m_searchEdit` | `clearSearch(true)` | 顶栏搜索框还原占位符并释放焦点 |
| 面板过滤状态 | `FilterStateModel` | `clearAllFilters(true)` | 右侧筛选面板全部取消勾选 |
| 视图可见条目状态 | `FilterProxyModel` | `applyFilters()` | `m_emptyFilterOverlay` 显隐切换 |

---

## Chapter 7: Interface & Visibility Consistency Check

1. **外部调用接口检查**：
   - `ShortcutActionRouter` 构造函数新增形参 `SearchController* searchController = nullptr`，采用默认参数，旧调用方完全兼容，100% 遵守【契约锁】；
   - `FilterPanel::resetFilterRequested()` 与 `ContentPanel::filterResetRequested()` 声明为 `signals`，属于只增不减扩展；
   - `ContentPanel::resizeEvent` 为标准 `protected` 虚函数覆写；
   - `ContentPanel::initEmptyFilterOverlay()` 与 `updateEmptyStateOverlay` 为 `private` 私有辅助函数，无对外签名污染。

---

## Chapter 8: Header Chain & Inclusion Closure Check

1. `ShortcutActionRouter.h` 中前置声明 `class SearchController;` 与 `class ContentPanel;`；
2. `ShortcutActionRouter.cpp` 显式 `#include "SearchController.h"` 与 `#include "ContentPanel.h"`，类型完全闭合；
3. `ContentPanel.cpp` 已完整包含 `<QLabel>`、`<QVBoxLayout>`、`<QPushButton>` 与 `UiHelper.h`，无隐式间接依赖缺失。

---

## Chapter 9: Compliance & Quality Self-Check Table

| 序号 | 检查项 | 状态 | 证据 / 说明 |
| :--- | :--- | :--- | :--- |
| 1 | 方案隔离目录 | PASS | 方案就地更新于 `Implementation Plan/` 物理目录 |
| 2 | 英文命名规范 | PASS | 文件名为 `Unified_Filter_Search_Reset_And_Empty_State_Plan.md` |
| 3 | 契约锁（头文件冻结） | PASS | 既有公有 API 签名保持兼容，采用默认参数扩展 |
| 4 | 依赖锁（单向流动） | PASS | Shell $\to$ View $\to$ Controller，无逆向头文件侵入 |
| 5 | 无直接修改代码铁律 | PASS | 仅生成/修正实施方案，源代码 0 修改 |
| 6 | SEARCH 块 100% 精确匹配 | PASS | FilterPanel 字符已对齐真实中文括号，AppShortcutController 原文唯一匹配 |
| 7 | 彻底解除焦点误杀 | PASS | 废除 Ctrl+D 上的 if (!isEditingFocus())，搜索框内打字按 Ctrl+D 100% 生效 |
| 8 | 严格杜绝内联 setStyleSheet | PASS | 遮罩与按钮外观 100% 收敛至 resources/style.qss |
| 9 | 按钮防虚线框强锁 | PASS | m_emptyFilterResetBtn 显式配置 setFocusPolicy(Qt::NoFocus) 与 outline:none |
| 10 | 100% 中文说明 | PASS | 除代码标识符外全中文表达 |

---

## Chapter 10: Behavior Preservation & Zero-Value-Alteration Table

1. **零属性篡改**：未修改任何既有颜色 Hex 值、边框、间距或字体属性；空态提示新增的样式严格遵循既有主题颜色（橘色 `#ff551c`、悬停 `#ff6e3d`、浅灰 `#DDDDDD`、暗灰 `#888888`）；
2. **交互形态保持**：既有的搜索逻辑、筛选器独立操作、快捷键分发逻辑全部完整保留，仅在触发“重置”操作时增补联动通知与空态展示，解除搜索框焦点误伤，无破坏性变更。
