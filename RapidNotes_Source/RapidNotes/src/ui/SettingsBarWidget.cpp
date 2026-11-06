#include "SettingsBarWidget.h"
#include "UiHelper.h"
#include "TagManagerWindow.h"
#include "dialogs/TextExtensionDialog.h"
#include "dialogs/CacheLocationDialog.h"
#include "ToolTipOverlay.h"
#include "../core/NavigationService.h"
#include "../core/AppConfig.h"

#include <QMenu>
#include <QEvent>

namespace RapidNotes {

SettingsBarWidget::SettingsBarWidget(QWidget* parent)
    : QWidget(parent) {
    setObjectName("SettingsBar");
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedHeight(42);
    initUi();
}

QPushButton* SettingsBarWidget::createIconButton(const QString& iconKey, const QColor& color, const QString& tooltipText) {
    QPushButton* btn = new QPushButton(UiHelper::getIcon(iconKey, color, 18), "", this);
    btn->setFixedSize(32, 32);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setObjectName("SettingsBarIconButton");
    btn->setProperty("tooltipText", tooltipText);
    btn->setAttribute(Qt::WA_Hover);

    // 绑定 ToolTipOverlay 悬浮提示，纯图标零文字暴露
    btn->installEventFilter(this);
    return btn;
}

bool SettingsBarWidget::eventFilter(QObject* watched, QEvent* event) {
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
        ToolTipOverlay::hideTip();
    }
    return QWidget::eventFilter(watched, event);
}

void SettingsBarWidget::initUi() {
    m_settingsBarLayout = new QHBoxLayout(this);
    m_settingsBarLayout->setContentsMargins(15, 5, 15, 5);
    m_settingsBarLayout->setSpacing(8);

    // 1. 最左侧（第一个按钮）：固定放置 extension 图标的扩展与组件管理器按钮
    m_btnExtensionManager = createIconButton("extension", QColor("#EEEEEE"), "扩展与组件管理");
    setupExtensionMenu();
    m_settingsBarLayout->addWidget(m_btnExtensionManager);

    // 2. 创建各独立功能图标（纯 Icon，通过 ToolTipOverlay 提示）
    m_btnTagManager = createIconButton("tag", QColor("#1abc9c"), "标签管理");
    connect(m_btnTagManager, &QPushButton::clicked, this, [this]() {
        TagManagerWindow* dlg = new TagManagerWindow(this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->exec();
    });

    m_btnTextExtension = createIconButton("Ext.svg", QColor("#e67e22"), "文本扩展名设置");
    connect(m_btnTextExtension, &QPushButton::clicked, this, [this]() {
        TextExtensionDialog dlg(this);
        dlg.exec();
    });

    m_btnCacheLocation = createIconButton("save-Thumbnail", QColor("#3498db"), "存储位置设置");
    connect(m_btnCacheLocation, &QPushButton::clicked, this, [this]() {
        CacheLocationDialog dlg(this);
        dlg.exec();
    });

    // 默认加至布局并依配置显隐
    m_settingsBarLayout->addWidget(m_btnTagManager);
    m_settingsBarLayout->addWidget(m_btnTextExtension);
    m_settingsBarLayout->addWidget(m_btnCacheLocation);
    m_settingsBarLayout->addStretch();

    refreshPinnedButtons();
}

void SettingsBarWidget::refreshPinnedButtons() {
    QStringList pinnedList = AppConfig::instance().getValue("SettingsBar/PinnedExtensions", QStringList{"tag", "text_ext", "cache_location"}).toStringList();

    if (m_btnTagManager) {
        m_btnTagManager->setVisible(pinnedList.contains("tag"));
    }
    if (m_btnTextExtension) {
        m_btnTextExtension->setVisible(pinnedList.contains("text_ext"));
    }
    if (m_btnCacheLocation) {
        m_btnCacheLocation->setVisible(pinnedList.contains("cache_location") || pinnedList.contains("save_thumbnail"));
    }
}

void SettingsBarWidget::setupExtensionMenu() {
    QMenu* extMenu = UiHelper::createMenu(m_btnExtensionManager);
    extMenu->setObjectName("SettingsBarExtensionMenu");

    connect(m_btnExtensionManager, &QPushButton::clicked, this, [this, extMenu]() {
        extMenu->clear();

        QStringList pinnedList = AppConfig::instance().getValue("SettingsBar/PinnedExtensions", QStringList{"tag", "text_ext", "cache_location"}).toStringList();

        QAction* actTag = extMenu->addAction(UiHelper::getIcon("tag", QColor("#1abc9c"), 18), "标签管理");
        actTag->setCheckable(true);
        actTag->setChecked(pinnedList.contains("tag"));

        connect(actTag, &QAction::triggered, this, [this](bool checked) {
            QStringList currentPinned = AppConfig::instance().getValue("SettingsBar/PinnedExtensions", QStringList{"tag", "text_ext", "cache_location"}).toStringList();
            if (checked && !currentPinned.contains("tag")) {
                currentPinned.append("tag");
            } else if (!checked) {
                currentPinned.removeAll("tag");
            }
            AppConfig::instance().setValue("SettingsBar/PinnedExtensions", currentPinned);
            AppConfig::instance().sync();

            refreshPinnedButtons();
        });

        QAction* actTextExt = extMenu->addAction(UiHelper::getIcon("Ext.svg", QColor("#e67e22"), 18), "文本扩展名设置");
        actTextExt->setCheckable(true);
        actTextExt->setChecked(pinnedList.contains("text_ext"));

        connect(actTextExt, &QAction::triggered, this, [this](bool checked) {
            QStringList currentPinned = AppConfig::instance().getValue("SettingsBar/PinnedExtensions", QStringList{"tag", "text_ext", "cache_location"}).toStringList();
            if (checked && !currentPinned.contains("text_ext")) {
                currentPinned.append("text_ext");
            } else if (!checked) {
                currentPinned.removeAll("text_ext");
            }
            AppConfig::instance().setValue("SettingsBar/PinnedExtensions", currentPinned);
            AppConfig::instance().sync();

            refreshPinnedButtons();
        });

        QAction* actCacheLoc = extMenu->addAction(UiHelper::getIcon("save-Thumbnail", QColor("#3498db"), 18), "存储位置设置");
        actCacheLoc->setCheckable(true);
        actCacheLoc->setChecked(pinnedList.contains("cache_location") || pinnedList.contains("save_thumbnail"));

        connect(actCacheLoc, &QAction::triggered, this, [this](bool checked) {
            QStringList currentPinned = AppConfig::instance().getValue("SettingsBar/PinnedExtensions", QStringList{"tag", "text_ext", "cache_location"}).toStringList();
            if (checked && !currentPinned.contains("cache_location")) {
                currentPinned.append("cache_location");
            } else if (!checked) {
                currentPinned.removeAll("cache_location");
                currentPinned.removeAll("save_thumbnail");
            }
            AppConfig::instance().setValue("SettingsBar/PinnedExtensions", currentPinned);
            AppConfig::instance().sync();

            refreshPinnedButtons();
        });

        extMenu->popup(m_btnExtensionManager->mapToGlobal(QPoint(0, m_btnExtensionManager->height())));
    });
}

} // namespace RapidNotes
