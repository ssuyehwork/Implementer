# 侧边栏“历史”与“收藏夹”对标列表视图微型卡片缩略图渲染实施方案 (修正版)

**文档语言**：中文（接口定义与 Git Merge Diff 标记除外）
**基准 Commit**：`d13c068`
**关联规范**：`Memories_QuarkMeta.md` 第 46、47 章节、`Standard Specifications for Implementation Plan.md`

---

## 物理阅读证据清单 (Gate 1: Pre-Draft Physical Evidence)

1. **涉及修改与阅读的物理文件列表**：
   - `QuarkMeta_Source/src/ui/delegates/FavoriteItemDelegate.h`（已物理读取第 1~30 行，确认委托声明与继承）
   - `QuarkMeta_Source/src/ui/delegates/FavoriteItemDelegate.cpp`（已物理读取第 50~100 行，确认旧实现圆角裁剪使用 `iconRect` 与 `Qt::KeepAspectRatioByExpanding` 冲刷边框的根因）
   - `QuarkMeta_Source/src/ui/HistoryPanel.h`（已物理读取第 1~50 行，确认成员变量与接口）
   - `QuarkMeta_Source/src/ui/HistoryPanel.cpp`（已物理读取第 1~120 行与 `eventFilter`，确认 `ColorPaletteEngine::isGraphicsFile(path)` 传参错误、缺少委托、双击与空格/回车未拦截）
   - `QuarkMeta_Source/src/ui/ShellIconManager.h`（已物理读取第 21 行，确认 `static inline QIcon getFileIcon(const QString& filePath, int size = 18)`）
   - `QuarkMeta_Source/src/ui/QuickLookWindow.h`（已物理读取第 26 行，确认 `static QuickLookWindow& instance();` 与 `void preview(const QString& filePath);`）
   - `QuarkMeta_Source/src/core/NavigationService.h`（已物理读取第 28 行与第 35 行，确认 `navigateTo` 与 `locateItem`）
   - `QuarkMeta_Source/src/util/ColorPaletteEngine.h`（已物理读取第 19 行，确认 `static bool isGraphicsFile(const QString& ext);`）
   - `QuarkMeta_Source/src/util/ThumbnailPipelineService.h`（已物理读取第 54 行，确认 `QThreadPool* decodePool();`）
   - `QuarkMeta_Source/src/util/DiskMediaExtractor.h`（已物理读取第 47 行，确认 `static QImage getCapsuleThumbnail(...)`）
   - `QuarkMeta_Source/CMakeLists.txt`（已物理读取第 325~335 行，确认委托注册位置）

2. **外部成员函数原样签名核验**：
   - `ColorPaletteEngine::isGraphicsFile(const QString& ext)`：`QuarkMeta_Source/src/util/ColorPaletteEngine.h:19`
   - `ShellIconManager::getFileIcon(const QString& filePath, int size = 18)`：`QuarkMeta_Source/src/ui/ShellIconManager.h:21`
   - `QuickLookWindow::instance().preview(const QString& filePath)`：`QuarkMeta_Source/src/ui/QuickLookWindow.h:26`
   - `NavigationService::instance().locateItem(const QString& path)`：`QuarkMeta_Source/src/core/NavigationService.h:35`
   - `NavigationService::instance().navigateTo(const QString& path, bool recordHistory, bool recursive)`：`QuarkMeta_Source/src/core/NavigationService.h:28`
   - `ThumbnailPipelineService::instance().decodePool()`：`QuarkMeta_Source/src/util/ThumbnailPipelineService.h:54`
   - `DiskMediaExtractor::getCapsuleThumbnail(const QString& filePath, int size, ...)`：`QuarkMeta_Source/src/util/DiskMediaExtractor.h:47`
   - `UiHelper::canPreviewFile(const QString& path)`：`QuarkMeta_Source/src/ui/UiHelper.h:166`

---

## Chapter 1: Overview（概述与根因分析）

### 1.1 缺陷现象与根因定位
1. **缺陷一：侧边栏“历史”选项卡无缩略图**
   - **根因 A**：`HistoryPanel::createHistoryItem` 中判定图形文件时，代码错误地写成了 `ColorPaletteEngine::isGraphicsFile(path)`。`isGraphicsFile` 的入参契约是文件扩展名后缀（如 `"eps"`、`"png"`），直接传入完整路径导致恒定返回 `false`，提取缩略图的代码块根本无法被执行。
   - **根因 B**：`HistoryPanel` 直接使用原生 `QTreeWidget` 且未安装自定义 ItemDelegate，原生树控件的默认 `iconSize (16, 16)` 缺乏圆角抗锯齿微卡片裁剪管线。
2. **缺陷二：侧边栏“收藏夹”选项卡缩略图卡片圆角失真（“左上角圆角、其余三角视觉为直角”）**
   - **根因**：在 `FavoriteItemDelegate::paint` 中直接使用 `iconRect` 裁剪并使用了 `Qt::KeepAspectRatioByExpanding` 冲刷边界，未将圆角路径精准施加在居中计算出的真实目标矩形 `targetRect` 上。
3. **缺陷三：侧边栏“历史”选项卡缺乏模式 A 联动激活（双击与空格/回车预览与定位）**
   - **根因**：`HistoryPanel` 未连接 `itemDoubleClicked`，也未在 `eventFilter` 中拦截空格/回车键。

### 1.2 重构设计目标
1. **统一对齐列表视图标准**：
   - 修复 `FavoriteItemDelegate`：统一采用 `Qt::KeepAspectRatio` 计算真实目标矩形 `targetRect`，并在 `targetRect` 上应用 4px 平滑圆角裁剪；
   - 新建规范独立的 `HistoryItemDelegate`（位于 `QuarkMeta_Source/src/ui/delegates/`），统一行高 28px、图标尺寸 18px、4px 圆角裁剪，遵循现有架构解耦；并在 `CMakeLists.txt` 中完成注册。
2. **修复历史缩略图提取与生命周期安全**：
   - 在 `HistoryPanel::createHistoryItem` 中正确传入 `fi.suffix()`；
   - 优化 `updateItemThumbnail`：跨线程调度只传递路径字符串与 Pixmap，在 UI 主线程通过 `HistoryPanel::updateItemThumbnail(path, pix)` 遍历匹配有效节点，彻底杜绝悬垂指针（Use-After-Free）。
3. **实现模式 A 即刻全功能预览与无缝目录定位**：
   - 双击或空格/回车时：若为可预览文件，立即弹出 `QuickLookWindow::instance().preview(path)` 并异步调用 `NavigationService::instance().locateItem(path)`；文件夹则调用 `navigateTo(path)`。

---

## Chapter 2: Modified Files List（影响文件清单）

1. `QuarkMeta_Source/src/ui/delegates/FavoriteItemDelegate.cpp`（修复缩略图居中与四角对称平滑裁剪算法）
2. `QuarkMeta_Source/src/ui/delegates/HistoryItemDelegate.h`（新建：独立历史微卡片渲染委托头文件）
3. `QuarkMeta_Source/src/ui/delegates/HistoryItemDelegate.cpp`（新建：独立历史微卡片渲染委托实现）
4. `QuarkMeta_Source/CMakeLists.txt`（注册 `HistoryItemDelegate.h` 与 `HistoryItemDelegate.cpp`）
5. `QuarkMeta_Source/src/ui/HistoryPanel.h`（引入 `HistoryItemDelegate` 声明，新增 `activateCurrentItem()`，重载安全 `updateItemThumbnail(path, pix)`）
6. `QuarkMeta_Source/src/ui/HistoryPanel.cpp`（装配委托，修复判图扩展名，实现主线程安全回填与模式 A 快捷操作）
7. `QuarkMeta_Source/Memories_QuarkMeta.md`（记录设计规范与历史记忆）

---

## Chapter 3: Detailed Line-by-Line Changes（精准替换块）

### 3.1 修复 `FavoriteItemDelegate.cpp` 中的缩略图圆角裁剪

```
<<<<<<< SEARCH
    // 微卡片圆角绘制
    QVariant decoData = index.data(Qt::DecorationRole);
    bool isFolder = index.data(Qt::UserRole + 4).toBool();
    bool hasCustomThumb = index.data(Qt::UserRole + 5).toBool();

    if (!isFolder && hasCustomThumb && decoData.canConvert<QIcon>()) {
        QIcon icon = decoData.value<QIcon>();
        QPixmap pix = icon.pixmap(QSize(64, 64));

        if (!pix.isNull()) {
            QPainterPath clipPath;
            clipPath.addRoundedRect(iconRect, 3, 3);
            painter->save();
            painter->setClipPath(clipPath);

            QPixmap scaled = pix.scaled(iconRect.size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            int x = iconRect.center().x() - scaled.width() / 2;
            int y = iconRect.center().y() - scaled.height() / 2;
            painter->drawPixmap(x, y, scaled);
            painter->restore();
        } else {
            icon.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
        }
    } else {
        QIcon icon = decoData.value<QIcon>();
        if (!icon.isNull()) {
            icon.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
        }
    }
=======
    // 微卡片圆角绘制（严格对标列表视图 TreeItemDelegate 标准：Qt::KeepAspectRatio + targetRect 真实四角对称圆角裁剪）
    QVariant decoData = index.data(Qt::DecorationRole);
    bool isFolder = index.data(Qt::UserRole + 4).toBool();
    bool hasCustomThumb = index.data(Qt::UserRole + 5).toBool();

    if (!isFolder && hasCustomThumb && decoData.canConvert<QIcon>()) {
        QIcon icon = decoData.value<QIcon>();
        QPixmap pix = icon.pixmap(QSize(64, 64));

        if (!pix.isNull()) {
            painter->save();
            // 1. 等比缩放计算目标几何矩形 targetRect
            QPixmap scaled = pix.scaled(iconRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            int x = iconRect.center().x() - scaled.width() / 2;
            int y = iconRect.center().y() - scaled.height() / 2;
            QRect targetRect(x, y, scaled.width(), scaled.height());

            // 2. 将圆角路径精准施加在 targetRect 上，彻底杜绝外溢冲刷与三角直角现象
            QPainterPath clipPath;
            clipPath.addRoundedRect(targetRect, 4, 4);
            painter->setClipPath(clipPath);

            painter->drawPixmap(targetRect, scaled);
            painter->restore();
        } else {
            icon.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
        }
    } else {
        QIcon icon = decoData.value<QIcon>();
        if (!icon.isNull()) {
            icon.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::Off);
        }
    }
>>>>>>> REPLACE
```

---

### 3.2 在 `CMakeLists.txt` 中注册 `HistoryItemDelegate`

```
<<<<<<< SEARCH
    src/ui/delegates/FavoriteItemDelegate.h
    src/ui/delegates/FavoriteItemDelegate.cpp
    src/ui/controllers/FavoriteTreeDropHandler.h
=======
    src/ui/delegates/FavoriteItemDelegate.h
    src/ui/delegates/FavoriteItemDelegate.cpp
    src/ui/delegates/HistoryItemDelegate.h
    src/ui/delegates/HistoryItemDelegate.cpp
    src/ui/controllers/FavoriteTreeDropHandler.h
>>>>>>> REPLACE
```

---

### 3.3 更新 `HistoryPanel.h`

```
<<<<<<< SEARCH
#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QMenu>
#include <QIcon>
#include "FavoritePanel.h"

namespace QuarkMeta {

class HistoryPanel : public QWidget {
    Q_OBJECT

public:
    explicit HistoryPanel(QWidget* parent = nullptr);
    ~HistoryPanel() override = default;

    void loadHistory();
    void filterTreeNodes(const QString& keyword);

signals:
    void requestLocateFile(const QString& path);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onItemClicked(QTreeWidgetItem* item, int column);
    void onContextMenuRequested(const QPoint& pos);
    void onHistoryItemAdded(const QString& path);
    void onHistoryItemRemoved(const QString& path);
    void onHistoryCleared();

private:
    void initUi();
    QTreeWidgetItem* createHistoryItem(const QString& path);
    void updateItemThumbnail(QTreeWidgetItem* item, const QString& path);

    QVBoxLayout* m_mainLayout = nullptr;
    QTreeWidget* m_treeWidget = nullptr;
    bool m_isLoaded = false;
    QString m_lastHoveredPath;
};

} // namespace QuarkMeta
=======
#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QMenu>
#include <QIcon>
#include "FavoritePanel.h"

namespace QuarkMeta {

class HistoryItemDelegate;

class HistoryPanel : public QWidget {
    Q_OBJECT

public:
    explicit HistoryPanel(QWidget* parent = nullptr);
    ~HistoryPanel() override = default;

    void loadHistory();
    void filterTreeNodes(const QString& keyword);
    void activateCurrentItem();

signals:
    void requestLocateFile(const QString& path);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onItemClicked(QTreeWidgetItem* item, int column);
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onContextMenuRequested(const QPoint& pos);
    void onHistoryItemAdded(const QString& path);
    void onHistoryItemRemoved(const QString& path);
    void onHistoryCleared();

private:
    void initUi();
    QTreeWidgetItem* createHistoryItem(const QString& path);
    void requestThumbnailExtraction(const QString& path);
    void updateItemThumbnail(const QString& path, const QPixmap& pix);

    QVBoxLayout* m_mainLayout = nullptr;
    QTreeWidget* m_treeWidget = nullptr;
    HistoryItemDelegate* m_delegate = nullptr;
    bool m_isLoaded = false;
    QString m_lastHoveredPath;
};

} // namespace QuarkMeta
>>>>>>> REPLACE
```

---

### 3.4 更新 `HistoryPanel.cpp`

```
<<<<<<< SEARCH
#include "HistoryPanel.h"
#include "UiHelper.h"
#include "ToolTipOverlay.h"
#include "controllers/ContextMenuFactory.h"
#include "../core/NavigationService.h"
#include "../core/ClipboardService.h"
#include "../meta/FavoriteService.h"
#include "../meta/HistoryService.h"
#include "../meta/LibraryService.h"
#include "../util/ThumbnailPipelineService.h"
#include "../util/ColorPaletteEngine.h"
#include "../util/ShellHelper.h"
#include "dialogs/FramelessConfirmDialog.h"
#include <QDesktopServices>
#include <QFileInfo>
#include <QDir>
#include <QFileIconProvider>
#include <QDrag>
#include <QMimeData>
#include <QUrl>
#include <QMouseEvent>
#include <QHeaderView>

namespace QuarkMeta {

HistoryPanel::HistoryPanel(QWidget* parent) : QWidget(parent) {
    initUi();

    connect(&HistoryService::instance(), &HistoryService::historyItemAdded, this, &HistoryPanel::onHistoryItemAdded);
    connect(&HistoryService::instance(), &HistoryService::historyItemRemoved, this, &HistoryPanel::onHistoryItemRemoved);
    connect(&HistoryService::instance(), &HistoryService::historyCleared, this, &HistoryPanel::onHistoryCleared);
}

void HistoryPanel::initUi() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setObjectName("HistoryTreeWidget");
    m_treeWidget->setHeaderHidden(true);
    m_treeWidget->setIndentation(0);
    m_treeWidget->setIconSize(QSize(16, 16));
    m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_treeWidget->setDragEnabled(true);
    m_treeWidget->setMouseTracking(true);

    m_mainLayout->addWidget(m_treeWidget);

    connect(m_treeWidget, &QTreeWidget::itemClicked, this, &HistoryPanel::onItemClicked);
    connect(m_treeWidget, &QTreeWidget::customContextMenuRequested, this, &HistoryPanel::onContextMenuRequested);

    if (m_treeWidget->viewport()) {
        m_treeWidget->viewport()->installEventFilter(this);
    }
}

void HistoryPanel::loadHistory() {
    if (m_isLoaded) return;
    m_isLoaded = true;

    m_treeWidget->clear();
    auto list = HistoryService::instance().getAllHistory();
    for (const auto& rec : list) {
        if (QFile::exists(rec.filePath)) {
            QTreeWidgetItem* item = createHistoryItem(rec.filePath);
            m_treeWidget->addTopLevelItem(item);
        }
    }
}

QTreeWidgetItem* HistoryPanel::createHistoryItem(const QString& path) {
    QFileInfo fi(path);
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, fi.fileName());
    item->setData(0, Qt::UserRole, path);

    // 普通文件优先展示系统/通用图标
    QFileIconProvider provider;
    QIcon sysIcon = provider.icon(fi);
    if (!sysIcon.isNull()) {
        item->setIcon(0, sysIcon);
    } else {
        item->setIcon(0, UiHelper::getIcon("file", "#888888", 16));
    }

    // 图形图片类文件异步提取缩略图回填
    if (ColorPaletteEngine::isGraphicsFile(path)) {
        updateItemThumbnail(item, path);
    }

    return item;
}

void HistoryPanel::updateItemThumbnail(QTreeWidgetItem* item, const QString& path) {
    ThumbnailPipelineService::instance().loadBatchAsync({path}, 32, [this, item](const QString&, const QPixmap& pix) {
        if (!pix.isNull() && item && item->treeWidget()) {
            item->setIcon(0, QIcon(pix));
        }
    });
}

void HistoryPanel::onItemClicked(QTreeWidgetItem* item, int column) {
    Q_UNUSED(item);
    Q_UNUSED(column);
    // 单击仅由 QTreeWidget 处理选中高亮，不发射定位信号以彻底杜绝额外目录载入与缩略图解析开销
}
=======
#include "HistoryPanel.h"
#include "UiHelper.h"
#include "ShellIconManager.h"
#include "QuickLookWindow.h"
#include "ToolTipOverlay.h"
#include "delegates/HistoryItemDelegate.h"
#include "controllers/ContextMenuFactory.h"
#include "../core/NavigationService.h"
#include "../core/ClipboardService.h"
#include "../meta/FavoriteService.h"
#include "../meta/HistoryService.h"
#include "../meta/LibraryService.h"
#include "../util/ThumbnailPipelineService.h"
#include "../util/DiskMediaExtractor.h"
#include "../util/ColorPaletteEngine.h"
#include "../util/ShellHelper.h"
#include "dialogs/FramelessConfirmDialog.h"
#include <QDesktopServices>
#include <QFileInfo>
#include <QDir>
#include <QDrag>
#include <QMimeData>
#include <QUrl>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QHeaderView>
#include <QPointer>
#include <QCoreApplication>
#include <QtConcurrent>

namespace QuarkMeta {

HistoryPanel::HistoryPanel(QWidget* parent) : QWidget(parent) {
    initUi();

    connect(&HistoryService::instance(), &HistoryService::historyItemAdded, this, &HistoryPanel::onHistoryItemAdded);
    connect(&HistoryService::instance(), &HistoryService::historyItemRemoved, this, &HistoryPanel::onHistoryItemRemoved);
    connect(&HistoryService::instance(), &HistoryService::historyCleared, this, &HistoryPanel::onHistoryCleared);
}

void HistoryPanel::initUi() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setObjectName("HistoryTreeWidget");
    m_treeWidget->setHeaderHidden(true);
    m_treeWidget->setIndentation(0);
    m_treeWidget->setIconSize(QSize(18, 18));
    m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_treeWidget->setDragEnabled(true);
    m_treeWidget->setMouseTracking(true);

    m_delegate = new HistoryItemDelegate(this);
    m_treeWidget->setItemDelegate(m_delegate);

    m_mainLayout->addWidget(m_treeWidget);

    connect(m_treeWidget, &QTreeWidget::itemClicked, this, &HistoryPanel::onItemClicked);
    connect(m_treeWidget, &QTreeWidget::itemDoubleClicked, this, &HistoryPanel::onItemDoubleClicked);
    connect(m_treeWidget, &QTreeWidget::customContextMenuRequested, this, &HistoryPanel::onContextMenuRequested);

    m_treeWidget->installEventFilter(this);
    if (m_treeWidget->viewport()) {
        m_treeWidget->viewport()->installEventFilter(this);
    }
}

void HistoryPanel::loadHistory() {
    if (m_isLoaded) return;
    m_isLoaded = true;

    m_treeWidget->clear();
    auto list = HistoryService::instance().getAllHistory();
    for (const auto& rec : list) {
        if (QFile::exists(rec.filePath)) {
            QTreeWidgetItem* item = createHistoryItem(rec.filePath);
            m_treeWidget->addTopLevelItem(item);
        }
    }
}

QTreeWidgetItem* HistoryPanel::createHistoryItem(const QString& path) {
    QFileInfo fi(path);
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, fi.fileName());
    item->setData(0, Qt::UserRole, path);
    item->setData(0, Qt::UserRole + 1, path);
    item->setData(0, Qt::UserRole + 5, false);

    // 普通文件优先展示 Shell 系统/通用图标
    QIcon sysIcon = ShellIconManager::getFileIcon(path, 18);
    if (!sysIcon.isNull()) {
        item->setIcon(0, sysIcon);
    } else {
        item->setIcon(0, UiHelper::getIcon("file", "#888888", 18));
    }

    // 图形图片类文件异步提取缩略图回填（传入纯后缀名）
    QString ext = fi.suffix().toLower();
    if (ColorPaletteEngine::isGraphicsFile(ext) || ext == "pdf") {
        requestThumbnailExtraction(path);
    }

    return item;
}

void HistoryPanel::requestThumbnailExtraction(const QString& path) {
    QPointer<HistoryPanel> weakThis(this);
    (void)QtConcurrent::run(ThumbnailPipelineService::instance().decodePool(), [weakThis, path]() {
        if (!weakThis) return;
        QImage img = DiskMediaExtractor::getCapsuleThumbnail(path, DiskMediaExtractor::kThumbSize);
        if (!img.isNull()) {
            if (img.width() > 128 || img.height() > 128) {
                img = img.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
            QPixmap pix = QPixmap::fromImage(img);
            QMetaObject::invokeMethod(QCoreApplication::instance(), [weakThis, path, pix]() {
                if (weakThis) {
                    weakThis->updateItemThumbnail(path, pix);
                }
            }, Qt::QueuedConnection);
        }
    });
}

void HistoryPanel::updateItemThumbnail(const QString& path, const QPixmap& pix) {
    if (pix.isNull() || !m_treeWidget) return;

    QString cleanTarget = QDir::toNativeSeparators(QDir::cleanPath(path));
    int count = m_treeWidget->topLevelItemCount();
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem* itm = m_treeWidget->topLevelItem(i);
        if (!itm) continue;

        QString itemPath = QDir::toNativeSeparators(QDir::cleanPath(itm->data(0, Qt::UserRole).toString()));
        if (QString::compare(itemPath, cleanTarget, Qt::CaseInsensitive) == 0) {
            itm->setIcon(0, QIcon(pix));
            itm->setData(0, Qt::UserRole + 5, true);
        }
    }

    if (m_treeWidget->viewport()) {
        m_treeWidget->viewport()->update();
    }
}

void HistoryPanel::onItemClicked(QTreeWidgetItem* item, int column) {
    Q_UNUSED(item);
    Q_UNUSED(column);
    // 单击仅由 QTreeWidget 处理选中高亮，不发射定位信号以彻底杜绝额外目录载入与缩略图解析开销
}

void HistoryPanel::onItemDoubleClicked(QTreeWidgetItem* item, int column) {
    Q_UNUSED(column);
    if (!item) return;
    activateCurrentItem();
}

void HistoryPanel::activateCurrentItem() {
    QTreeWidgetItem* curItem = m_treeWidget ? m_treeWidget->currentItem() : nullptr;
    if (!curItem) return;

    QString path = curItem->data(0, Qt::UserRole).toString();
    if (path.isEmpty() || !QFile::exists(path)) return;

    QFileInfo fi(path);
    if (fi.isDir()) {
        NavigationService::instance().navigateTo(path, true, false);
    } else {
        // 模式 A 契约：即刻零延迟弹出 QuickLook 预览，同时异步联动内容面板精准定位
        if (UiHelper::canPreviewFile(path)) {
            QuickLookWindow::instance().preview(path);
        }
        NavigationService::instance().locateItem(path);
    }
}
>>>>>>> REPLACE
```

---

```
<<<<<<< SEARCH
void HistoryPanel::filterTreeNodes(const QString& keyword) {
    QString kw = keyword.trimmed().toLower();
    for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = m_treeWidget->topLevelItem(i);
        if (kw.isEmpty()) {
            item->setHidden(false);
        } else {
            bool match = item->text(0).toLower().contains(kw) || item->data(0, Qt::UserRole).toString().toLower().contains(kw);
            item->setHidden(!match);
        }
    }
}

bool HistoryPanel::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_treeWidget->viewport()) {
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
=======
void HistoryPanel::filterTreeNodes(const QString& keyword) {
    if (m_delegate) {
        m_delegate->setFilterKeyword(keyword.trimmed());
    }

    QString kw = keyword.trimmed().toLower();
    for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = m_treeWidget->topLevelItem(i);
        if (kw.isEmpty()) {
            item->setHidden(false);
        } else {
            bool match = item->text(0).toLower().contains(kw) || item->data(0, Qt::UserRole).toString().toLower().contains(kw);
            item->setHidden(!match);
        }
    }

    if (m_treeWidget->viewport()) {
        m_treeWidget->viewport()->update();
    }
}

bool HistoryPanel::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_treeWidget) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            // 模式 A 契约：未带 Ctrl 的空格键或回车键直接触发激活
            if ((keyEvent->key() == Qt::Key_Space && !(keyEvent->modifiers() & Qt::ControlModifier)) ||
                keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                activateCurrentItem();
                return true;
            }
        }
    }

    if (watched == m_treeWidget->viewport()) {
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
>>>>>>> REPLACE
```

---

## Chapter 4: Build & Verification Steps（构建与运行时验收标准）

### 4.1 编译验证
确认新增的 `HistoryItemDelegate` 已注册于 `CMakeLists.txt`，头文件包含完整闭合。

### 4.2 运行时验收标准
1. **历史缩略图正常显现**：历史列表中所有图片及图档均能异步并发解码回填彩色微缩略图；
2. **四角圆角对称**：收藏夹与历史面板微卡片均为标准 4px 均匀圆角裁剪；
3. **模式 A 联动**：双击或按下空格/回车，即刻唤起 QuickLook 预览并联动主内容区精准定位；
4. **生命周期绝对安全**：历史记录清空或移除时，后台线程池任务回到 UI 线程不访问悬垂裸指针。
