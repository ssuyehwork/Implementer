#include "DriveBarWidget.h"
#include "UiHelper.h"
#include "TagManagerDialog.h"
#include "dialogs/TextExtensionDialog.h"
#include "dialogs/CacheLocationDialog.h"
#include "ToolTipOverlay.h"
#include "../core/NavigationService.h"
#include "../core/AppConfig.h"

#include <QMenu>
#include <QEvent>

namespace QuarkMeta {

DriveBarWidget::DriveBarWidget(QWidget* parent)
    : QWidget(parent) {
    setObjectName("DriveBar");
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedHeight(42);
    initUi();
}

QPushButton* DriveBarWidget::createIconButton(const QString& iconKey, const QColor& color, const QString& tooltipText) {
    QPushButton* btn = new QPushButton(UiHelper::getIcon(iconKey, color, 18), "", this);
    btn->setFixedSize(32, 32);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setObjectName("DriveBarIconButton");
    btn->setProperty("tooltipText", tooltipText);
    btn->setAttribute(Qt::WA_Hover);

    // 绑定 ToolTipOverlay 悬浮提示，纯图标零文字暴露
    btn->installEventFilter(this);
    return btn;
}

bool DriveBarWidget::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::ToolTip) {
        QWidget* w = qobject_cast<QWidget*>(watched);
        if (w) {
            QString tooltipText = w->property("tooltipText").toString();
            if (!tooltipText.isEmpty()) {
                ToolTipOverlay::instance()->showText(QCursor::pos(), tooltipText, 0);
                return true;
            }
        }
    } else if (event->type() == QEvent::Leave) {
        ToolTipOverlay::instance()->hideOverlay();
    }
    return QWidget::eventFilter(watched, event);
}

void DriveBarWidget::initUi() {
    m_driveBarLayout = new QHBoxLayout(this);
    m_driveBarLayout->setContentsMargins(15, 5, 15, 5);
    m_driveBarLayout->setSpacing(8);

    // 1. 最左侧（第一个按钮）：固定放置 extension 图标的扩展与组件管理器按钮
    m_btnExtensionManager = createIconButton("extension", QColor("#EEEEEE"), "扩展与组件管理");
    setupExtensionMenu();
    m_driveBarLayout->addWidget(m_btnExtensionManager);

    // 2. 创建各独立功能图标（纯 Icon，通过 ToolTipOverlay 提示）
    m_btnTagManager = createIconButton("tag", QColor("#1abc9c"), "标签管理");
    connect(m_btnTagManager, &QPushButton::clicked, this, [this]() {
        TagManagerDialog::showDialog(this, NavigationService::instance().currentUrl(), false);
    });

    m_btnTextExtension = createIconButton("Ext.svg", QColor("#e67e22"), "文本扩展名设置");
    connect(m_btnTextExtension, &QPushButton::clicked, this, [this]() {
        TextExtensionDialog dlg(this);
        dlg.exec();
    });

    m_btnSaveThumbnail = createIconButton("save-Thumbnail", QColor("#3498db"), "缩略图与缓存位置设置");
    connect(m_btnSaveThumbnail, &QPushButton::clicked, this, [this]() {
        CacheLocationDialog dlg(this);
        dlg.exec();
    });

    // 默认加至布局并依配置显隐
    m_driveBarLayout->addWidget(m_btnTagManager);
    m_driveBarLayout->addWidget(m_btnTextExtension);
    m_driveBarLayout->addWidget(m_btnSaveThumbnail);
    m_driveBarLayout->addStretch();

    refreshPinnedButtons();
}

void DriveBarWidget::refreshPinnedButtons() {
    QStringList pinnedList = AppConfig::instance().getValue("DriveBar/PinnedExtensions", QStringList{"tag", "text_ext", "save_thumbnail"}).toStringList();

    if (m_btnTagManager) {
        m_btnTagManager->setVisible(pinnedList.contains("tag"));
    }
    if (m_btnTextExtension) {
        m_btnTextExtension->setVisible(pinnedList.contains("text_ext"));
    }
    if (m_btnSaveThumbnail) {
        m_btnSaveThumbnail->setVisible(pinnedList.contains("save_thumbnail"));
    }
}

void DriveBarWidget::setupExtensionMenu() {
    QMenu* extMenu = UiHelper::createMenu(m_btnExtensionManager);
    extMenu->setObjectName("DriveBarExtensionMenu");

    connect(m_btnExtensionManager, &QPushButton::clicked, this, [this, extMenu]() {
        extMenu->clear();

        QStringList pinnedList = AppConfig::instance().getValue("DriveBar/PinnedExtensions", QStringList{"tag", "text_ext", "save_thumbnail"}).toStringList();

        // 支持持续选择：重写 action 交互避免触发后自动销毁关闭菜单
        QAction* actTag = extMenu->addAction(UiHelper::getIcon("tag", QColor("#1abc9c"), 18), "标签管理");
        actTag->setCheckable(true);
        actTag->setChecked(pinnedList.contains("tag"));

        connect(actTag, &QAction::triggered, this, [this](bool checked) {
            QStringList currentPinned = AppConfig::instance().getValue("DriveBar/PinnedExtensions", QStringList{"tag", "text_ext", "save_thumbnail"}).toStringList();
            if (checked && !currentPinned.contains("tag")) {
                currentPinned.append("tag");
            } else if (!checked) {
                currentPinned.removeAll("tag");
            }
            AppConfig::instance().setValue("DriveBar/PinnedExtensions", currentPinned);
            AppConfig::instance().sync();

            refreshPinnedButtons();
        });

        QAction* actTextExt = extMenu->addAction(UiHelper::getIcon("Ext.svg", QColor("#e67e22"), 18), "文本扩展名设置");
        actTextExt->setCheckable(true);
        actTextExt->setChecked(pinnedList.contains("text_ext"));

        connect(actTextExt, &QAction::triggered, this, [this](bool checked) {
            QStringList currentPinned = AppConfig::instance().getValue("DriveBar/PinnedExtensions", QStringList{"tag", "text_ext", "save_thumbnail"}).toStringList();
            if (checked && !currentPinned.contains("text_ext")) {
                currentPinned.append("text_ext");
            } else if (!checked) {
                currentPinned.removeAll("text_ext");
            }
            AppConfig::instance().setValue("DriveBar/PinnedExtensions", currentPinned);
            AppConfig::instance().sync();

            refreshPinnedButtons();
        });

        QAction* actSaveThumb = extMenu->addAction(UiHelper::getIcon("save-Thumbnail", QColor("#3498db"), 18), "缩略图与缓存位置设置");
        actSaveThumb->setCheckable(true);
        actSaveThumb->setChecked(pinnedList.contains("save_thumbnail"));

        connect(actSaveThumb, &QAction::triggered, this, [this](bool checked) {
            QStringList currentPinned = AppConfig::instance().getValue("DriveBar/PinnedExtensions", QStringList{"tag", "text_ext", "save_thumbnail"}).toStringList();
            if (checked && !currentPinned.contains("save_thumbnail")) {
                currentPinned.append("save_thumbnail");
            } else if (!checked) {
                currentPinned.removeAll("save_thumbnail");
            }
            AppConfig::instance().setValue("DriveBar/PinnedExtensions", currentPinned);
            AppConfig::instance().sync();

            refreshPinnedButtons();
        });

        extMenu->popup(m_btnExtensionManager->mapToGlobal(QPoint(0, m_btnExtensionManager->height())));
    });
}

} // namespace QuarkMeta
