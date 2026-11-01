cmake_minimum_required(VERSION 3.20)
project(QuarkMeta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 修复：优先探测 QT_DIR 环境变量，若无则使用文档红线默认路径
if(DEFINED ENV{QT_DIR})
    set(CMAKE_PREFIX_PATH "$ENV{QT_DIR}")
else()
    set(CMAKE_PREFIX_PATH "C:/Qt/6.10.2/msvc2022_64")
endif()

# 强制 MSVC 编译器及选项 (文档红线: /W4 /O2)
if(MSVC)
    add_compile_options(/W4 /O2 /utf-8 /Zm1000 /Zi)
    add_link_options(/DEBUG /OPT:REF /OPT:ICF)
endif()

# 全局屏蔽 MSVC 针对标准 C 函数 (如 strcpy 等) 抛出的 C4996 安全警告
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# Qt 6 基础组件
# 2026-05-10 修复：添加缺失的Core和Gui组件
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg Concurrent)

# 2026-03-xx 按照用户要求：禁用 Vulkan 探测以消除日志报错，并清理构建路径矛盾
set(QT_NO_VULKAN ON)

# 路径严谨性核查：若检测到 VS 默认的 out 路径，主动输出警告提示
if(CMAKE_BINARY_DIR MATCHES "/out/")
    message(WARNING "⚠️ 检测到非法构建路径 [out]！请在 VS 工具栏切换至 [x64-release] 预设以符合《源码纯净》规范。")
endif()

# 2026-03-xx 将 QuarkMeta 可执行文件直接输出到项目子目录 QuarkMeta 下
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/QuarkMeta")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/QuarkMeta")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/QuarkMeta")

# 自动生成 MOC/UIC/RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# 2026-06-xx 按照用户要求：彻底废除递归搜集 (GLOB)，改为显式列出所有有效源文件。
# 此举可确保构建系统在彻底移除历史冗余模块后不再包含冗余代码，符合《源码纯净》规范。
set(SOURCES
    src/main.cpp
    src/meta/sqlite3.c
    src/meta/sqlite3.h
    src/meta/QuarkMetaJson.cpp
    src/meta/QuarkMetaJson.h
    src/meta/DiskNavigatorService.cpp
    src/meta/DiskNavigatorService.h
    src/meta/StatisticsService.cpp
    src/meta/StatisticsService.h
    src/meta/MetaCacheDecorator.cpp
    src/meta/MetaCacheDecorator.h
    src/core/CentralEventHub.cpp
    src/core/CentralEventHub.h
    src/core/CoreEngine.cpp
    src/core/CoreEngine.h
    src/core/CoreController.cpp
    src/core/CoreController.h
    src/core/PhysicalDiskSearchExtractor.cpp
    src/core/PhysicalDiskSearchExtractor.h
    src/core/IndexedEntry.cpp
    src/core/IndexedEntry.h
    src/core/ItemRecord.cpp
    src/core/ItemRecord.h
    src/core/DiskScanService.cpp
    src/core/DiskScanService.h
    src/core/FileCreationService.cpp
    src/core/FileCreationService.h
    src/core/FileFilterService.cpp
    src/core/FileFilterService.h
    src/core/VolumeOnlineManager.cpp
    src/core/VolumeOnlineManager.h
    src/core/DiskTrashService.cpp
    src/core/DiskTrashService.h
    src/core/TransferToMarkedCommand.cpp
    src/core/TransferToMarkedCommand.h
    src/core/OperationSnapshotEngine.h
    src/core/OperationSnapshotEngine.cpp
    src/core/TrashService.h
    src/core/TrashService.cpp
    src/core/PermanentDeleteService.h
    src/core/PermanentDeleteService.cpp
    src/core/ClipboardService.h
    src/core/ClipboardService.cpp
    src/core/DeviceWatcher.h
    src/core/DeviceWatcher.cpp
    src/core/DirectoryWatchService.h
    src/core/DirectoryWatchService.cpp
    src/core/TaskProgressService.h
    src/core/TaskProgressService.cpp
    src/core/AppConfig.h
    src/core/NavigationService.cpp
    src/core/NavigationService.h
    src/core/NavigationHistoryService.cpp
    src/core/NavigationHistoryService.h
    src/core/SearchHistoryService.cpp
    src/core/SearchHistoryService.h
    src/core/BatchRenameService.h
    src/core/BatchRenameService.cpp
    src/core/TagLexiconService.h
    src/core/TagLexiconService.cpp
    src/core/PresetTagService.h
    src/core/PresetTagService.cpp
    src/crypto/EncryptionManager.cpp
    src/crypto/EncryptionManager.h
    src/meta/DiskTrashRepo.cpp
    src/meta/DiskTrashRepo.h
    src/meta/DriveMetaDao.cpp
    src/meta/DriveMetaDao.h
    src/meta/TrashRepository.cpp
    src/meta/TrashRepository.h
    src/meta/DatabaseManager.cpp
    src/meta/DatabaseManager.h
    src/meta/ExtensionColorDao.h
    src/meta/ExtensionColorDao.cpp
    src/meta/FavoriteDao.h
    src/meta/FavoriteDao.cpp
    src/meta/HistoryDao.h
    src/meta/HistoryDao.cpp
    src/meta/HistoryService.h
    src/meta/HistoryService.cpp
    src/meta/FavoriteService.h
    src/meta/FavoriteService.cpp
    src/meta/MarkedFolderDao.cpp
    src/meta/MarkedFolderDao.h
    src/meta/MarkedFolderService.cpp
    src/meta/MarkedFolderService.h
    src/meta/ItemMetaMapper.cpp
    src/meta/ItemMetaMapper.h
    src/meta/LibraryDao.h
    src/meta/LibraryDao.cpp
    src/meta/LibraryItemDao.cpp
    src/meta/LibraryItemDao.h
    src/meta/SmartFolderDao.h
    src/meta/SmartFolderDao.cpp
    src/meta/LibraryService.h
    src/meta/LibraryService.cpp
    src/meta/LibrarySyncService.cpp
    src/meta/LibrarySyncService.h
    src/meta/LibraryValidationService.cpp
    src/meta/LibraryValidationService.h
    src/ui/LibraryPanel.h
    src/ui/LibraryPanel.cpp
    src/ui/SidebarContainerWidget.h
    src/ui/SidebarContainerWidget.cpp
    src/meta/DatabaseMigrator.h
    src/meta/BatchRenameEngine.cpp
    src/meta/BatchRenameEngine.h
    src/meta/DuplicateDetectorService.h
    src/meta/DuplicateDetectorService.cpp
    src/meta/MetadataDefs.h
    src/meta/QuarkMetaJsonStore.h
    src/meta/QuarkMetaJsonStore.cpp
    src/meta/MetaIndexService.h
    src/meta/MetaIndexService.cpp
    src/meta/MetaDbRepository.h
    src/meta/MetaDbRepository.cpp
    src/meta/MetaMemoryCache.h
    src/meta/MetaMemoryCache.cpp
    src/meta/MetadataManager.cpp
    src/meta/MetadataManager.h
    src/meta/MediaExtractorPipeline.cpp
    src/meta/MediaExtractorPipeline.h
    src/ui/BatchRenameDialog.cpp
    src/ui/BatchRenameDialog.h
    src/ui/FileCollisionDialog.h
    src/ui/FileCollisionDialog.cpp
    src/ui/PresetManager.cpp
    src/ui/PresetManager.h
    src/ui/UndoToastOverlay.cpp
    src/ui/UndoToastOverlay.h
    src/ui/BreadcrumbBar.cpp
    src/ui/BreadcrumbBar.h
    src/ui/ColorPicker.cpp
    src/ui/ColorPicker.h
    src/ui/ContentPanel.cpp
    src/ui/ContentPanel.h
    src/ui/ViewDragDropHelper.cpp
    src/ui/ViewDragDropHelper.h
    src/ui/DropTreeView.cpp
    src/ui/DropTreeView.h
    src/ui/DropListView.cpp
    src/ui/DropListView.h
    src/ui/IScanResultView.h
    src/ui/FilterStateModel.h
    src/ui/FilterStateModel.cpp
    src/ui/models/FilterProxyModel.h
    src/ui/models/FilterProxyModel.cpp
    src/ui/models/SectionProxyModel.h
    src/ui/models/SectionProxyModel.cpp
    src/ui/ScanStatsEngine.h
    src/ui/ScanStatsEngine.cpp
    src/ui/FilterPanel.cpp
    src/ui/FilterPanel.h
    src/ui/ThumbnailStatusGroup.cpp
    src/ui/ThumbnailStatusGroup.h
    src/ui/DuplicateStatusGroup.cpp
    src/ui/DuplicateStatusGroup.h
    src/ui/LinkStatusGroup.cpp
    src/ui/LinkStatusGroup.h
    src/ui/NoteStatusGroup.cpp
    src/ui/NoteStatusGroup.h
    src/ui/TagStatusGroup.cpp
    src/ui/TagStatusGroup.h
    src/ui/AspectRatioGroup.cpp
    src/ui/AspectRatioGroup.h
    src/ui/FileSizeGroup.cpp
    src/ui/FileSizeGroup.h
    src/ui/AutoColorGroup.cpp
    src/ui/AutoColorGroup.h
    src/ui/ColorLabelGroup.cpp
    src/ui/ColorLabelGroup.h
    src/ui/RatingGroup.cpp
    src/ui/RatingGroup.h
    src/ui/FileTypeGroup.cpp
    src/ui/FileTypeGroup.h
    src/ui/CreateDateGroup.cpp
    src/ui/CreateDateGroup.h
    src/ui/ModifyDateGroup.cpp
    src/ui/ModifyDateGroup.h
    src/ui/FramelessDialogBase.h
    src/ui/FramelessDialog.cpp
    src/ui/FramelessDialog.h
    src/ui/FramelessFileDialog.cpp
    src/ui/FramelessFileDialog.h
    src/ui/JustifiedView.cpp
    src/ui/JustifiedView.h
    src/ui/DropJustifiedView.cpp
    src/ui/DropJustifiedView.h
    src/ui/ElidedTextUtility.h
    src/ui/CardPainterHelper.cpp
    src/ui/CardPainterHelper.h
    src/ui/ThumbnailDelegate.cpp
    src/ui/ThumbnailDelegate.h
    src/ui/Logger.h
    src/ui/AppShortcutController.h
    src/ui/AppShortcutController.cpp
    src/ui/LocalShortcutInterceptor.h
    src/ui/LocalShortcutInterceptor.cpp
    src/ui/PanelMediator.h
    src/ui/PanelMediator.cpp
    src/ui/MainWindow.cpp
    src/ui/MainWindow.h
    src/ui/DriveBarWidget.h
    src/ui/DriveBarWidget.cpp
    src/ui/NavBarWidget.h
    src/ui/NavBarWidget.cpp
    src/ui/TabBarWidget.h
    src/ui/TabBarWidget.cpp
    src/ui/TitleBarWidget.h
    src/ui/TitleBarWidget.cpp
    src/ui/ContentHeaderWidget.h
    src/ui/ContentHeaderWidget.cpp
    src/ui/SearchController.h
    src/ui/SearchController.cpp
    src/ui/TrayController.cpp
    src/ui/TrayController.h
    src/ui/HoverEventFilter.cpp
    src/ui/HoverEventFilter.h
    src/ui/FramelessWindowHelper.cpp
    src/ui/FramelessWindowHelper.h
    src/ui/PanelLayoutManager.cpp
    src/ui/PanelLayoutManager.h
    src/ui/AddressBar.cpp
    src/ui/AddressBar.h
    src/ui/MetaPanel.cpp
    src/ui/MetaPanel.h
    src/ui/TagSelectorOverlay.h
    src/ui/TagSelectorOverlay.cpp
    src/ui/PresetTagsDialog.h
    src/ui/PresetTagsDialog.cpp
    src/ui/TaskProgressStrip.h
    src/ui/TaskProgressStrip.cpp
    src/ui/components/ElasticEdit.h
    src/ui/components/ElasticEdit.cpp
    src/ui/components/TagPill.h
    src/ui/components/TagPill.cpp
    src/ui/components/FlowLayout.h
    src/ui/components/FlowLayout.cpp
    src/ui/components/ColorPill.h
    src/ui/components/ColorPill.cpp
    src/ui/components/ColorWheel.h
    src/ui/components/ColorWheel.cpp
    src/ui/components/InlineHueSlider.cpp
    src/ui/components/InlineHueSlider.h
    src/ui/components/ColorBlock.cpp
    src/ui/components/ColorBlock.h
    src/ui/NavPanel.cpp
    src/ui/NavPanel.h
    src/ui/FavoritePanel.cpp
    src/ui/FavoritePanel.h
    src/ui/MarkedPanel.cpp
    src/ui/MarkedPanel.h
    src/ui/HistoryPanel.h
    src/ui/HistoryPanel.cpp
    src/ui/QuickLookWindow.cpp
    src/ui/QuickLookWindow.h
    src/ui/QuickLookMinimap.cpp
    src/ui/QuickLookMinimap.h
    src/ui/RuleRow.cpp
    src/ui/RuleRow.h
    src/ui/CreateRuleRow.cpp
    src/ui/CreateRuleRow.h
    src/ui/BatchCreateDialog.cpp
    src/ui/BatchCreateDialog.h
    src/ui/TagManagerDialog.cpp
    src/ui/TagManagerDialog.h
    src/ui/SearchHistoryPanel.cpp
    src/ui/SearchHistoryPanel.h
    src/ui/AddressHistoryPanel.cpp
    src/ui/AddressHistoryPanel.h
    src/ui/TagManagerController.cpp
    src/ui/TagManagerController.h
    src/ui/controllers/ContentContextMenu.h
    src/ui/controllers/ContentContextMenu.cpp
    src/ui/controllers/ContextMenuFactory.h
    src/ui/controllers/ContextMenuFactory.cpp
    src/ui/controllers/ContentKeyHandler.h
    src/ui/controllers/ContentKeyHandler.cpp
    src/ui/controllers/ContentSortController.h
    src/ui/controllers/ContentSortController.cpp
    src/ui/controllers/ContentDataLoader.h
    src/ui/controllers/ContentDataLoader.cpp
    src/ui/controllers/ContentFileOpsHandler.h
    src/ui/controllers/ContentFileOpsHandler.cpp
    src/ui/controllers/ContentViewCoordinator.h
    src/ui/controllers/ContentViewCoordinator.cpp
    src/ui/controllers/ContentPaneSplitManager.h
    src/ui/controllers/ContentPaneSplitManager.cpp
    src/ui/controllers/ContentChangeWatcher.h
    src/ui/controllers/ContentChangeWatcher.cpp
    src/ui/controllers/PaneActivationTracker.h
    src/ui/controllers/PaneActivationTracker.cpp
    src/ui/workers/ContentStatsWorker.h
    src/ui/workers/ContentStatsWorker.cpp
    src/core/ActionCommand.h
    src/core/UndoManager.h
    src/core/BasicCommands.h
    src/core/commands/RenameCommand.h
    src/core/commands/MoveCommand.h
    src/core/commands/MetadataCommand.h
    src/core/commands/SecureDeleteCommand.h
    src/core/commands/ShellProtectionCommand.h
    src/core/commands/BatchRenameCommand.h
    src/util/VolumePathResolver.h
    src/util/VolumePathResolver.cpp
    src/ui/FolderButton.h
    src/ui/FolderButton.cpp
    src/ui/QuickLookGraphicsView.h
    src/ui/QuickLookGraphicsView.cpp
    src/ui/components/StyledCheckBox.h
    src/ui/components/StyledCheckBox.cpp
    src/ui/components/ClickableRow.h
    src/ui/components/ClickableRow.cpp
    src/ui/dialogs/FramelessInputDialog.h
    src/ui/dialogs/FramelessInputDialog.cpp
    src/ui/dialogs/TextExtensionDialog.h
    src/ui/dialogs/TextExtensionDialog.cpp
    src/ui/dialogs/CacheLocationDialog.h
    src/ui/dialogs/CacheLocationDialog.cpp
    src/ui/dialogs/CreateFolderWithItemsDialog.h
    src/ui/dialogs/CreateFolderWithItemsDialog.cpp
    src/ui/TagPillContainer.h
    src/ui/TagPillContainer.cpp
    src/ui/dialogs/SmartFolderValueWidgets.h
    src/ui/dialogs/SmartFolderValueWidgets.cpp
    src/ui/dialogs/SmartFolderRuleRow.h
    src/ui/dialogs/SmartFolderRuleRow.cpp
    src/ui/dialogs/SmartFolderDialog.h
    src/ui/dialogs/SmartFolderDialog.cpp
    src/ui/dialogs/FileCompareDialog.h
    src/ui/dialogs/FileCompareDialog.cpp
    src/ui/dialogs/FramelessColorPicker.h
    src/ui/dialogs/FramelessColorPicker.cpp
    src/ui/dialogs/FramelessConfirmDialog.h
    src/ui/dialogs/FramelessConfirmDialog.cpp
    src/ui/dialogs/FramelessMessageBox.h
    src/ui/dialogs/FramelessMessageBox.cpp
    src/ui/CardLayoutEngine.h
    src/ui/CardLayoutEngine.cpp
    src/ui/RowLayoutEngine.h
    src/ui/RowLayoutEngine.cpp
    src/ui/RatingBarLayout.h
    src/ui/RatingBarLayout.cpp
    src/ui/ToolTipOverlay.cpp
    src/ui/ToolTipOverlay.h
    src/ui/TreeItemDelegate.h
    src/ui/FileNameLineEdit.h
    src/ui/FileNameLineEdit.cpp
    src/ui/ColumnItemDelegate.h
    src/ui/ColumnItemDelegate.cpp
    src/ui/ColumnBlankCanvasWidget.h
    src/ui/ColumnBlankCanvasWidget.cpp
    src/ui/ColumnViewPane.h
    src/ui/ColumnViewPane.cpp
    src/ui/ColumnViewWidget.h
    src/ui/ColumnViewWidget.cpp
    src/ui/RenameCapableDelegate.h
    src/ui/RenameCapableDelegate.cpp
    src/ui/models/ItemModelBase.h
    src/ui/models/DiskItemModel.h
    src/ui/models/DiskItemModel.cpp
    src/ui/ThemeManager.h
    src/ui/ThemeManager.cpp
    src/ui/QuarkApplication.h
    src/ui/QuarkApplication.cpp
    src/ui/UiHelper.h
    src/ui/UiHelper.cpp
    src/ui/IconCacheManager.h
    src/ui/IconCacheManager.cpp
    src/ui/ShellIconManager.h
    src/util/ColorPaletteEngine.h
    src/util/ColorPaletteEngine.cpp
    src/util/ThumbnailPipelineService.h
    src/util/ThumbnailPipelineService.cpp
    src/ui/SvgIconRenderer.h
    src/ui/SvgIconRenderer.cpp
    src/ui/WindowsShellThumbnailProvider.h
    src/ui/WindowsShellThumbnailProvider.cpp
    src/ui/FormatDecoders.h
    src/ui/FormatDecoders.cpp
    src/ui/ImageDecoderFacade.h
    src/ui/ImageDecoderFacade.cpp
    src/util/DiskMediaExtractor.h
    src/util/DiskMediaExtractor.cpp
    src/util/DeepThumbnailExtractor.h
    src/util/DeepThumbnailExtractor.cpp
    src/util/ShellHelper.cpp
    src/util/ShellHelper.h
    src/util/HardwareInfoHelper.cpp
    src/util/HardwareInfoHelper.h
    QuarkMeta.rc
    resources/resources.qrc
)

# 新增：libtiff 核心源码（排除 JPEG/JBIG/LZMA/WebP/zstd/LERC 等需要额外第三方库的部分，
# 排除 Unix 平台文件，排除 C++ 封装层，排除工具/构建脚本）
set(LIBTIFF_SOURCES
    src/third_party/libtiff/tif_aux.c
    src/third_party/libtiff/tif_close.c
    src/third_party/libtiff/tif_codec.c
    src/third_party/libtiff/tif_color.c
    src/third_party/libtiff/tif_compress.c
    src/third_party/libtiff/tif_dir.c
    src/third_party/libtiff/tif_dirinfo.c
    src/third_party/libtiff/tif_dirread.c
    src/third_party/libtiff/tif_dirwrite.c
    src/third_party/libtiff/tif_dumpmode.c
    src/third_party/libtiff/tif_error.c
    src/third_party/libtiff/tif_extension.c
    src/third_party/libtiff/tif_fax3.c
    src/third_party/libtiff/tif_fax3sm.c
    src/third_party/libtiff/tif_flush.c
    src/third_party/libtiff/tif_getimage.c
    src/third_party/libtiff/tif_hash_set.c
    src/third_party/libtiff/tif_luv.c
    src/third_party/libtiff/tif_lzw.c
    src/third_party/libtiff/tif_next.c
    src/third_party/libtiff/tif_ojpeg.c
    src/third_party/libtiff/tif_open.c
    src/third_party/libtiff/tif_packbits.c
    src/third_party/libtiff/tif_pixarlog.c
    src/third_party/libtiff/tif_predict.c
    src/third_party/libtiff/tif_print.c
    src/third_party/libtiff/tif_read.c
    src/third_party/libtiff/tif_strip.c
    src/third_party/libtiff/tif_swab.c
    src/third_party/libtiff/tif_thunder.c
    src/third_party/libtiff/tif_tile.c
    src/third_party/libtiff/tif_version.c
    src/third_party/libtiff/tif_warning.c
    src/third_party/libtiff/tif_win32.c
    src/third_party/libtiff/tif_write.c
    src/third_party/libtiff/tif_zip.c
)
list(APPEND SOURCES ${LIBTIFF_SOURCES})

# 可执行文件 (添加 WIN32 属性以消除控制台黑框)
# 修复：改由 QuarkMeta.rc 引入以杜绝 mt.exe 冲突
add_executable(QuarkMeta WIN32 ${SOURCES})

# 链接库 (文档红线列表: ole32, bcrypt)
# 2026-04-14 按照用户要求：恢复 MFT 逻辑，重新引入 ntdll 依赖
# 2026-05-10 修复：添加缺失的Qt6::Core和Qt6::Gui库
target_link_libraries(QuarkMeta PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
    Qt6::Concurrent
    ntdll
    ole32
    bcrypt
    psapi
)

target_include_directories(QuarkMeta PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/meta
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/crypto
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util
    ${CMAKE_CURRENT_SOURCE_DIR}/src/meta
    ${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/libtiff
)

# 2026-07-20 按照用户最新要求：
# 彻底去除强制管理员权限，改为普通权限 (asInvoker)
if(MSVC)
    target_link_options(QuarkMeta PRIVATE "/MANIFESTUAC:level='asInvoker' uiAccess='false'")
endif()
# Force Rebuild TagLexiconService: 2026-08-28 19:40
