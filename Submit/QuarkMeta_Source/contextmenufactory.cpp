#include "ContextMenuFactory.h"
#include "../UiHelper.h"
#include "../../util/ShellHelper.h"
#include "../ToolTipOverlay.h"
#include "../TabBarWidget.h"
#include "../../core/NavigationService.h"
#include "../StyleLibrary.h"
#include "../ColorPicker.h"
#include "../../core/CoreEngine.h"
#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QFileInfo>
#include <QWidgetAction>
#include <QGridLayout>
#include <QPushButton>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>

namespace QuarkMeta {

bool ContextMenuFactory::togglePinState(const QStringList& paths, bool pin) {
    if (paths.isEmpty()) return false;

    AppCommand cmd;
    cmd.type = AppCommandType::SetPinned;
    cmd.targetPaths = paths;
    cmd.params["pinned"] = pin;
    return CoreEngine::instance().executeCommand(cmd);
}

bool ContextMenuFactory::extractContentToClipboard(const QString& path) {
    if (path.isEmpty() || QFileInfo(path).isDir()) {
        ToolTipOverlay::instance()->showToast("提取失败：只能提取文件内容", 1500, QColor("#e81123"));
        return false;
    }

    QString ext = QFileInfo(path).suffix().toLower();
    if (!UiHelper::isTextFile(ext)) {
        ToolTipOverlay::instance()->showToast("不支持提取内容：文件格式超出纯文本范围", 1500, QColor("#e81123"));
        return false;
    }

    QString content;
    if (UiHelper::extractTextContent(path, content)) {
        QApplication::clipboard()->setText(content);
        ToolTipOverlay::instance()->showToast(QString("已成功提取内容并存入剪贴板 (共 %1 字符)").arg(content.length()), 1500, QColor("#2ecc71"));
        return true;
    } else {
        ToolTipOverlay::instance()->showToast("提取失败：文件超过限制或无法作为纯文本解析", 1500, QColor("#e81123"));
        return false;
    }
}

QAction* ContextMenuFactory::buildExtractContentAction(QMenu* menu, const QString& path, QObject* receiver) {
    if (!menu || path.isEmpty()) return nullptr;

    bool isFolder = QFileInfo(path).isDir();
    QString fileExt = QFileInfo(path).suffix().toLower();
    bool canExtract = !isFolder && UiHelper::isTextFile(fileExt);

    if (canExtract) {
        QAction* actExtract = menu->addAction(UiHelper::getIcon("copy", QColor("#EEEEEE"), 18), "支持提取内容（Ctrl+Shift+E）");
        QObject::connect(actExtract, &QAction::triggered, receiver ? receiver : menu, [path]() {
            extractContentToClipboard(path);
        });
        return actExtract;
    } else {
        QAction* actDisabled = menu->addAction(UiHelper::getIcon("prohibit", QColor("#888888"), 18), "不支持提取内容");
        actDisabled->setEnabled(false);
        return actDisabled;
    }
}

QAction* ContextMenuFactory::buildShowInExplorerAction(QMenu* menu, const QString& path, bool isBlankSpace, QObject* receiver) {
    if (!menu || path.isEmpty()) return nullptr;

    QAction* action = menu->addAction(UiHelper::getIcon("folder_search", QColor("#EEEEEE"), 18), kTextShowInExplorer);
    QObject::connect(action, &QAction::triggered, receiver ? receiver : menu, [path, isBlankSpace]() {
        if (isBlankSpace) {
            ShellHelper::openDirectoryInExplorer(path);
        } else {
            ShellHelper::openInExplorer(path);
        }
    });
    return action;
}

QMenu* ContextMenuFactory::buildIconPickerMenu(QMenu* parentMenu,
                                               const QString& curIconKey,
                                               const QString& curColorHex,
                                               std::function<void(const QString& iconKey)> onIconSelected,
                                               std::function<void(const QString& colorHex)> onColorSelected) {
    if (!parentMenu) return nullptr;

    QString iconKey = curIconKey.isEmpty() ? "folder_filled" : curIconKey;
    QString colorHex = curColorHex.isEmpty() ? "#888888" : curColorHex;

    // 1. 颜色条组件
    QWidgetAction* colorPickerAction = new QWidgetAction(parentMenu);
    ColorStripPicker* colorPickerWidget = new ColorStripPicker(colorHex, parentMenu);
    colorPickerAction->setDefaultWidget(colorPickerWidget);
    parentMenu->addAction(colorPickerAction);

    // 2. 切换图标二级菜单
    QMenu* iconMenu = parentMenu->addMenu(UiHelper::getIcon(iconKey, QColor(colorHex)), "切换图标");
    UiHelper::applyMenuStyle(iconMenu);

    QWidgetAction* pickerAction = new QWidgetAction(iconMenu);
    QWidget* pickerWidget = new QWidget(iconMenu);
    QGridLayout* pickerLayout = new QGridLayout(pickerWidget);
    pickerLayout->setContentsMargins(6, 6, 6, 6);
    pickerLayout->setSpacing(6);

    static const QStringList builtInIcons = {
        "toolbox", "file_managed", "folder_managed",
        "Favorite_filled", "camera", "heart_filled",
        "thumbs_up", "thumbs_down", "share",
        "flag", "mail", "at_sign",
        "cloud_upload", "cloud_download", "server",
        "hard_drive", "video", "music",
        "shield_check", "paperclip", "clock_history",
        "bicycle", "thumbs_up_filled", "thumbs_down_filled",
        "folder_filled", "share_filled", "prohibit",
        "archive-svgrepo-com.svg", "document-attach-outline.svg", "image-picture-svgrepo-com.svg",
        "microsoftonenote-svgrepo-com.svg", "alarm-clock-filled-svgrepo-com.svg", "corel-draw-svgrepo-com.svg",
        "dbs-hadoop-svgrepo-com.svg", "shield-svgrepo-com.svg", "suitcase1-svgrepo-com.svg",
        "unarchive-svgrepo-com.svg", "usb-svgrepo-com.svg", "zipped-svgrepo-com.svg",
        "uncategorized-2", "Folder-add.svg", "Favorites.svg",
        "Destination.svg", "smart-folder-1.svg", "Database-1.svg",
        "Export-1.svg", "Monitoring-1.svg", "path-3.svg",
        "save-thumbnails-1.svg", "Data_Classification_svg", "path_2_svg",
        "save_thumbnails_2_svg", "fire_svgrepo_com_svg", "Folder_path_svg",
        "ice_cream_cone_svg", "ice_cream_cup_svg", "Linked_svg",
        "Linked_3_svg", "Linked_4_svg", "Linked_5_svg",
        "air_balloon_holiday_tourism_svgrepo_com_svg", "alligator_pear_avocado_avocado_pear_svgrepo_com_svg", "ananas_fruit_pineapple_svgrepo_com_svg",
        "archive_svgrepo_com_svg", "banana_bananas_bunch_svgrepo_com_svg", "beach_svg",
        "bean_beans_kidney_svgrepo_com_svg", "branch_fruit_olive_svgrepo_com_svg", "bus_holiday_tourism_2_svgrepo_com_svg",
        "coconut_holiday_tourism_svgrepo_com_svg", "eiffel_holiday_tourism_svgrepo_com_svg", "elephant_svg",
        "EXT_svg", "File_path_svg", "folder_special_svg",
        "fragaria_fruit_strawberry_svgrepo_com_svg", "fresh_tomato_tomatoes_svgrepo_com_svg", "fruit_grape_grapes_svgrepo_com_svg",
        "gantanhao_svg", "gantanhao_1_svg", "gantanhao_2_svg",
        "holiday_juice_tourism_svgrepo_com_svg", "holiday_tourism_travel_3_svgrepo_com_svg", "info_filled_svg",
        "kaixin101_logotype_of_a_smiling_star_svgrepo_com_svg", "Linked_1_svg", "psd_2_svg",
        "pumpkin_halloween_svgrepo_com_svg", "RapidNotes_1_svg", "sea_snail_svg",
        "time_planning_svgrepo_com_svg", "turtle_svg",
        "home_filled", "star_filled", "tag_filled", "bookmark_filled",
        "gift_filled", "calendar_filled", "trash_filled", "download"
    };

    QColor catColor = QColor(colorHex);
    int row = 0, col = 0;
    QList<QPair<QPushButton*, QString>> iconButtons;

    for (const QString& key : builtInIcons) {
        QPushButton* btn = new QPushButton(pickerWidget);
        btn->setFixedSize(28, 28);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setObjectName("FavPickerIconBtn");
        btn->setIcon(UiHelper::getIcon(key, catColor, 18));
        btn->setIconSize(QSize(18, 18));
        pickerLayout->addWidget(btn, row, col);

        iconButtons.append({btn, key});

        QObject::connect(btn, &QPushButton::clicked, parentMenu, [key, onIconSelected]() {
            if (onIconSelected) onIconSelected(key);
        });

        col++;
        if (col >= 10) { col = 0; row++; }
    }

    pickerWidget->setLayout(pickerLayout);
    pickerAction->setDefaultWidget(pickerWidget);
    iconMenu->addAction(pickerAction);

    QObject::connect(colorPickerWidget, &ColorStripPicker::colorSelected, parentMenu, [iconMenu, iconButtons, onColorSelected](const QString& selectedHex) {
        QString finalColor = selectedHex.isEmpty() ? "#888888" : selectedHex.toUpper();
        iconMenu->setIcon(UiHelper::getIcon("folder_filled", QColor(finalColor)));
        for (const auto& btnPair : iconButtons) {
            btnPair.first->setIcon(UiHelper::getIcon(btnPair.second, QColor(finalColor), 18));
        }
        if (onColorSelected) onColorSelected(finalColor);
    });

    parentMenu->addSeparator();
    return iconMenu;
}

bool ContextMenuFactory::copyPathsToClipboard(const QStringList& paths, bool showOverlay) {
    if (paths.isEmpty()) return false;

    QStringList cleanPaths;
    for (const auto& p : paths) {
        if (!p.isEmpty()) {
            cleanPaths << QDir::toNativeSeparators(p);
        }
    }
    if (cleanPaths.isEmpty()) return false;

    QApplication::clipboard()->setText(cleanPaths.join("\r\n"));
    if (showOverlay) {
        ToolTipOverlay::instance()->showToast("已复制路径至剪贴板", 1500, Style::SuccessGreen);
    }
    return true;
}

bool ContextMenuFactory::copyNamesToClipboard(const QStringList& paths, NameDelimiter delimiter, bool showOverlay) {
    if (paths.isEmpty()) return false;

    QStringList names;
    for (const auto& p : paths) {
        if (!p.isEmpty()) {
            QString name = QFileInfo(p).fileName();
            if (!name.isEmpty() && !names.contains(name)) {
                names << name;
            }
        }
    }
    if (names.isEmpty()) return false;

    QString sep;
    switch (delimiter) {
        case NameDelimiter::NewLine:
            sep = "\r\n";
            break;
        case NameDelimiter::Enumeration:
            sep = "、";
            break;
        case NameDelimiter::Comma:
            sep = ",";
            break;
    }

    QApplication::clipboard()->setText(names.join(sep));
    if (showOverlay) {
        ToolTipOverlay::instance()->showToast("已复制名称至剪贴板", 1500, Style::SuccessGreen);
    }
    return true;
}

bool ContextMenuFactory::copyNamesToClipboard(const QStringList& paths, bool showOverlay) {
    return copyNamesToClipboard(paths, NameDelimiter::NewLine, showOverlay);
}

QAction* ContextMenuFactory::buildCopyPathAction(QMenu* menu, const QStringList& paths, QObject* receiver) {
    if (!menu || paths.isEmpty()) return nullptr;

    // 1. 创建“复制路径”二级父菜单
    QMenu* copyPathMenu = menu->addMenu(UiHelper::getIcon("link", QColor("#EEEEEE"), 18), "复制路径");
    UiHelper::applyMenuStyle(copyPathMenu);

    QObject* target = receiver ? receiver : menu;

    // 辅助 lambda：添加首尾双引号并存入系统剪贴板，随后弹出 Toast 提示
    auto copyQuotedToClipboard = [](const QString& textToCopy) {
        QString quoted = QString("\"%1\"").arg(textToCopy);
        QApplication::clipboard()->setText(quoted);
        ToolTipOverlay::instance()->showToast("已复制路径至剪贴板", 1500, Style::SuccessGreen);
    };

    // 2. 第一个子选项：“复制完整路径”
    QString primaryNativePath = QDir::toNativeSeparators(paths.first());
    QAction* actFull = copyPathMenu->addAction(UiHelper::getIcon("link", QColor("#EEEEEE"), 16), "复制完整路径");
    QObject::connect(actFull, &QAction::triggered, target, [paths, copyQuotedToClipboard]() {
        if (paths.size() == 1) {
            copyQuotedToClipboard(QDir::toNativeSeparators(paths.first()));
        } else {
            QStringList quotedList;
            for (const QString& p : paths) {
                if (!p.isEmpty()) {
                    quotedList << QString("\"%1\"").arg(QDir::toNativeSeparators(p));
                }
            }
            QApplication::clipboard()->setText(quotedList.join("\r\n"));
            ToolTipOverlay::instance()->showToast(QString("已复制 %1 个完整路径至剪贴板").arg(quotedList.size()), 1500, Style::SuccessGreen);
        }
    });

    // 3. 按照层级拆解相对路径子选项
    QStringList parts = primaryNativePath.split('\\', Qt::SkipEmptyParts);

    if (parts.size() >= 2) {
        copyPathMenu->addSeparator();

        // 从第 1 层（去除盘符后）开始逐层向后切片，直到倒数第二层（保留：父文件夹\文件名）
        for (int i = 1; i < parts.size() - 1; ++i) {
            QStringList subParts = parts.mid(i);
            QString levelPath = subParts.join('\\');

            QAction* actLevel = copyPathMenu->addAction(levelPath);
            QObject::connect(actLevel, &QAction::triggered, target, [levelPath, copyQuotedToClipboard]() {
                copyQuotedToClipboard(levelPath);
            });
        }
    }

    return copyPathMenu->menuAction();
}

QMenu* ContextMenuFactory::buildTabCascadeMenu(QMenu* parentMenu, const QString& path, QWidget* contextWidget) {
    if (!parentMenu || path.isEmpty()) return nullptr;

    QMenu* tabCascadeMenu = parentMenu->addMenu(UiHelper::getIcon("add", QColor("#EEEEEE"), 18), "标签页");
    UiHelper::applyMenuStyle(tabCascadeMenu);

    QAction* actOpenNewTab = tabCascadeMenu->addAction(UiHelper::getIcon("add", QColor("#EEEEEE"), 16), "在新标签页中打开");
    QObject::connect(actOpenNewTab, &QAction::triggered, contextWidget ? contextWidget : parentMenu, [path]() {
        NavigationService::instance().openInNewTab(path);
    });

    QMenu* openInExistingTabMenu = tabCascadeMenu->addMenu(UiHelper::getIcon("folder_filled", QColor("#EEEEEE"), 16), "在标签页中打开");
    UiHelper::applyMenuStyle(openInExistingTabMenu);

    QWidget* windowWidget = contextWidget ? contextWidget->window() : nullptr;
    TabBarWidget* tabBar = windowWidget ? windowWidget->findChild<TabBarWidget*>() : nullptr;
    if (tabBar) {
        int count = tabBar->tabCount();
        for (int i = 0; i < count; ++i) {
            QString tabTitle = tabBar->tabTitle(i);
            QAction* actMergeTab = openInExistingTabMenu->addAction(UiHelper::getIcon("tab", QColor("#EEEEEE"), 16), tabTitle);
            QObject::connect(actMergeTab, &QAction::triggered, contextWidget ? contextWidget : parentMenu, [tabBar, i, path]() {
                tabBar->openInExistingTab(i, path);
            });
        }
    }

    return tabCascadeMenu;
}

QMenu* ContextMenuFactory::buildOpenWithMenu(QMenu* parentMenu, const QString& filePath, QWidget* parentWidget) {
    if (filePath.isEmpty()) return nullptr;
    return buildOpenWithMenu(parentMenu, QStringList() << filePath, parentWidget);
}

QMenu* ContextMenuFactory::buildOpenWithMenu(QMenu* parentMenu, const QStringList& filePaths, QWidget* parentWidget) {
    if (!parentMenu || filePaths.isEmpty()) return nullptr;

    QStringList validFiles;
    for (const QString& p : filePaths) {
        if (!p.isEmpty() && !QFileInfo(p).isDir()) {
            validFiles << p;
        }
    }
    if (validFiles.isEmpty()) return nullptr;

    QMenu* openWithMenu = parentMenu->addMenu(UiHelper::getIcon("launch", QColor("#EEEEEE"), 18), "打开方式");
    UiHelper::applyMenuStyle(openWithMenu);

    QString primaryFile = validFiles.first();
    QList<ShellHelper::AssocAppInfo> apps = ShellHelper::getAssociatedApps(primaryFile);
    for (const auto& app : apps) {
        QIcon icon = app.icon.isNull() ? UiHelper::getIcon("launch", QColor("#EEEEEE"), 16) : app.icon;
        QAction* act = openWithMenu->addAction(icon, app.name);
        QString exec = app.execPath;
        QObject::connect(act, &QAction::triggered, [validFiles, exec]() {
            if (!exec.isEmpty()) {
                QProcess::startDetached(exec, validFiles);
            } else {
                for (const QString& f : validFiles) {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(f));
                }
            }
        });
    }

    if (!apps.isEmpty()) {
        openWithMenu->addSeparator();
    }

    QAction* actChooseOther = openWithMenu->addAction(UiHelper::getIcon("add", QColor("#EEEEEE"), 16), "选择其他应用...");
    QObject::connect(actChooseOther, &QAction::triggered, [validFiles, parentWidget]() {
        if (validFiles.size() == 1) {
            ShellHelper::showOpenWithDialog(validFiles.first(), parentWidget);
        } else {
            QString exec = QFileDialog::getOpenFileName(parentWidget, "选择打开程序", QString(), "可执行程序 (*.exe);;所有文件 (*.*)");
            if (!exec.isEmpty()) {
                QProcess::startDetached(exec, validFiles);
            }
        }
    });

    return openWithMenu;
}

QAction* ContextMenuFactory::buildCopyNameAction(QMenu* menu, const QStringList& paths, QObject* receiver) {
    if (!menu || paths.isEmpty()) return nullptr;

    QMenu* subMenu = menu->addMenu(UiHelper::getIcon("text", QColor("#EEEEEE"), 18), "复制名称");
    UiHelper::applyMenuStyle(subMenu);

    QObject* target = receiver ? receiver : menu;

    QAction* actNewLine = subMenu->addAction(UiHelper::getIcon("text", QColor("#EEEEEE"), 18), "分行名称");
    QObject::connect(actNewLine, &QAction::triggered, target, [paths]() {
        copyNamesToClipboard(paths, NameDelimiter::NewLine, true);
    });

    QAction* actEnum = subMenu->addAction(UiHelper::getIcon("text", QColor("#EEEEEE"), 18), "顿号分割名称");
    QObject::connect(actEnum, &QAction::triggered, target, [paths]() {
        copyNamesToClipboard(paths, NameDelimiter::Enumeration, true);
    });

    QAction* actComma = subMenu->addAction(UiHelper::getIcon("text", QColor("#EEEEEE"), 18), "逗号分割名称");
    QObject::connect(actComma, &QAction::triggered, target, [paths]() {
        copyNamesToClipboard(paths, NameDelimiter::Comma, true);
    });

    return subMenu->menuAction();
}

QAction* ContextMenuFactory::buildPinToggleAction(QMenu* menu, bool isPinned, std::function<void(bool)> onToggle, QObject* receiver) {
    if (!menu) return nullptr;

    QIcon icon = UiHelper::getIcon(isPinned ? "pin_tilted" : "pin_vertical", QColor("#EEEEEE"), 18);
    QString text = isPinned ? "取消置顶（Alt+D）" : "置顶（Alt+D）";

    QAction* action = menu->addAction(icon, text);
    QObject::connect(action, &QAction::triggered, receiver ? receiver : menu, [isPinned, onToggle]() {
        if (onToggle) onToggle(!isPinned);
    });
    return action;
}

} // namespace QuarkMeta