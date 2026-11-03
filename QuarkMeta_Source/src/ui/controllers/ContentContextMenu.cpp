#include "ContentContextMenu.h"
#include "../ContentPanel.h"
#include "../../meta/HistoryService.h"
#include "../ColumnViewWidget.h"
#include "../TitleBarWidget.h"
#include "../TabBarWidget.h"
#include "ContentSortController.h"
#include "ContentKeyHandler.h"
#include "../UiHelper.h"
#include "../StyleLibrary.h"
#include "../ToolTipOverlay.h"
#include "../ColorPicker.h"
#include "../FavoritePanel.h"
#include "../BatchRenameDialog.h"
#include "../BatchCreateDialog.h"
#include "../FramelessDialog.h"
#include "../dialogs/FramelessInputDialog.h"
#include "../dialogs/CreateFolderWithItemsDialog.h"
#include "../dialogs/FileCompareDialog.h"
#include "../../util/ShellHelper.h"
#include "../../util/DeepThumbnailExtractor.h"
#include "../../core/TrashService.h"
#include "../../core/DiskTrashService.h"
#include "../../core/PermanentDeleteService.h"
#include "../../core/ClipboardService.h"
#include "../../core/NavigationHistoryService.h"
#include "../../core/NavigationService.h"
#include "../../core/OperationSnapshotEngine.h"
#include "../../util/DiskIoService.h"
#include <QFileDialog>
#include "../../core/CoreEngine.h"
#include "../../meta/MetadataManager.h"
#include "../../meta/FavoriteDao.h"
#include "../../meta/FavoriteService.h"
#include "../../meta/MarkedFolderService.h"
#include "../../meta/LibraryService.h"
#include "ContextMenuFactory.h"
#include "../../crypto/EncryptionManager.h"
#include "../../core/LastOperationManager.h"
#include "../ShellIconManager.h"

#include <QMenu>
#include <QWidgetAction>
#include <QActionGroup>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include <QClipboard>
#include <QThreadPool>
#include <QPointer>
#include <QMouseEvent>

namespace QuarkMeta {

ContentContextMenu::ContentContextMenu(ContentPanel* parentPanel)
    : QObject(parentPanel), m_panel(parentPanel) {
}

void ContentContextMenu::showMenu(QAbstractItemView* view, const QPoint& pos) {
    if (!m_panel || !view) return;

    QModelIndex currentIndex = view->indexAt(pos);
    bool onItem = currentIndex.isValid() && !currentIndex.data(SectionHeaderRole).toBool();

    if (onItem && view->selectionModel()) {
        if (!view->selectionModel()->isSelected(currentIndex)) {
            view->selectionModel()->select(currentIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
            view->setCurrentIndex(currentIndex);
        }
    }

    QModelIndex col0Index = onItem ? currentIndex.sibling(currentIndex.row(), static_cast<int>(FileListColumn::Name)) : QModelIndex();
    QString path = onItem ? col0Index.data(PathRole).toString() : "";
    QFileInfo itemInfo(path);

    QString currentPath = m_panel->pathForView(view);

    QString currentCategoryType = m_panel->getCurrentCategoryType();

    bool isComputerRoot = (currentPath.isEmpty() || currentPath == "computer://");
    bool isTrashView = (currentCategoryType == "trash" || currentPath == "trash://");

    bool isDriveRoot = onItem && (itemInfo.isRoot() || path.endsWith(":\\") || path.endsWith(":/") || (path.length() == 2 && path.endsWith(':')));
    bool isFolder = onItem && (isDriveRoot || currentIndex.data(TypeRole).toString() == "folder");

    QMenu menu(m_panel);
    UiHelper::applyMenuStyle(&menu);

    // =========================================================================
    // 场景 1：回收站视图
    // =========================================================================
    if (isTrashView) {
        if (onItem) {
            menu.addAction(UiHelper::getIcon("refresh", QColor("#EEEEEE"), 18), "还原")->setData(ContentPanel::ActionRestore);
            menu.addAction(UiHelper::getIcon("cut", QColor("#EEEEEE"), 18), "剪切")->setData(ContentPanel::ActionCut);
            menu.addAction(UiHelper::getIcon("trash", QColor("#EEEEEE"), 18), "永久删除")->setData(ContentPanel::ActionSecureDelete);
            menu.addSeparator();
        }
        menu.addAction(UiHelper::getIcon("refresh", QColor("#EEEEEE"), 18), "还原全部")->setData(ContentPanel::ActionRestoreAll);
        menu.addAction(UiHelper::getIcon("trash", QColor("#EEEEEE"), 18), "清空回收站")->setData(ContentPanel::ActionEmptyTrash);

        m_panel->setContextMenuActive(true);
        QAction* selectedAction = menu.exec(view->viewport()->mapToGlobal(pos));
        m_panel->setContextMenuActive(false);

        if (!selectedAction || !selectedAction->data().isValid() || selectedAction->data().typeId() != QMetaType::Int) return;

        ContentPanel::ContextAction action = static_cast<ContentPanel::ContextAction>(selectedAction->data().toInt());
        if (action == ContentPanel::ActionNone) return;
        switch (action) {
            case ContentPanel::ActionRestore:
                TrashService::instance().restoreItems(m_panel->getSelectedTrashIds(), m_panel);
                break;
            case ContentPanel::ActionCut:
                ClipboardService::instance().cutItems(m_panel->getSelectedPaths());
                break;
            case ContentPanel::ActionSecureDelete:
                PermanentDeleteService::instance().execute(m_panel->getSelectedPaths(), m_panel);
                break;
            case ContentPanel::ActionRestoreAll:
                TrashService::instance().restoreAll(m_panel);
                break;
            case ContentPanel::ActionEmptyTrash:
                // 🚀【统一安全管线】：直接调用具有弹窗确认、进度条和扇区覆写的全量清空管线
                PermanentDeleteService::instance().executeEmptyTrash(m_panel);
                break;
            default:
                break;
        }
        return;
    }

    // =========================================================================
    // 场景 2：选中具体项目
    // =========================================================================
    if (onItem) {
        if (isDriveRoot) {
            menu.addAction(UiHelper::getIcon("open", QColor("#EEEEEE"), 18), "打开")->setData(ContentPanel::ActionOpen);
            menu.addAction(UiHelper::getIcon("add", QColor("#EEEEEE"), 18), "在新标签页中打开")->setData(ContentPanel::ActionOpenInNewTab);
            QAction* actShowInExpDrive = menu.addAction(UiHelper::getIcon("folder_search", QColor("#EEEEEE"), 18), ContextMenuFactory::kTextShowInExplorer);
            actShowInExpDrive->setData(ContentPanel::ActionShowInExplorer);

            QString currentColorStr = currentIndex.data(ColorRole).toString();
            QWidgetAction* pickerAction = new QWidgetAction(&menu);
            ColorStripPicker* pickerWidget = new ColorStripPicker(currentColorStr, &menu);
            pickerAction->setDefaultWidget(pickerWidget);
            menu.addAction(pickerAction);

            connect(pickerWidget, &ColorStripPicker::colorSelected, this, [this, view, &menu](const QString& hexColor) {
                auto indexes = view->selectionModel()->selectedIndexes();
                QStringList targetPaths;
                for (const auto& idx : indexes) {
                    if (idx.column() == static_cast<int>(FileListColumn::Name)) {
                        QString p = idx.data(PathRole).toString();
                        if (!p.isEmpty()) targetPaths << p;
                    }
                }
                if (!targetPaths.isEmpty()) {
                    AppCommand cmd;
                    cmd.type = AppCommandType::SetColor;
                    cmd.targetPaths = targetPaths;
                    cmd.params["color"] = hexColor;
                    CoreEngine::instance().executeCommand(cmd);
                    if (m_panel) {
                        for (const QString& p : targetPaths) m_panel->updateItemMetadata(p);
                    }
                }
                menu.close();
            });

            bool isPinned = currentIndex.data(PinnedRole).toBool();
            ContextMenuFactory::buildPinToggleAction(&menu, isPinned, [this, view](bool pin) {
                auto indexes = view->selectionModel()->selectedIndexes();
                QStringList targetPaths;
                for (const auto& idx : indexes) {
                    if (idx.column() == 0) {
                        QString p = idx.data(PathRole).toString();
                        if (!p.isEmpty()) targetPaths << p;
                    }
                }
                if (!targetPaths.isEmpty()) {
                    AppCommand cmd;
                    cmd.type = AppCommandType::SetPinned;
                    cmd.targetPaths = targetPaths;
                    cmd.params["pinned"] = pin;
                    CoreEngine::instance().executeCommand(cmd);
                }
            }, m_panel);

            QStringList selPathsDrive = m_panel->getSelectedPaths();
            if (selPathsDrive.isEmpty() && !path.isEmpty()) selPathsDrive << path;

            MarkedFolderService::instance().buildMarkAction(&menu, selPathsDrive, m_panel);
            FavoriteService::instance().buildFavoriteAction(&menu, path, m_panel);
            LibraryService::instance().buildLibraryAction(&menu, selPathsDrive, m_panel);

            int driveItemCount = selPathsDrive.size() > 0 ? static_cast<int>(selPathsDrive.size()) : 1;
            QMenu* newMenuDrive = menu.addMenu(UiHelper::getIcon("add", QColor("#EEEEEE"), 18), "新建...");
            UiHelper::applyMenuStyle(newMenuDrive);
            newMenuDrive->addAction(UiHelper::getIcon("folder_filled", QColor("#EEEEEE")), "创建文件夹（Ctrl+Shift+N）")->setData(ContentPanel::ActionNewFolder);
            newMenuDrive->addAction(UiHelper::getIcon("text", QColor("#EEEEEE")), "创建 Markdown（Ctrl+N）")->setData(ContentPanel::ActionNewMd);
            newMenuDrive->addAction(UiHelper::getIcon("text", QColor("#EEEEEE")), "创建纯文本文件 (txt)")->setData(ContentPanel::ActionNewTxt);
            newMenuDrive->addAction(UiHelper::getIcon("folder", QColor("#EEEEEE")), QString("用所选项目新建文件夹(%1)").arg(driveItemCount))->setData(ContentPanel::ActionCreateFolderWithSelectedItems);

            menu.addSeparator();

            QAction* actItemPaste = menu.addAction(UiHelper::getIcon("paste", QColor("#EEEEEE"), 18), "粘贴（Ctrl+V）");
            actItemPaste->setData(ContentPanel::ActionPaste);
            actItemPaste->setEnabled(m_panel->canPaste(path));

            QStringList selPaths = m_panel->getSelectedPaths();
            if (selPaths.isEmpty()) selPaths << path;
            ContextMenuFactory::buildCopyNameAction(&menu, selPaths, m_panel);
            ContextMenuFactory::buildCopyPathAction(&menu, selPaths, m_panel);

            QMenu* moreMenuDrive = menu.addMenu(UiHelper::getIcon("more_horizontal", QColor("#EEEEEE"), 18), "更多");
            UiHelper::applyMenuStyle(moreMenuDrive);

            ContextMenuFactory::buildExtractContentAction(moreMenuDrive, path, m_panel);

            QString nativePath = QDir::toNativeSeparators(path);
            QStringList itemTags;
            if (!nativePath.isEmpty()) {
                itemTags = MetadataManager::instance().getMeta(nativePath.toStdWString()).tags;
            }
            if (itemTags.isEmpty() && col0Index.isValid()) {
                itemTags = col0Index.data(TagsRole).toStringList();
            }
            QStringList cleanTags;
            for (const QString& t : itemTags) {
                QString trimmed = t.trimmed();
                if (!trimmed.isEmpty()) cleanTags << trimmed;
            }
            QAction* actCopyTags = menu.addAction(UiHelper::getIcon("tag", QColor("#EEEEEE"), 18), "复制标签");
            actCopyTags->setData(ContentPanel::ActionCopyTags);
            actCopyTags->setEnabled(!cleanTags.isEmpty());

            QAction* actPasteTags = menu.addAction(UiHelper::getIcon("paste_tag", QColor("#EEEEEE"), 18), "粘贴标签");
            actPasteTags->setData(ContentPanel::ActionPasteTags);
            actPasteTags->setEnabled(ClipboardService::instance().hasCopiedTags());

            QAction* actRepeat = menu.addAction(UiHelper::getIcon("repeat", QColor("#EEEEEE"), 18), LastOperationManager::instance().displayText());
            actRepeat->setData(ContentPanel::ActionRepeatLastOp);
            actRepeat->setEnabled(LastOperationManager::instance().hasOperation());

            menu.addSeparator();
            menu.addAction(UiHelper::getIcon("refresh", QColor("#EEEEEE"), 18), "刷新（F5）")->setData(ContentPanel::ActionRefresh);
        } else {
            menu.addAction(UiHelper::getIcon(isFolder ? "folder" : "open", QColor("#EEEEEE"), 18), isFolder ? "打开文件夹" : "打开")->setData(ContentPanel::ActionOpen);
            if (isFolder) {
                ContextMenuFactory::buildTabCascadeMenu(&menu, path, m_panel);
            } else {
                menu.addAction(UiHelper::getIcon("launch", QColor("#EEEEEE"), 18), "用系统默认程序打开")->setData(ContentPanel::ActionOpenDefault);
                QStringList selPathsForOpen = m_panel ? m_panel->getSelectedPaths() : QStringList();
                if (selPathsForOpen.isEmpty() && !path.isEmpty()) {
                    selPathsForOpen << path;
                }
                ContextMenuFactory::buildOpenWithMenu(&menu, selPathsForOpen, m_panel);
            }
            QAction* actShowInExp = menu.addAction(UiHelper::getIcon("folder_search", QColor("#EEEEEE"), 18), ContextMenuFactory::kTextShowInExplorer);
            actShowInExp->setData(ContentPanel::ActionShowInExplorer);

            bool isLibraryScene = m_panel && (m_panel->getCurrentCategoryType() == "library" ||
                                              m_panel->getCurrentCategoryType() == "smart_folder" ||
                                              m_panel->currentPath().startsWith("library://") ||
                                              m_panel->currentPath().startsWith("smartfolder://"));

            if (m_panel && (m_panel->isRecursive() || isLibraryScene)) {
                menu.addAction(UiHelper::getIcon("folder_filled", QColor("#EEEEEE"), 18), "在 QuarkMeta 中显示")->setData(ContentPanel::ActionShowInQuarkMeta);
            }

            QString currentColorStr = currentIndex.data(ColorRole).toString();
            QWidgetAction* pickerAction = new QWidgetAction(&menu);
            ColorStripPicker* pickerWidget = new ColorStripPicker(currentColorStr, &menu);
            pickerAction->setDefaultWidget(pickerWidget);
            menu.addAction(pickerAction);

            connect(pickerWidget, &ColorStripPicker::colorSelected, this, [this, view, &menu](const QString& hexColor) {
                auto indexes = view->selectionModel()->selectedIndexes();
                QStringList targetPaths;
                for (const auto& idx : indexes) {
                    if (idx.column() == 0) {
                        QString p = idx.data(PathRole).toString();
                        if (!p.isEmpty()) targetPaths << p;
                    }
                }
                if (!targetPaths.isEmpty()) {
                    AppCommand cmd;
                    cmd.type = AppCommandType::SetColor;
                    cmd.targetPaths = targetPaths;
                    cmd.params["color"] = hexColor;
                    CoreEngine::instance().executeCommand(cmd);
                    if (m_panel) {
                        for (const QString& p : targetPaths) m_panel->updateItemMetadata(p);
                    }
                }
                menu.close();
            });

            bool isPinned = currentIndex.data(PinnedRole).toBool();
            ContextMenuFactory::buildPinToggleAction(&menu, isPinned, [this, view](bool pin) {
                auto indexes = view->selectionModel()->selectedIndexes();
                QStringList targetPaths;
                for (const auto& idx : indexes) {
                    if (idx.column() == 0) {
                        QString p = idx.data(PathRole).toString();
                        if (!p.isEmpty()) targetPaths << p;
                    }
                }
                if (!targetPaths.isEmpty()) {
                    AppCommand cmd;
                    cmd.type = AppCommandType::SetPinned;
                    cmd.targetPaths = targetPaths;
                    cmd.params["pinned"] = pin;
                    CoreEngine::instance().executeCommand(cmd);
                }
            }, m_panel);

            QStringList selPathsItem = m_panel->getSelectedPaths();
            if (selPathsItem.isEmpty() && !path.isEmpty()) selPathsItem << path;

            MarkedFolderService::instance().buildMarkAction(&menu, selPathsItem, m_panel);
            FavoriteService::instance().buildFavoriteAction(&menu, path, m_panel);
            LibraryService::instance().buildLibraryAction(&menu, selPathsItem, m_panel);

            int selItemCount = selPathsItem.size() > 0 ? static_cast<int>(selPathsItem.size()) : 1;
            QMenu* newMenuItem = menu.addMenu(UiHelper::getIcon("add", QColor("#EEEEEE"), 18), "新建...");
            UiHelper::applyMenuStyle(newMenuItem);
            newMenuItem->addAction(UiHelper::getIcon("folder_filled", QColor("#EEEEEE")), "创建文件夹（Ctrl+Shift+N）")->setData(ContentPanel::ActionNewFolder);
            newMenuItem->addAction(UiHelper::getIcon("text", QColor("#EEEEEE")), "创建 Markdown（Ctrl+N）")->setData(ContentPanel::ActionNewMd);
            newMenuItem->addAction(UiHelper::getIcon("text", QColor("#EEEEEE")), "创建纯文本文件 (txt)")->setData(ContentPanel::ActionNewTxt);
            newMenuItem->addAction(UiHelper::getIcon("folder", QColor("#EEEEEE")), QString("用所选项目新建文件夹(%1)").arg(selItemCount))->setData(ContentPanel::ActionCreateFolderWithSelectedItems);

            menu.addSeparator();

            menu.addAction(UiHelper::getIcon("copy", QColor("#EEEEEE"), 18), "复制（Ctrl+C）")->setData(ContentPanel::ActionCopy);
            menu.addAction(UiHelper::getIcon("cut", QColor("#EEEEEE"), 18), "剪切（Ctrl+X）")->setData(ContentPanel::ActionCut);
            menu.addAction(UiHelper::getIcon("copy", QColor("#EEEEEE"), 18), "创建副本")->setData(ContentPanel::ActionDuplicate);

            if (!isComputerRoot && !currentPath.isEmpty()) {
                std::wstring volSerial = MetadataManager::getVolumeSerialNumber(path.toStdWString());
                QStringList recentFolders = NavigationHistoryService::getRecentVisitedFolders(volSerial);
                recentFolders.removeAll(currentPath);

                QMenu* moveMenu = menu.addMenu(UiHelper::getIcon("Export-1.svg", QColor("#EEEEEE"), 18), "移动到");
                UiHelper::applyMenuStyle(moveMenu);

                QAction* actDistribute = moveMenu->addAction(UiHelper::getIcon("Destination.svg", QColor("#EEEEEE"), 16), "一键分发 (Alt+V)");
                connect(actDistribute, &QAction::triggered, this, [this]() {
                    QStringList selectedPaths = m_panel->getSelectedPaths();
                    MarkedFolderService::instance().executeTransferToMarked(selectedPaths);
                });
                moveMenu->addSeparator();

                auto performMoveTo = [this](const QString& targetDir) {
                    QStringList selectedPaths = m_panel->getSelectedPaths();
                    if (selectedPaths.isEmpty()) return;

                    LastOperationManager::instance().recordMoveToFolder(targetDir);

                    DiskIoContext ioCtx;
                    ioCtx.sources = selectedPaths;
                    ioCtx.destination = targetDir;
                    ioCtx.isMove = true;

                    QPointer<ContentPanel> weakPanel(m_panel);
                    DiskIoService::instance().executeAsync(ioCtx, [weakPanel](bool success) {
                        QMetaObject::invokeMethod(QCoreApplication::instance(), [weakPanel, success]() {
                            if (weakPanel) {
                                if (success) {
                                    weakPanel->refreshAll();
                                    ToolTipOverlay::instance()->showToast("文件移动成功", 1500, QColor("#2ecc71"));
                                } else {
                                    ToolTipOverlay::instance()->showToast("移动失败：物理写入未能完成", 2000, QColor("#e81123"));
                                }
                            }
                        });
                    });
                };

                class MoveMenuHoverFilter : public QObject {
                public:
                    explicit MoveMenuHoverFilter(QMenu* menu) : QObject(menu), m_menu(menu) {}
                protected:
                    bool eventFilter(QObject* watched, QEvent* event) override {
                        if (watched == m_menu) {
                            if (event->type() == QEvent::MouseMove) {
                                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                                QAction* action = m_menu->actionAt(mouseEvent->pos());
                                if (action && action->property("hoverPath").isValid()) {
                                    QString path = action->property("hoverPath").toString();
                                    if (!path.isEmpty()) {
                                        QString nativePath = QDir::toNativeSeparators(path);
                                        if (m_lastHoveredPath != nativePath) {
                                            m_lastHoveredPath = nativePath;
                                            ToolTipOverlay::instance()->showText(QCursor::pos(), nativePath, 0);
                                        }
                                        return QObject::eventFilter(watched, event);
                                    }
                                }
                                m_lastHoveredPath.clear();
                                ToolTipOverlay::hideTip();
                            } else if (event->type() == QEvent::Leave) {
                                m_lastHoveredPath.clear();
                                ToolTipOverlay::hideTip();
                            }
                        }
                        return QObject::eventFilter(watched, event);
                    }
                private:
                    QMenu* m_menu;
                    QString m_lastHoveredPath;
                };

                moveMenu->installEventFilter(new MoveMenuHoverFilter(moveMenu));
                connect(moveMenu, &QMenu::aboutToHide, this, []() {
                    ToolTipOverlay::hideTip();
                });

                for (const QString& recentDir : recentFolders) {
                    QFileInfo dirInfo(recentDir);
                    QString displayName = dirInfo.fileName();
                    if (displayName.isEmpty()) {
                        displayName = recentDir; // 盘符或根目录降级显示原路径
                    }
                    QAction* actMove = moveMenu->addAction(UiHelper::getIcon("folder_filled", QColor("#EEEEEE"), 16), displayName);
                    actMove->setProperty("hoverPath", recentDir);
                    connect(actMove, &QAction::triggered, this, [performMoveTo, recentDir]() {
                        performMoveTo(recentDir);
                    });
                }

                if (!recentFolders.isEmpty()) {
                    moveMenu->addSeparator();
                }

                QAction* actBrowseMove = moveMenu->addAction(UiHelper::getIcon("folder", QColor("#EEEEEE"), 16), "浏览选择文件夹...");
                connect(actBrowseMove, &QAction::triggered, this, [this, performMoveTo]() {
                    QString selectedDir = QFileDialog::getExistingDirectory(
                        m_panel,
                        "选择移动的目标文件夹",
                        m_panel->currentPath(),
                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                    );
                    if (!selectedDir.isEmpty()) {
                        performMoveTo(selectedDir);
                    }
                });
            }

            QAction* actItemPaste = menu.addAction(UiHelper::getIcon("paste", QColor("#EEEEEE"), 18), "粘贴（Ctrl+V）");
            actItemPaste->setData(ContentPanel::ActionPaste);
            actItemPaste->setEnabled(m_panel->canPaste(isFolder ? path : currentPath));

            QStringList selPaths = m_panel->getSelectedPaths();
            if (selPaths.isEmpty()) selPaths << path;
            ContextMenuFactory::buildCopyNameAction(&menu, selPaths, m_panel);
            ContextMenuFactory::buildCopyPathAction(&menu, selPaths, m_panel);

            // 文件对比：仅当选中项里"文件"（排除文件夹、驱动器根目录、重复路径）数量 >= 2 时才出现
            QStringList compareFiles;
            for (const QString& p : selPathsItem) {
                if (!p.isEmpty() && !QFileInfo(p).isDir()) {
                    if (!compareFiles.contains(p)) {
                        compareFiles << p;
                    }
                }
            }
            if (compareFiles.size() >= 2) {
                menu.addAction(UiHelper::getIcon("Compare.svg", QColor("#EEEEEE"), 18), "文件对比")->setData(ContentPanel::ActionFileCompare);
            }

            QMenu* moreMenu = menu.addMenu(UiHelper::getIcon("more_horizontal", QColor("#EEEEEE"), 18), "更多");
            UiHelper::applyMenuStyle(moreMenu);

            ContextMenuFactory::buildExtractContentAction(moreMenu, path, m_panel);

            if (!isFolder) {
                moreMenu->addSeparator();
                moreMenu->addAction(UiHelper::getIcon("repeat", QColor("#EEEEEE"), 18), "重新提取缩略图")->setData(ContentPanel::ActionReextractThumbnail);

                QMenu* cryptoMenu = moreMenu->addMenu(UiHelper::getIcon("shield", QColor("#EEEEEE"), 18), "外壳保护");
                UiHelper::applyMenuStyle(cryptoMenu);
                cryptoMenu->addAction(UiHelper::getIcon("lock", QColor("#EEEEEE"), 18), "执行外壳保护")->setData(ContentPanel::ActionEncrypt);
                cryptoMenu->addAction(UiHelper::getIcon("unlock", QColor("#EEEEEE"), 18), "解除保护")->setData(ContentPanel::ActionDecrypt);
                cryptoMenu->addAction(UiHelper::getIcon("key", QColor("#EEEEEE"), 18), "修改保护密码")->setData(ContentPanel::ActionChangePwd);
            }

            QString nativePath = QDir::toNativeSeparators(path);
            QStringList itemTags;
            if (!nativePath.isEmpty()) {
                itemTags = MetadataManager::instance().getMeta(nativePath.toStdWString()).tags;
            }
            if (itemTags.isEmpty() && col0Index.isValid()) {
                itemTags = col0Index.data(TagsRole).toStringList();
            }
            QStringList cleanTags;
            for (const QString& t : itemTags) {
                QString trimmed = t.trimmed();
                if (!trimmed.isEmpty()) cleanTags << trimmed;
            }
            QAction* actCopyTags = menu.addAction(UiHelper::getIcon("tag", QColor("#EEEEEE"), 18), "复制标签");
            actCopyTags->setData(ContentPanel::ActionCopyTags);
            actCopyTags->setEnabled(!cleanTags.isEmpty());

            QAction* actPasteTags = menu.addAction(UiHelper::getIcon("paste_tag", QColor("#EEEEEE"), 18), "粘贴标签");
            actPasteTags->setData(ContentPanel::ActionPasteTags);
            actPasteTags->setEnabled(ClipboardService::instance().hasCopiedTags());

            QAction* actRepeat = menu.addAction(UiHelper::getIcon("repeat", QColor("#EEEEEE"), 18), LastOperationManager::instance().displayText());
            actRepeat->setData(ContentPanel::ActionRepeatLastOp);
            actRepeat->setEnabled(LastOperationManager::instance().hasOperation());

            menu.addAction(UiHelper::getIcon("rename-1.svg", QColor("#EEEEEE"), 18), "重命名（F2）")->setData(ContentPanel::ActionRename);

            menu.addSeparator();
            menu.addAction(UiHelper::getIcon("refresh", QColor("#EEEEEE"), 18), "刷新（F5）")->setData(ContentPanel::ActionRefresh);
        }
    }
    // =========================================================================
    // 场景 3：点击空白处
    // =========================================================================
    else {
        if (isComputerRoot) {
            ContextMenuFactory::buildShowInExplorerAction(&menu, "computer://", m_panel);
            menu.addAction(UiHelper::getIcon("refresh", QColor("#EEEEEE"), 18), "刷新（F5）")->setData(ContentPanel::ActionRefresh);
        } else {
            bool isWritablePhysical = m_panel && m_panel->isWritablePhysicalDirectoryView();
            if (isWritablePhysical) {
                QMenu* newMenu = menu.addMenu(UiHelper::getIcon("add", QColor("#EEEEEE"), 18), "新建...");
                UiHelper::applyMenuStyle(newMenu);
                newMenu->addAction(UiHelper::getIcon("folder_filled", QColor("#EEEEEE")), "创建文件夹（Ctrl+Shift+N）")->setData(ContentPanel::ActionNewFolder);
                newMenu->addAction(UiHelper::getIcon("text", QColor("#EEEEEE")), "创建 Markdown（Ctrl+N）")->setData(ContentPanel::ActionNewMd);
                newMenu->addAction(UiHelper::getIcon("text", QColor("#EEEEEE")), "创建纯文本文件 (txt)")->setData(ContentPanel::ActionNewTxt);

                menu.addSeparator();
                menu.addAction(UiHelper::getIcon("add", QColor("#EEEEEE")), "批量创建项目...")->setData(ContentPanel::ActionBatchCreate);
                menu.addSeparator();
            }

            QAction* actPaste = menu.addAction(UiHelper::getIcon("paste", QColor("#EEEEEE"), 18), "粘贴（Ctrl+V）");
            actPaste->setData(ContentPanel::ActionPaste);
            actPaste->setEnabled(m_panel->canPaste(currentPath));

            menu.addSeparator();
            bool isPhysicalPath = !currentPath.isEmpty() && !currentPath.contains("://") && QDir(currentPath).exists();
            if (isPhysicalPath) {
                QAction* actShowInExp = menu.addAction(UiHelper::getIcon("folder_search", QColor("#EEEEEE"), 18), ContextMenuFactory::kTextShowInExplorer);
                actShowInExp->setData(ContentPanel::ActionOpenDirectoryInExplorer);
            }

            menu.addAction(UiHelper::getIcon("refresh", QColor("#EEEEEE"), 18), "刷新（F5）")->setData(ContentPanel::ActionRefresh);
        }
    }

    // --- 拆分窗格控制菜单项 ---
    if (m_panel && (m_panel->isSplitMode() || m_panel->isSecondaryPane())) {
        menu.addSeparator();
        QAction* closePaneAction = menu.addAction(UiHelper::getIcon("close", QColor("#EEEEEE"), 18), "关闭窗格");
        QObject::connect(closePaneAction, &QAction::triggered, [this]() {
            if (m_panel) {
                m_panel->requestClosePane();
            }
        });
    }

    menu.addSeparator();

    // 排序二级子菜单
    bool isAscending = (m_panel->currentSortOrder() == Qt::AscendingOrder);
    QString sortIconKey = isAscending ? "arrow_up_long" : "arrow_down_long";
    QMenu* sortMenu = menu.addMenu(UiHelper::getIcon(sortIconKey, QColor("#EEEEEE"), 18), "排序");
    UiHelper::applyMenuStyle(sortMenu);

    QActionGroup* typeGroup = new QActionGroup(this);
    auto addTypeAct = [&](const QString& label, ContentPanel::SortType type) {
        QAction* act = sortMenu->addAction(label);
        act->setCheckable(true);
        act->setChecked(m_panel->currentSortType() == type);
        typeGroup->addAction(act);
        connect(act, &QAction::triggered, [this, type]() {
            m_panel->setSortType(type);
        });
    };

    addTypeAct("名称", ContentPanel::SortByName);
    addTypeAct("创建日期", ContentPanel::SortByCreateDate);
    addTypeAct("修改日期", ContentPanel::SortByModifyDate);
    addTypeAct("扩展名", ContentPanel::SortByExtension);
    addTypeAct("大小", ContentPanel::SortBySize);
    addTypeAct("尺寸", ContentPanel::SortByDimension);
    addTypeAct("评分", ContentPanel::SortByRating);

    sortMenu->addSeparator();

    QActionGroup* orderGroup = new QActionGroup(this);
    auto addOrderAct = [&](const QString& label, Qt::SortOrder order) {
        QAction* act = sortMenu->addAction(label);
        act->setCheckable(true);
        act->setChecked(m_panel->currentSortOrder() == order);
        orderGroup->addAction(act);
        connect(act, &QAction::triggered, [this, order]() {
            m_panel->setSortOrder(order);
        });
    };

    addOrderAct("升序", Qt::AscendingOrder);
    addOrderAct("降序", Qt::DescendingOrder);

    // 删除子菜单
    if (onItem && !isDriveRoot) {
        menu.addSeparator();
        QMenu* delMenu = menu.addMenu(UiHelper::getIcon("trash", QColor("#EEEEEE"), 18), "删除");
        UiHelper::applyMenuStyle(delMenu);
        delMenu->addAction(UiHelper::getIcon("trash", QColor("#EEEEEE"), 18), "移入回收站")->setData(ContentPanel::ActionDelete);
        delMenu->addAction(UiHelper::getIcon("trash", QColor("#EEEEEE"), 18), "永久删除")->setData(ContentPanel::ActionSecureDelete);
    }

    m_panel->setContextMenuActive(true);
    QAction* selectedAction = menu.exec(view->viewport()->mapToGlobal(pos));
    m_panel->setContextMenuActive(false);

    if (!selectedAction || !selectedAction->data().isValid() || selectedAction->data().typeId() != QMetaType::Int) return;

    ContentPanel::ContextAction action = static_cast<ContentPanel::ContextAction>(selectedAction->data().toInt());
    if (action == ContentPanel::ActionNone) return;

    switch (action) {
        case ContentPanel::ActionOpen:
            m_panel->activateIndex(currentIndex);
            break;
        case ContentPanel::ActionOpenInNewTab: {
            QString targetPath = path;
            if (targetPath.isEmpty()) {
                QStringList selected = m_panel->getSelectedPaths();
                if (!selected.isEmpty()) targetPath = selected.first();
            }
            if (!targetPath.isEmpty()) {
                NavigationService::instance().openInNewTab(targetPath);
            }
            break;
        }
        case ContentPanel::ActionOpenDefault: {
            auto indexes = view->selectionModel()->selectedIndexes();
            QStringList targetPaths;
            for (const auto& idx : indexes) {
                if (idx.column() == 0) {
                    QString filePath = idx.data(PathRole).toString();
                    if (!filePath.isEmpty() && QFileInfo::exists(filePath)) {
                        targetPaths << filePath;
                        HistoryService::instance().recordFileAccess(filePath);
                        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
                    }
                }
            }
            if (!targetPaths.isEmpty()) {
                AppCommand cmd;
                cmd.type = AppCommandType::RecordAccess;
                cmd.targetPaths = targetPaths;
                CoreEngine::instance().executeCommand(cmd);
            }
            break;
        }
        case ContentPanel::ActionShowInQuarkMeta: {
            QStringList selectedPaths = m_panel->getSelectedPaths();
            QString targetPath = !selectedPaths.isEmpty() ? selectedPaths.first() : path;
            if (!targetPath.isEmpty()) {
                QFileInfo fi(targetPath);
                QString targetDir = fi.absolutePath();
                m_panel->setPendingSelectPath(targetPath, false);

                QString currentNative = QDir::toNativeSeparators(QDir::cleanPath(currentPath));
                QString targetNative = QDir::toNativeSeparators(QDir::cleanPath(targetDir));

                if (QString::compare(currentNative, targetNative, Qt::CaseInsensitive) == 0 && !m_panel->isRecursive()) {
                    // 目标就在当前目录且当前非递归视图：直接就地恢复选中并高亮滚动
                    m_panel->restoreSelections();
                } else {
                    NavigationService::instance().navigateTo(targetDir);
                }
            }
            break;
        }
        case ContentPanel::ActionRepeatLastOp: {
            ContentKeyHandler::executeRepeatLastOp(m_panel, view);
            break;
        }
        case ContentPanel::ActionShowInExplorer: {
            QStringList selectedPaths = m_panel->getSelectedPaths();
            if (selectedPaths.isEmpty() && !path.isEmpty()) {
                selectedPaths << path;
            }

            // 按所在文件夹分组
            QMap<QString, QStringList> folderGroups;
            for (const QString& itemPath : selectedPaths) {
                if (itemPath.isEmpty() || itemPath.contains("://")) continue;
                QFileInfo fi(itemPath);
                // 文件取其所在目录；文件夹取其上级目录以在资源管理器中被高亮选中
                QString parentDir = fi.isDir() ? fi.dir().absolutePath() : fi.absolutePath();
                folderGroups[parentDir].append(itemPath);
            }

            if (folderGroups.size() > 5) {
                FramelessConfirmDialog dlg(
                    "打开资源管理器确认",
                    QString("本次操作将同时打开 %1 个资源管理器窗口，是否继续？").arg(folderGroups.size()),
                    FramelessConfirmDialog::OkCancel,
                    "warning",
                    QColor("#f1c40f"),
                    m_panel
                );
                if (dlg.exec() != QDialog::Accepted) {
                    break;
                }
            }

            for (auto it = folderGroups.begin(); it != folderGroups.end(); ++it) {
                // 打开该文件夹窗口并选中其中的项目（首个目标）
                if (!it.value().isEmpty()) {
                    ShellHelper::openInExplorer(it.value().first());
                }
            }
            break;
        }
        case ContentPanel::ActionOpenDirectoryInExplorer: {
            if (!currentPath.isEmpty() && !currentPath.contains("://") && QDir(currentPath).exists()) {
                ShellHelper::openDirectoryInExplorer(currentPath);
            }
            break;
        }
        case ContentPanel::ActionNewFolder:
            m_panel->createNewItem("folder");
            break;
        case ContentPanel::ActionCreateFolderWithSelectedItems: {
            QStringList selectedPaths = m_panel->getSelectedPaths();
            if (selectedPaths.isEmpty() && !path.isEmpty()) {
                selectedPaths << path;
            }
            if (!selectedPaths.isEmpty()) {
                CreateFolderWithItemsDialog dlg(currentPath, selectedPaths, m_panel);
                if (dlg.exec() == QDialog::Accepted) {
                    m_panel->refreshAll();
                }
            }
            break;
        }
        case ContentPanel::ActionNewMd:
            m_panel->createNewItem("md");
            break;
        case ContentPanel::ActionFileCompare: {
            QStringList selectedPaths = m_panel->getSelectedPaths();
            QStringList compareFiles;
            for (const QString& p : selectedPaths) {
                if (!p.isEmpty() && !QFileInfo(p).isDir()) {
                    if (!compareFiles.contains(p)) {
                        compareFiles << p;
                    }
                }
            }
            if (compareFiles.size() >= 2) {
                FileCompareDialog dlg(compareFiles, m_panel);
                dlg.exec();
            }
            break;
        }
        case ContentPanel::ActionNewTxt:
            m_panel->createNewItem("txt");
            break;
        case ContentPanel::ActionPin:
        case ContentPanel::ActionUnpin: {
            auto indexes = view->selectionModel()->selectedIndexes();
            bool pin = (action == ContentPanel::ActionPin);
            QStringList targetPaths;
            for (const auto& idx : indexes) {
                if (idx.column() == 0) {
                    QString p = idx.data(PathRole).toString();
                    if (!p.isEmpty()) targetPaths << p;
                }
            }
            if (targetPaths.isEmpty() && !path.isEmpty()) {
                targetPaths << path;
            }
            if (!targetPaths.isEmpty()) {
                ContextMenuFactory::togglePinState(targetPaths, pin);
            }
            break;
        }
        case ContentPanel::ActionEncrypt: {
            FramelessInputDialog dlg("加密保护", "设置加密密码:", "", m_panel);
            dlg.setEchoMode(QLineEdit::Password);
            if (dlg.exec() == QDialog::Accepted) {
                QString pwd = dlg.text();
                if (pwd.isEmpty()) break;
                auto indexes = view->selectionModel()->selectedIndexes();
                QStringList targets;
                for (const auto& idx : indexes) if (idx.column() == 0) targets << idx.data(PathRole).toString();

                ToolTipOverlay::instance()->showToast("加密任务已在后台启动...", 2000);

                QPointer<ContentPanel> self(m_panel);
                EncryptionManager::instance().encryptBatchAsync(targets, pwd.toStdString(), [self](bool success) {
                    if (self) self->refreshAll();
                    ToolTipOverlay::instance()->showToast(success ? "加密任务处理完成" : "部分项目加密失败", 1500, success ? QColor("#2ecc71") : QColor("#e81123"));
                });
            }
            break;
        }
        case ContentPanel::ActionDecrypt: {
            FramelessInputDialog dlg("解除外壳保护", "输入解密密码:", "", m_panel);
            dlg.setEchoMode(QLineEdit::Password);
            if (dlg.exec() == QDialog::Accepted) {
                QString pwd = dlg.text();
                if (pwd.isEmpty()) break;
                auto indexes = view->selectionModel()->selectedIndexes();
                QStringList targets;
                for (const auto& idx : indexes) if (idx.column() == 0) targets << idx.data(PathRole).toString();

                ToolTipOverlay::instance()->showToast("解密还原任务已在后台启动...", 2000);

                QPointer<ContentPanel> self(m_panel);
                EncryptionManager::instance().decryptBatchAsync(targets, pwd.toStdString(), [self](bool anySuccess) {
                    if (self) self->refreshAll();
                    if (anySuccess) {
                        ToolTipOverlay::instance()->showToast("解除保护成功，文件已还原", 1500, QColor("#2ecc71"));
                    } else {
                        ToolTipOverlay::instance()->showToast("解密失败，请检查密码是否正确", 2000, QColor("#e74c3c"));
                    }
                });
            }
            break;
        }
        case ContentPanel::ActionChangePwd: {
            FramelessInputDialog dlgOld("修改保护密码", "输入原密码:", "", m_panel);
            dlgOld.setEchoMode(QLineEdit::Password);
            if (dlgOld.exec() != QDialog::Accepted || dlgOld.text().isEmpty()) break;
            QString oldPwd = dlgOld.text();

            FramelessInputDialog dlgNew("修改保护密码", "输入新密码:", "", m_panel);
            dlgNew.setEchoMode(QLineEdit::Password);
            if (dlgNew.exec() != QDialog::Accepted || dlgNew.text().isEmpty()) break;
            QString newPwd = dlgNew.text();

            auto indexes = view->selectionModel()->selectedIndexes();
            QStringList targets;
            for (const auto& idx : indexes) if (idx.column() == 0) targets << idx.data(PathRole).toString();

            ToolTipOverlay::instance()->showToast("密码修改中...", 2000);

            QPointer<ContentPanel> self(m_panel);
            EncryptionManager::instance().changePasswordBatchAsync(targets, oldPwd.toStdString(), newPwd.toStdString(), [self](bool anySuccess) {
                if (self) self->refreshAll();
                if (anySuccess) {
                    ToolTipOverlay::instance()->showToast("保护密码修改成功", 1500, QColor("#2ecc71"));
                } else {
                    ToolTipOverlay::instance()->showToast("原密码错误，修改失败", 2000, QColor("#e74c3c"));
                }
            });
            break;
        }
        case ContentPanel::ActionBatchRename:
        case ContentPanel::ActionRename: {
            QStringList selectedPaths = m_panel->getSelectedPaths();
            if (selectedPaths.size() > 1) {
                m_panel->performBatchRename();
            } else {
                view->edit(currentIndex);
            }
            break;
        }
        case ContentPanel::ActionCopy:
            ClipboardService::instance().copyItems(m_panel->getSelectedPaths());
            break;
        case ContentPanel::ActionCut:
            ClipboardService::instance().cutItems(m_panel->getSelectedPaths());
            break;
        case ContentPanel::ActionDuplicate: {
            QStringList selectedPaths = m_panel->getSelectedPaths();
            if (selectedPaths.isEmpty() && !path.isEmpty()) {
                selectedPaths << path;
            }
            if (!selectedPaths.isEmpty()) {
                QString targetDir = isFolder ? QFileInfo(path).absolutePath() : currentPath;
                if (targetDir.isEmpty() || targetDir.contains("://")) {
                    targetDir = QFileInfo(selectedPaths.first()).absolutePath();
                }

                DiskIoContext ioCtx;
                ioCtx.sources = selectedPaths;
                ioCtx.destination = targetDir;
                ioCtx.isMove = false;
                ioCtx.autoRenameAll = true;

                QPointer<ContentPanel> weakPanel(m_panel);
                DiskIoService::instance().executeAsync(ioCtx, [weakPanel](bool success) {
                    QMetaObject::invokeMethod(QCoreApplication::instance(), [weakPanel, success]() {
                        if (weakPanel) {
                            if (success) {
                                weakPanel->refreshAll();
                                ToolTipOverlay::instance()->showToast("副本创建成功", 1500, QColor("#2ecc71"));
                            } else {
                                ToolTipOverlay::instance()->showToast("创建副本失败：物理写入未能完成", 2000, QColor("#e81123"));
                            }
                        }
                    });
                });
            }
            break;
        }
        case ContentPanel::ActionPaste:
            ClipboardService::instance().executePaste(isFolder ? path : currentPath, m_panel);
            break;
        case ContentPanel::ActionCopyTags: {
            QString nativePath = QDir::toNativeSeparators(path);
            QStringList tags;
            if (!nativePath.isEmpty()) {
                tags = MetadataManager::instance().getMeta(nativePath.toStdWString()).tags;
            }
            if (tags.isEmpty() && col0Index.isValid()) {
                tags = col0Index.data(TagsRole).toStringList();
            }
            QStringList cleanTags;
            for (const QString& t : tags) {
                QString trimmed = t.trimmed();
                if (!trimmed.isEmpty()) cleanTags << trimmed;
            }
            if (!cleanTags.isEmpty()) {
                ClipboardService::instance().setCopiedTags(cleanTags);
                ToolTipOverlay::instance()->showToast(QString("已复制 %1 个标签").arg(cleanTags.size()), 1500, QColor("#2ecc71"));
            } else {
                ToolTipOverlay::instance()->showToast("当前项目未绑定任何标签", 1500, QColor("#e81123"));
            }
            break;
        }
        case ContentPanel::ActionPasteTags: {
            ClipboardService::instance().executePasteTags(view);
            break;
        }
        case ContentPanel::ActionBatchCreate: {
            BatchCreateDialog dlg(currentPath, m_panel);
            if (dlg.exec() == QDialog::Accepted) {
                m_panel->refreshAll();
            }
            break;
        }
        case ContentPanel::ActionRestore: {
            auto indexes = view->selectionModel()->selectedIndexes();
            for (const auto& idx : indexes) {
                if (idx.column() == 0) {
                    if (idx.data(IsDiskTrashRole).toBool()) {
                        int id = idx.data(DiskTrashIdRole).toInt();
                        QString trashPath = idx.data(PathRole).toString();
                        DiskTrashService::restoreFromDiskTrash(id, trashPath);
                    }
                }
            }
            m_panel->refreshAll();
            MetadataManager::instance().notifyUI(MetadataManager::RefreshLevel::FullRebuild);
            break;
        }
        case ContentPanel::ActionDelete:
            TrashService::instance().moveToTrash(m_panel->getSelectedPaths(), m_panel);
            break;
        case ContentPanel::ActionSecureDelete:
            PermanentDeleteService::instance().execute(m_panel->getSelectedPaths(), m_panel);
            break;
        case ContentPanel::ActionAddToFavorites: {
            QStringList selectedPaths = m_panel->getSelectedPaths();
            if (selectedPaths.isEmpty() && !path.isEmpty()) {
                selectedPaths << path;
            }

            if (!selectedPaths.isEmpty()) {
                AppCommand cmd;
                cmd.type = AppCommandType::ToggleFavorite;
                cmd.targetPaths = selectedPaths;
                CoreEngine::instance().executeCommand(cmd);
            }
            break;
        }
        case ContentPanel::ActionCopyName: {
            QModelIndexList indexes = m_panel->getSelectedIndexes();
            QStringList targetPaths;
            for (const auto& idx : indexes) {
                if (idx.column() == 0) {
                    QString p = idx.data(PathRole).toString();
                    if (!p.isEmpty()) targetPaths << p;
                }
            }
            if (targetPaths.isEmpty() && !path.isEmpty()) {
                targetPaths << path;
            }
            ContextMenuFactory::copyNamesToClipboard(targetPaths, true);
            break;
        }
        case ContentPanel::ActionCopyPath: {
            QModelIndexList indexes = m_panel->getSelectedIndexes();
            QStringList targetPaths;
            for (const auto& idx : indexes) {
                if (idx.column() == 0) {
                    QString p = idx.data(PathRole).toString();
                    if (!p.isEmpty()) targetPaths << p;
                }
            }
            if (targetPaths.isEmpty() && !path.isEmpty()) {
                targetPaths << path;
            }
            ContextMenuFactory::copyPathsToClipboard(targetPaths, true);
            break;
        }
        case ContentPanel::ActionRefresh:
            m_panel->refreshAll();
            break;
        case ContentPanel::ActionReextractThumbnail: {
            auto indexes = view->selectionModel()->selectedIndexes();
            QStringList targetPaths;
            for (const auto& idx : indexes) {
                if (idx.column() == 0 && !idx.data(PathRole).toString().isEmpty() && idx.data(TypeRole).toString() != "folder") {
                    targetPaths << idx.data(PathRole).toString();
                }
            }
            if (targetPaths.isEmpty() && !path.isEmpty() && !isFolder) {
                targetPaths << path;
            }

            if (!targetPaths.isEmpty()) {
                ToolTipOverlay::instance()->showToast(QString("正在深度重新提取 %1 个项目的缩略图...").arg(targetPaths.size()), 2000, QColor("#3498db"));

                QPointer<ContentPanel> weakThis(m_panel);
                DeepThumbnailExtractor::instance().extractBatchAsync(
                    targetPaths,
                    [weakThis](const QString& itemPath, bool success) {
                        if (weakThis && success) {
                            weakThis->reloadThumbnailForPath(itemPath);
                        }
                    },
                    [weakThis](int successCount, int totalCount) {
                        if (weakThis) {
                            ToolTipOverlay::instance()->showText(
                                QCursor::pos(),
                                QString("缩略图提取完成：成功 %1 / 总计 %2").arg(successCount).arg(totalCount),
                                2000,
                                successCount > 0 ? QColor("#2ecc71") : QColor("#e81123")
                            );
                        }
                    }
                );
            }
            break;
        }
        default:
            break;
    }
}

} // namespace QuarkMeta
