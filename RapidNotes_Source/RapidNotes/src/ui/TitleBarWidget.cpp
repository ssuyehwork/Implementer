#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "TitleBarWidget.h"
#include "TabBarWidget.h"
#include "UiHelper.h"
#include "HoverEventFilter.h"
#include "SvgIconRenderer.h"
#include "StyleLibrary.h"

#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QSignalBlocker>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace QuarkMeta {

constexpr int kLayoutEdgeMargin = 5;

TitleBarWidget::TitleBarWidget(QWidget* parent, HoverEventFilter* hoverFilter)
    : QWidget(parent) {
    setObjectName("TitleBar");
    setAttribute(Qt::WA_StyledBackground, true);
    setAcceptDrops(true);
    setFixedHeight(34);
    initUi(hoverFilter);
}

void TitleBarWidget::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData() && event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        QWidget::dragEnterEvent(event);
    }
}

void TitleBarWidget::dropEvent(QDropEvent* event) {
    if (event->mimeData() && event->mimeData()->hasUrls()) {
        for (const QUrl& url : event->mimeData()->urls()) {
            QString path = url.toLocalFile();
            if (!path.isEmpty() && QFileInfo(path).isDir()) {
                if (m_tabBar) {
                    m_tabBar->openOrFocusTab(path);
                }
                event->acceptProposedAction();
                return;
            }
        }
    }
    QWidget::dropEvent(event);
}

bool TitleBarWidget::isPinned() const {
    return m_btnPinTop ? m_btnPinTop->isChecked() : false;
}

void TitleBarWidget::setPinned(bool pinned) {
    if (!m_btnPinTop) return;
    QSignalBlocker blocker(m_btnPinTop);
    m_btnPinTop->setChecked(pinned);
    m_btnPinTop->setIcon(UiHelper::getIcon(pinned ? "pin_vertical" : "pin_tilted", pinned ? Style::ActiveOrange : Style::TextMain));
}

void TitleBarWidget::setZoomLevel(int value) {
    if (!m_sizeSlider) return;
    QSignalBlocker blocker(m_sizeSlider);
    m_sizeSlider->setValue(qBound(m_sizeSlider->minimum(), value, m_sizeSlider->maximum()));
}

void TitleBarWidget::setWindowMaximized(bool maximized) {
    if (!m_btnMax) return;
    QString iconKey = maximized ? "restore_line" : "maximize";
    m_btnMax->setIcon(UiHelper::getIcon(iconKey, QColor("#EEEEEE")));
    m_btnMax->setProperty("tooltipText", maximized ? "还原" : "最大化");
}

void TitleBarWidget::setViewModeOption(ViewModeOption mode) {
    m_currentViewMode = mode;
}

void TitleBarWidget::setDriveBarVisible(bool visible) {
    if (!m_btnToggleDriveBar) return;
    QSignalBlocker blocker(m_btnToggleDriveBar);
    m_btnToggleDriveBar->setChecked(visible);
    m_btnToggleDriveBar->setIcon(UiHelper::getIcon(visible ? "chevrons_down" : "chevrons_up", QColor("#EEEEEE")));
}

void TitleBarWidget::initUi(HoverEventFilter* hoverFilter) {
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(5, 0, kLayoutEdgeMargin, 0);
    m_layout->setSpacing(8);

    m_logoLabel = new QLabel(this);
    m_logoLabel->setFixedSize(18, 18);
    m_logoLabel->setPixmap(UiHelper::getIcon("quarkmeta", Style::BrandOrange).pixmap(16, 16));
    m_logoLabel->setAlignment(Qt::AlignCenter);
    m_logoLabel->setObjectName("TitleLogoLabel");
    m_layout->addWidget(m_logoLabel);

    m_tabBar = new TabBarWidget(this, hoverFilter);
    m_tabBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_layout->addWidget(m_tabBar, 1);

    m_rightToolContainer = new QWidget(this);
    m_rightToolContainer->setObjectName("TitleRightToolContainer");
    m_rightToolContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    QHBoxLayout* rightLayout = new QHBoxLayout(m_rightToolContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(8);

    auto createTitleBtn = [this, hoverFilter](const QString& iconKey, const QString& tip) -> QPushButton* {
        QPushButton* btn = new QPushButton(this);
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setAttribute(Qt::WA_Hover);
        btn->setFixedSize(24, 24);
        btn->setIcon(UiHelper::getIcon(iconKey, QColor("#EEEEEE")));
        btn->setIconSize(QSize(18, 18));
        btn->setObjectName("TitleControlBtn");
        btn->setProperty("tooltipText", tip);
        if (hoverFilter) {
            btn->installEventFilter(hoverFilter);
        }
        return btn;
    };

    m_btnViewMenu = createTitleBtn("write_1", "排列方式");
    setupViewMenu();

    m_sizeSlider = new QSlider(Qt::Horizontal, this);
    m_sizeSlider->setRange(30, 230);
    m_sizeSlider->setFixedSize(110, 20);
    m_sizeSlider->setCursor(Qt::PointingHandCursor);
    m_sizeSlider->setObjectName("SizeSlider");

    connect(m_sizeSlider, &QSlider::valueChanged, this, [this](int value) {
        emit zoomLevelChanged(value);
    });

    m_btnToggleDriveBar = createTitleBtn("chevrons_down", "展开/收起插件栏");
    m_btnToggleDriveBar->setCheckable(true);
    m_btnToggleDriveBar->setChecked(true);
    connect(m_btnToggleDriveBar, &QPushButton::toggled, this, [this](bool checked) {
        m_btnToggleDriveBar->setIcon(UiHelper::getIcon(checked ? "chevrons_down" : "chevrons_up", QColor("#EEEEEE")));
        emit driveBarToggleRequested(checked);
    });

    m_btnLayout = createTitleBtn("layout", "布局管理与重置");
    connect(m_btnLayout, &QPushButton::clicked, this, [this]() {
        emit layoutMenuRequested(m_btnLayout->mapToGlobal(QPoint(0, m_btnLayout->height())));
    });

    m_btnCreate = createTitleBtn("add", "新建...");
    setupCreateMenu();

    m_btnPinTop = createTitleBtn("pin_tilted", "置顶窗口");
    m_btnPinTop->setCheckable(true);
    m_btnPinTop->setChecked(false);

    connect(m_btnPinTop, &QPushButton::toggled, this, [this](bool checked) {
        m_btnPinTop->setIcon(UiHelper::getIcon(checked ? "pin_vertical" : "pin_tilted", checked ? Style::ActiveOrange : Style::TextMain));
        emit pinToggled(checked);
    });

    m_btnMin = createTitleBtn("minimize", "最小化");
    m_btnMax = createTitleBtn("maximize", "最大化/还原");
    m_btnClose = createTitleBtn("close", "关闭项目");
    m_btnClose->setObjectName("TitleCloseBtn");

    connect(m_btnMin, &QPushButton::clicked, this, [this]() {
        QWidget* topWin = window();
        if (topWin) {
#ifdef Q_OS_WIN
            ::SendMessage(reinterpret_cast<HWND>(topWin->winId()), WM_SYSCOMMAND, SC_MINIMIZE, 0);
#else
            topWin->showMinimized();
#endif
        }
    });

    // 关键修正 3：使用 Win32 消息派发还原与最大化，彻底解决还原失败
    connect(m_btnMax, &QPushButton::clicked, this, [this]() {
        QWidget* topWin = window();
        if (!topWin) return;
#ifdef Q_OS_WIN
        HWND hwnd = reinterpret_cast<HWND>(topWin->winId());
        if (::IsZoomed(hwnd)) {
            ::SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
        } else {
            ::SendMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        }
#else
        if (topWin->isMaximized()) topWin->showNormal();
        else topWin->showMaximized();
#endif
    });

    connect(m_btnClose, &QPushButton::clicked, this, [this]() {
        if (window()) window()->close();
    });

    rightLayout->addWidget(m_sizeSlider, 0, Qt::AlignVCenter);
    rightLayout->addWidget(m_btnViewMenu, 0, Qt::AlignVCenter);
    rightLayout->addWidget(m_btnToggleDriveBar, 0, Qt::AlignVCenter);
    rightLayout->addWidget(m_btnLayout, 0, Qt::AlignVCenter);
    rightLayout->addWidget(m_btnCreate, 0, Qt::AlignVCenter);
    rightLayout->addWidget(m_btnPinTop, 0, Qt::AlignVCenter);
    rightLayout->addWidget(m_btnMin, 0, Qt::AlignVCenter);
    rightLayout->addWidget(m_btnMax, 0, Qt::AlignVCenter);
    rightLayout->addWidget(m_btnClose, 0, Qt::AlignVCenter);

    m_layout->addWidget(m_rightToolContainer, 0, Qt::AlignRight | Qt::AlignVCenter);
}

void TitleBarWidget::setupViewMenu() {
    connect(m_btnViewMenu, &QPushButton::clicked, this, [this]() {
        QMenu menu(this);
        menu.setObjectName("TitleBarViewModeMenu");
        UiHelper::applyMenuStyle(&menu);

        QAction* actAdaptive = menu.addAction(UiHelper::getIcon("resize2", QColor("#EEEEEE"), 18), "自适应(A)");
        QAction* actGrid = menu.addAction(UiHelper::getIcon("gridgapm", QColor("#EEEEEE"), 18), "网格(G)");
        QAction* actList = menu.addAction(UiHelper::getIcon("list_ul", QColor("#EEEEEE"), 18), "列表(L)");
        QAction* actColumn = menu.addAction(UiHelper::getIcon("column_view", QColor("#EEEEEE"), 18), "分栏(C)");

        actAdaptive->setCheckable(true);
        actGrid->setCheckable(true);
        actList->setCheckable(true);
        actColumn->setCheckable(true);

        actAdaptive->setChecked(m_currentViewMode == JustifiedViewMode);
        actGrid->setChecked(m_currentViewMode == GridViewMode);
        actList->setChecked(m_currentViewMode == ListViewMode);
        actColumn->setChecked(m_currentViewMode == ColumnViewMode);

        QString checkPath = SvgIconRenderer::getSvgTempFilePath("check", QColor("#ff551c"));
        menu.setStyleSheet(menu.styleSheet() + QString(
            "QMenu#TitleBarViewModeMenu::indicator:checked { image: url(%1); }"
        ).arg(checkPath));

        connect(actAdaptive, &QAction::triggered, this, [this]() {
            m_currentViewMode = JustifiedViewMode;
            emit viewModeRequested(JustifiedViewMode);
        });
        connect(actGrid, &QAction::triggered, this, [this]() {
            m_currentViewMode = GridViewMode;
            emit viewModeRequested(GridViewMode);
        });
        connect(actList, &QAction::triggered, this, [this]() {
            m_currentViewMode = ListViewMode;
            emit viewModeRequested(ListViewMode);
        });
        connect(actColumn, &QAction::triggered, this, [this]() {
            m_currentViewMode = ColumnViewMode;
            emit viewModeRequested(ColumnViewMode);
        });

        menu.exec(m_btnViewMenu->mapToGlobal(QPoint(0, m_btnViewMenu->height())));
    });
}

void TitleBarWidget::setupCreateMenu() {
    QMenu* createMenu = UiHelper::createMenu(m_btnCreate);

    QAction* actNewFolder = createMenu->addAction(UiHelper::getIcon("folder_filled", QColor("#EEEEEE")), "创建文件夹");
    QAction* actNewMd     = createMenu->addAction(UiHelper::getIcon("text", QColor("#EEEEEE")), "创建 Markdown");
    QAction* actNewTxt    = createMenu->addAction(UiHelper::getIcon("text", QColor("#EEEEEE")), "创建纯文本文件 (txt)");

    connect(m_btnCreate, &QPushButton::clicked, this, [this, createMenu]() {
        createMenu->popup(m_btnCreate->mapToGlobal(QPoint(0, m_btnCreate->height())));
    });

    auto handleCreate = [this](const QString& type) {
        emit createItemRequested(type);
    };
    connect(actNewFolder, &QAction::triggered, this, [handleCreate](){ handleCreate("folder"); });
    connect(actNewMd,     &QAction::triggered, this, [handleCreate](){ handleCreate("md"); });
    connect(actNewTxt,    &QAction::triggered, this, [handleCreate](){ handleCreate("txt"); });
}

} // namespace QuarkMeta