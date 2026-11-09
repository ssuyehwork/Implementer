cmake_minimum_required(VERSION 3.16)

# 启用现代 MSVC 运行时库策略 (彻底解决 /MT 与 /MD 混用冲突)
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

project(RapidNotes VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 全局设置 MSVC 为静态运行时库 (/MT Release, /MTd Debug)，匹配静态 Qt6 (msvc2022_64_static)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# 2026-03-xx 按照用户要求：彻底废除 MinGW 支持，锁定 MSVC (Visual Studio 2022) 环境
if(NOT MSVC)
    message(FATAL_ERROR "🔴 [编译器拦截] 此项目已废除 MinGW 支持，必须且只能使用 Visual Studio 2022 (MSVC) 进行编译。")
endif()

# 2026-03-xx 按照用户要求：MSVC/VS2022 专属编译环境深度优化
add_compile_options(/utf-8)       # 解决中文字符集乱码
add_compile_options(/MP)          # 启用多处理器并行编译，榨干 CPU 性能
# [FIX] 2026-03-xx 移除 /permissive- 以兼容 Windows SDK/WinRT 的内部重定义冲突

# 屏蔽 VS 的非标准警告并优化 Windows 头文件包含
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
# [FIX] 2026-03-xx 移除 WIN32_LEAN_AND_MEAN 以确保 COM/WinRT 所需的底层头文件完整加载
add_definitions(-DNOMINMAX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 2026-03-xx 按照用户要求：增加环境自检提示，协助解决 Qt6 路径未找到问题
if(NOT Qt6_DIR AND NOT CMAKE_PREFIX_PATH)
    message(STATUS "🔍 [环境检测] 正在尝试从默认路径定位 Qt6...")
    set(CMAKE_PREFIX_PATH "C:/Qt/6.10.2/msvc2022_64")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql Network Concurrent Svg)

# 2026-03-xx 按照用户要求：重新启用日志输出，配合本地日志系统使用
# add_definitions(-DQT_NO_DEBUG_OUTPUT)
# add_definitions(-DQT_NO_INFO_OUTPUT)
# add_definitions(-DQT_NO_WARNING_OUTPUT)

# 遵循“三权分立”规范：将生成的 exe 输出到固化运行层 RapidNotes_Build/RapidNotes
set(TARGET_APP_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/../../RapidNotes_Build/RapidNotes")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${TARGET_APP_OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${TARGET_APP_OUTPUT_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${TARGET_APP_OUTPUT_DIR}")

# 强制重写各构建类型变量，防止 VS 多配置生成器自动追加 Release/Debug 子目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${TARGET_APP_OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${TARGET_APP_OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${TARGET_APP_OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${TARGET_APP_OUTPUT_DIR}")

include_directories(
    src
    src/ui
    src/ui/models
    src/ui/components
    src/ui/controllers
    src/core
    src/util
    src/meta
)

# 源代码列表
set(SOURCES
    src/core/AES.cpp
    src/core/AES.h
    src/core/AppConfig.h
    src/core/ActionRecorder.cpp
    src/core/ActionRecorder.h
    src/core/CentralEventHub.cpp
    src/core/CentralEventHub.h
    src/core/ClipboardMonitor.cpp
    src/core/ClipboardMonitor.h
    src/core/ClipboardService.cpp
    src/core/ClipboardService.h
    src/core/CoreEngine.cpp
    src/core/CoreEngine.h
    src/core/DatabaseManager.cpp
    src/core/DatabaseManager.h
    src/core/FileCryptoHelper.cpp
    src/core/FileCryptoHelper.h
    src/core/FileStorageHelper.cpp
    src/core/FileStorageHelper.h
    src/core/HardwareInfoHelper.cpp
    src/core/HardwareInfoHelper.h
    src/core/HotkeyManager.cpp
    src/core/HotkeyManager.h
    src/core/HttpServer.cpp
    src/core/HttpServer.h
    src/core/ItemClassifier.cpp
    src/core/ItemClassifier.h
    src/core/ItemRecord.cpp
    src/core/ItemRecord.h
    src/core/ItemType.cpp
    src/core/ItemType.h
    src/core/KeyboardHook.cpp
    src/core/KeyboardHook.h
    src/core/Logger.cpp
    src/core/Logger.h
    src/core/MessageCaptureHandler.cpp
    src/core/MessageCaptureHandler.h
    src/core/ModelContract.h
    src/meta/FavoriteDao.h
    src/meta/FavoriteService.h
    src/meta/LibrarySyncService.h
    src/meta/LibraryDao.h
    src/meta/LibraryService.cpp
    src/meta/LibraryService.h
    src/util/ShellHelper.h
    src/core/NavigationHistoryService.cpp
    src/core/NavigationHistoryService.h
    src/core/NavigationService.cpp
    src/core/NavigationService.h
    src/core/OCRManager.cpp
    src/core/OCRManager.h
    src/core/PathSummary.cpp
    src/core/PathSummary.h
    src/core/SearchHistoryService.cpp
    src/core/SearchHistoryService.h
    src/core/ReminderService.cpp
    src/core/ReminderService.h
    src/core/ShortcutManager.cpp
    src/core/ShortcutManager.h
    src/core/TagCodec.cpp
    src/core/TagCodec.h
    src/core/TaskProgressService.cpp
    src/core/TaskProgressService.h
    src/core/TrashService.cpp
    src/core/TrashService.h
    src/core/PermanentDeleteService.cpp
    src/core/PermanentDeleteService.h
    src/core/ThumbnailMaker.cpp
    src/core/ThumbnailMaker.h
    src/core/WordBucket.cpp
    src/core/WordBucket.h
    src/main.cpp
    src/models/CategoryModel.cpp
    src/models/CategoryModel.h
    src/models/NoteModel.cpp
    src/models/NoteModel.h
    src/ui/ActivationDialog.cpp
    src/ui/ActivationDialog.h
    src/ui/AddressBar.cpp
    src/ui/AddressBar.h
    src/ui/AddressHistoryPanel.cpp
    src/ui/AddressHistoryPanel.h
    src/ui/AdvancedTagSelector.cpp
    src/ui/AdvancedTagSelector.h
    src/ui/AppShortcutController.cpp
    src/ui/AppShortcutController.h
    src/ui/BreadcrumbBar.cpp
    src/ui/BreadcrumbBar.h
    src/ui/CardLayoutEngine.cpp
    src/ui/CardLayoutEngine.h
    src/ui/CardPainterHelper.cpp
    src/ui/CardPainterHelper.h
    src/ui/CategoryDelegate.h
    src/ui/CategoryLockWidget.cpp
    src/ui/CategoryLockWidget.h
    src/ui/CategoryPasswordDialog.cpp
    src/ui/CategoryPasswordDialog.h
    src/ui/CleanListView.cpp
    src/ui/CleanListView.h
    src/ui/ClickableLineEdit.h
    src/ui/ColorPickerWindow.cpp
    src/ui/ColorPickerWindow.h
    src/ui/ContentHeaderWidget.cpp
    src/ui/ContentHeaderWidget.h
    src/ui/components/ClickableRow.cpp
    src/ui/components/ClickableRow.h
    src/ui/components/ColorBlock.cpp
    src/ui/components/ColorBlock.h
    src/ui/components/ColorDot.cpp
    src/ui/components/ColorDot.h
    src/ui/components/ColorPill.cpp
    src/ui/components/ColorPill.h
    src/ui/components/ColorWheel.cpp
    src/ui/components/ColorWheel.h
    src/ui/components/ElasticEdit.cpp
    src/ui/components/ElasticEdit.h
    src/ui/components/FlowLayout.cpp
    src/ui/components/FlowLayout.h
    src/ui/components/InlineHueSlider.cpp
    src/ui/components/InlineHueSlider.h
    src/ui/components/StyledCheckBox.cpp
    src/ui/components/StyledCheckBox.h
    src/ui/components/TagPill.cpp
    src/ui/components/TagPill.h
    src/ui/ContentPanel.cpp
    src/ui/ContentPanel.h
    src/ui/DropJustifiedView.cpp
    src/ui/DropJustifiedView.h
    src/ui/DropListView.cpp
    src/ui/DropListView.h
    src/ui/DropTreeView.cpp
    src/ui/DropTreeView.h
    src/ui/Editor.cpp
    src/ui/Editor.h
    src/ui/FileNameLineEdit.cpp
    src/ui/FileNameLineEdit.h
    src/ui/FileSearchHistoryPopup.h
    src/ui/FileSearchWidget.cpp
    src/ui/FileSearchWidget.h
    src/ui/FileSearchWindow.cpp
    src/ui/FileSearchWindow.h
    src/ui/FileStorageWindow.cpp
    src/ui/FileStorageWindow.h
    src/ui/ColorPicker.cpp
    src/ui/ColorPicker.h
    src/ui/ColorLabelGroup.cpp
    src/ui/ColorLabelGroup.h
    src/ui/CreateDateGroup.cpp
    src/ui/CreateDateGroup.h
    src/ui/FileTypeGroup.cpp
    src/ui/FileTypeGroup.h
    src/ui/FilterPanel.cpp
    src/ui/FilterPanel.h
    src/ui/SmartFolderDialog.h
    src/core/PresetTagService.h
    src/ui/QuickLookWindow.h
    src/ui/ModifyDateGroup.cpp
    src/ui/ModifyDateGroup.h
    src/ui/NoteStatusGroup.cpp
    src/ui/NoteStatusGroup.h
    src/ui/RatingGroup.cpp
    src/ui/RatingGroup.h
    src/ui/RatingBarLayout.cpp
    src/ui/RatingBarLayout.h
    src/ui/PresetTagsDialog.cpp
    src/ui/PresetTagsDialog.h
    src/ui/TagPillContainer.cpp
    src/ui/TagPillContainer.h
    src/ui/TagStatusGroup.cpp
    src/ui/TagStatusGroup.h
    src/ui/FilterStateModel.cpp
    src/ui/FilterStateModel.h
    src/ui/FireworksOverlay.cpp
    src/ui/FireworksOverlay.h
    src/ui/FloatingBall.cpp
    src/ui/FloatingBall.h
    src/ui/FlowLayout.cpp
    src/ui/FlowLayout.h
    src/ui/FramelessDialog.cpp
    src/ui/FramelessDialog.h
    src/ui/FramelessWindowHelper.cpp
    src/ui/FramelessWindowHelper.h
    src/ui/HelpWindow.cpp
    src/ui/HelpWindow.h
    src/ui/HoverEventFilter.cpp
    src/ui/HoverEventFilter.h
    src/ui/IconHelper.h
    src/ui/IdeaQuickPanel.cpp
    src/ui/IdeaQuickPanel.h
    src/ui/IntegratedSearchWindow.cpp
    src/ui/IntegratedSearchWindow.h
    src/ui/JustifiedView.cpp
    src/ui/JustifiedView.h
    src/ui/KeywordSearchWidget.cpp
    src/ui/KeywordSearchWidget.h
    src/ui/KeywordSearchWindow.cpp
    src/ui/KeywordSearchWindow.h
    src/ui/LibraryPanel.cpp
    src/ui/LibraryPanel.h
    src/ui/LocalShortcutInterceptor.cpp
    src/ui/LocalShortcutInterceptor.h
    src/ui/MainWindow.cpp
    src/ui/MainWindow.h
    src/ui/MetaPanel.cpp
    src/ui/MetaPanel.h
    src/ui/NavBarWidget.cpp
    src/ui/NavBarWidget.h
    src/ui/NoteDelegate.h
    src/ui/NoteEditWindow.cpp
    src/ui/NoteEditWindow.h
    src/ui/OCRResultWindow.cpp
    src/ui/OCRResultWindow.h
    src/ui/OCRWindow.cpp
    src/ui/OCRWindow.h
    src/ui/PanelLayoutManager.cpp
    src/ui/PanelLayoutManager.h
    src/ui/PanelMediator.cpp
    src/ui/PanelMediator.h
    src/ui/PasswordGeneratorWindow.cpp
    src/ui/PasswordGeneratorWindow.h
    src/ui/PasswordVerifyDialog.cpp
    src/ui/PasswordVerifyDialog.h
    src/ui/PixelRulerOverlay.cpp
    src/ui/PixelRulerOverlay.h
    src/ui/QuickNoteDelegate.cpp
    src/ui/QuickNoteDelegate.h
    src/ui/QuickPreview.h
    src/ui/QuickWindow.cpp
    src/ui/QuickWindow.h
    src/ui/RecentNotesDelegate.cpp
    src/ui/RecentNotesDelegate.h
    src/ui/RecentNotesWindow.cpp
    src/ui/RecentNotesWindow.h
    src/ui/RenameCapableDelegate.cpp
    src/ui/RenameCapableDelegate.h
    src/ui/ResizeHandle.cpp
    src/ui/ResizeHandle.h
    src/ui/RowLayoutEngine.cpp
    src/ui/RowLayoutEngine.h
    src/ui/ScanStats.h
    src/ui/ScanStatsEngine.cpp
    src/ui/ScanStatsEngine.h
    src/ui/ScreenshotTool.cpp
    src/ui/ScreenshotTool.h
    src/ui/SearchAppWindow.cpp
    src/ui/SearchAppWindow.h
    src/ui/SearchController.cpp
    src/ui/SearchController.h
    src/ui/SearchHistoryPanel.cpp
    src/ui/SearchHistoryPanel.h
    src/ui/SearchLineEdit.cpp
    src/ui/SearchLineEdit.h
    src/ui/SecurityLockDialog.cpp
    src/ui/SecurityLockDialog.h
    src/ui/SettingsBarWidget.cpp
    src/ui/SettingsBarWidget.h
    src/ui/dialogs/TextExtensionDialog.cpp
    src/ui/dialogs/TextExtensionDialog.h
    src/ui/dialogs/CacheLocationDialog.cpp
    src/ui/dialogs/CacheLocationDialog.h
    src/ui/SettingsWindow.cpp
    src/ui/SettingsWindow.h
    src/ui/SidebarContainerWidget.cpp
    src/ui/SidebarContainerWidget.h
    src/ui/StringUtils.h
    src/ui/SvgIcons.h
    src/ui/SystemTray.cpp
    src/ui/SystemTray.h
    src/ui/TabBarWidget.cpp
    src/ui/TabBarWidget.h
    src/ui/TagCapsule.h
    src/ui/TagEditDialog.cpp
    src/ui/TagEditDialog.h
    src/ui/TagEditorWidget.cpp
    src/ui/TagEditorWidget.h
    src/ui/TagManagerWindow.cpp
    src/ui/TagManagerWindow.h
    src/ui/TagSelectorOverlay.cpp
    src/ui/TagSelectorOverlay.h
    src/ui/TaskProgressStrip.cpp
    src/ui/TaskProgressStrip.h
    src/ui/ThumbnailDelegate.cpp
    src/ui/ThumbnailDelegate.h
    src/ui/TimePasteWindow.cpp
    src/ui/TimePasteWindow.h
    src/ui/StyleLibrary.h
    src/ui/SvgIconRenderer.cpp
    src/ui/SvgIconRenderer.h
    src/ui/ThemeManager.cpp
    src/ui/ThemeManager.h
    src/ui/TitleBarWidget.cpp
    src/ui/TitleBarWidget.h
    src/ui/UiHelper.cpp
    src/ui/UiHelper.h
    src/util/ColorPaletteEngine.cpp
    src/util/ColorPaletteEngine.h
    src/ui/TitleEditorDialog.h
    src/ui/TodoCalendarWindow.cpp
    src/ui/TodoCalendarWindow.h
    src/ui/ToolTipOverlay.h
    src/ui/TrayController.cpp
    src/ui/TrayController.h
    src/ui/TreeItemDelegate.h
    src/ui/UnifiedSearchWindow.cpp
    src/ui/UnifiedSearchWindow.h
    src/ui/ViewDragDropHelper.cpp
    src/ui/ViewDragDropHelper.h
    src/ui/WritingAnimation.cpp
    src/ui/WritingAnimation.h
    src/ui/controllers/ContentChangeWatcher.cpp
    src/ui/controllers/ContentChangeWatcher.h
    src/ui/controllers/ContentDataLoader.cpp
    src/ui/controllers/ContentDataLoader.h
    src/ui/controllers/ContentFileOpsHandler.cpp
    src/ui/controllers/ContentFileOpsHandler.h
    src/ui/controllers/ContentKeyHandler.cpp
    src/ui/controllers/ContentKeyHandler.h
    src/ui/controllers/ContentPaneSplitManager.cpp
    src/ui/controllers/ContentPaneSplitManager.h
    src/ui/controllers/ContentViewCoordinator.cpp
    src/ui/controllers/ContentViewCoordinator.h
    src/ui/controllers/ContentSortController.cpp
    src/ui/controllers/ContentSortController.h
    src/ui/controllers/ContentContextMenu.cpp
    src/ui/controllers/ContentContextMenu.h
    src/ui/controllers/ContextMenuFactory.cpp
    src/ui/controllers/ContextMenuFactory.h
    src/ui/controllers/PaneActivationTracker.cpp
    src/ui/controllers/PaneActivationTracker.h
    src/ui/models/DiskItemModel.h
    src/ui/models/FilterProxyModel.cpp
    src/ui/models/FilterProxyModel.h
    src/ui/models/ItemModelBase.h
    src/ui/models/SectionProxyModel.cpp
    src/ui/models/SectionProxyModel.h
    src/ui/workers/ContentStatsWorker.cpp
    src/ui/workers/ContentStatsWorker.h
    src/core/ActionCommand.h
    src/core/BasicCommands.h
    src/core/BatchRenameService.cpp
    src/core/BatchRenameService.h
    src/core/CoreController.cpp
    src/core/CoreController.h
    src/core/DeviceWatcher.cpp
    src/core/DeviceWatcher.h
    src/core/DirectoryWatchService.cpp
    src/core/DirectoryWatchService.h
    src/core/DiskScanService.cpp
    src/core/DiskScanService.h
    src/core/DiskTrashService.cpp
    src/core/DiskTrashService.h
    src/core/FileCreationService.cpp
    src/core/FileCreationService.h
    src/core/FileFilterService.cpp
    src/core/FileFilterService.h
    src/core/IndexedEntry.cpp
    src/core/IndexedEntry.h
    src/core/LastOperationManager.h
    src/core/OperationSnapshotEngine.cpp
    src/core/OperationSnapshotEngine.h
    src/core/PhysicalDiskSearchExtractor.cpp
    src/core/PhysicalDiskSearchExtractor.h
    src/core/PresetTagService.cpp
    src/core/TagLexiconService.cpp
    src/core/TagLexiconService.h
    src/core/TransferToMarkedCommand.cpp
    src/core/TransferToMarkedCommand.h
    src/core/UndoManager.h
    src/core/VolumeOnlineManager.cpp
    src/core/VolumeOnlineManager.h
    src/core/commands/BatchRenameCommand.h
    src/core/commands/MetadataCommand.h
    src/core/commands/MoveCommand.h
    src/core/commands/RenameCommand.h
    src/core/commands/SecureDeleteCommand.h
    src/core/commands/ShellProtectionCommand.h
    src/crypto/EncryptionManager.cpp
    src/crypto/EncryptionManager.h
    src/meta/AssociationCascadeService.cpp
    src/meta/AssociationCascadeService.h
    src/meta/BatchRenameEngine.cpp
    src/meta/BatchRenameEngine.h
    src/meta/DatabaseManager.cpp
    src/meta/DatabaseManager.h
    src/meta/DatabaseMigrator.h
    src/meta/DiskNavigatorService.cpp
    src/meta/DiskNavigatorService.h
    src/meta/DiskTrashRepo.cpp
    src/meta/DiskTrashRepo.h
    src/meta/DriveMetaDao.cpp
    src/meta/DriveMetaDao.h
    src/meta/DuplicateDetectorService.cpp
    src/meta/DuplicateDetectorService.h
    src/meta/ExtensionColorDao.cpp
    src/meta/ExtensionColorDao.h
    src/meta/FavoriteDao.cpp
    src/meta/FavoriteService.cpp
    src/meta/FileOperationHelper.h
    src/meta/HistoryDao.cpp
    src/meta/HistoryDao.h
    src/meta/HistoryService.cpp
    src/meta/HistoryService.h
    src/meta/ItemMetaMapper.cpp
    src/meta/ItemMetaMapper.h
    src/meta/LibraryDao.cpp
    src/meta/LibraryItemDao.cpp
    src/meta/LibraryItemDao.h
    src/meta/LibrarySyncService.cpp
    src/meta/LibraryValidationService.cpp
    src/meta/LibraryValidationService.h
    src/meta/MarkedFolderDao.cpp
    src/meta/MarkedFolderDao.h
    src/meta/MarkedFolderService.cpp
    src/meta/MarkedFolderService.h
    src/meta/MediaExtractorPipeline.cpp
    src/meta/MediaExtractorPipeline.h
    src/meta/MetaCacheDecorator.cpp
    src/meta/MetaCacheDecorator.h
    src/meta/MetaDbRepository.cpp
    src/meta/MetaDbRepository.h
    src/meta/MetaIndexService.cpp
    src/meta/MetaIndexService.h
    src/meta/MetaMemoryCache.cpp
    src/meta/MetaMemoryCache.h
    src/meta/MetadataDefs.h
    src/meta/MetadataManager.cpp
    src/meta/MetadataManager.h
    src/meta/SmartFolderDao.cpp
    src/meta/SmartFolderDao.h
    src/meta/StatisticsService.cpp
    src/meta/StatisticsService.h
    src/meta/TrashRepository.cpp
    src/meta/TrashRepository.h
    src/meta/sqlite3.c
    src/meta/sqlite3.h
    src/meta/sqlite3ext.h
    src/third_party/libtiff/t4.h
    src/third_party/libtiff/tif_aux.c
    src/third_party/libtiff/tif_close.c
    src/third_party/libtiff/tif_codec.c
    src/third_party/libtiff/tif_color.c
    src/third_party/libtiff/tif_compress.c
    src/third_party/libtiff/tif_config.h
    src/third_party/libtiff/tif_dir.c
    src/third_party/libtiff/tif_dir.h
    src/third_party/libtiff/tif_dirinfo.c
    src/third_party/libtiff/tif_dirread.c
    src/third_party/libtiff/tif_dirwrite.c
    src/third_party/libtiff/tif_dumpmode.c
    src/third_party/libtiff/tif_error.c
    src/third_party/libtiff/tif_extension.c
    src/third_party/libtiff/tif_fax3.c
    src/third_party/libtiff/tif_fax3.h
    src/third_party/libtiff/tif_fax3sm.c
    src/third_party/libtiff/tif_flush.c
    src/third_party/libtiff/tif_getimage.c
    src/third_party/libtiff/tif_hash_set.c
    src/third_party/libtiff/tif_hash_set.h
    src/third_party/libtiff/tif_jpeg.c
    src/third_party/libtiff/tif_luv.c
    src/third_party/libtiff/tif_lzw.c
    src/third_party/libtiff/tif_next.c
    src/third_party/libtiff/tif_ojpeg.c
    src/third_party/libtiff/tif_open.c
    src/third_party/libtiff/tif_packbits.c
    src/third_party/libtiff/tif_pixarlog.c
    src/third_party/libtiff/tif_predict.c
    src/third_party/libtiff/tif_predict.h
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
    src/third_party/libtiff/tiff.h
    src/third_party/libtiff/tiffconf.h
    src/third_party/libtiff/tiffio.h
    src/third_party/libtiff/tiffiop.h
    src/third_party/libtiff/tiffvers.h
    src/third_party/libtiff/uvcode.h
    src/third_party/libtiff/xtifficon.h
    src/third_party/libtiff/xtiffio.h
    src/ui/AspectRatioGroup.cpp
    src/ui/AspectRatioGroup.h
    src/ui/AutoColorGroup.cpp
    src/ui/AutoColorGroup.h
    src/ui/BatchCreateDialog.cpp
    src/ui/BatchCreateDialog.h
    src/ui/BatchProgressDialog.h
    src/ui/BatchRenameDialog.cpp
    src/ui/BatchRenameDialog.h
    src/ui/CreateRuleRow.cpp
    src/ui/CreateRuleRow.h
    src/ui/DriveButton.cpp
    src/ui/DriveButton.h
    src/ui/DuplicateConflictDialog.cpp
    src/ui/DuplicateConflictDialog.h
    src/ui/ElidedTextUtility.h
    src/ui/FavoritePanel.cpp
    src/ui/FavoritePanel.h
    src/ui/FileCollisionDialog.cpp
    src/ui/FileCollisionDialog.h
    src/ui/FileSizeGroup.cpp
    src/ui/FileSizeGroup.h
    src/ui/FolderButton.cpp
    src/ui/FolderButton.h
    src/ui/FormatDecoders.cpp
    src/ui/FormatDecoders.h
    src/ui/FramelessDialogBase.h
    src/ui/FramelessFileDialog.cpp
    src/ui/FramelessFileDialog.h
    src/ui/HistoryPanel.cpp
    src/ui/HistoryPanel.h
    src/ui/IScanResultView.h
    src/ui/IconCacheManager.cpp
    src/ui/IconCacheManager.h
    src/ui/ImageDecoderFacade.cpp
    src/ui/ImageDecoderFacade.h
    src/ui/LinkStatusGroup.cpp
    src/ui/LinkStatusGroup.h
    src/ui/Logger.h
    src/ui/MarkedPanel.cpp
    src/ui/MarkedPanel.h
    src/ui/NavPanel.cpp
    src/ui/NavPanel.h
    src/ui/PresetManager.cpp
    src/ui/PresetManager.h
    src/ui/QuarkApplication.cpp
    src/ui/QuarkApplication.h
    src/ui/QuickLookGraphicsView.cpp
    src/ui/QuickLookGraphicsView.h
    src/ui/QuickLookMinimap.cpp
    src/ui/QuickLookMinimap.h
    src/ui/QuickLookWindow.cpp
    src/ui/RuleRow.cpp
    src/ui/RuleRow.h
    src/ui/ShellIconManager.h
    src/ui/StatusTextCoordinator.cpp
    src/ui/StatusTextCoordinator.h
    src/ui/TagManagerController.cpp
    src/ui/TagManagerController.h
    src/ui/TagManagerDialog.cpp
    src/ui/TagManagerDialog.h
    src/ui/TaskProgressToolBar.cpp
    src/ui/TaskProgressToolBar.h
    src/ui/ThumbnailStatusGroup.cpp
    src/ui/ThumbnailStatusGroup.h
    src/ui/ToolTipOverlay.cpp
    src/ui/UndoToastOverlay.cpp
    src/ui/UndoToastOverlay.h
    src/ui/WindowsShellThumbnailProvider.cpp
    src/ui/WindowsShellThumbnailProvider.h
    src/ui/dialogs/CreateFolderWithItemsDialog.cpp
    src/ui/dialogs/CreateFolderWithItemsDialog.h
    src/ui/dialogs/FileCompareDialog.cpp
    src/ui/dialogs/FileCompareDialog.h
    src/ui/dialogs/FramelessColorPicker.cpp
    src/ui/dialogs/FramelessColorPicker.h
    src/ui/dialogs/FramelessConfirmDialog.cpp
    src/ui/dialogs/FramelessConfirmDialog.h
    src/ui/dialogs/FramelessConflictDialog.cpp
    src/ui/dialogs/FramelessConflictDialog.h
    src/ui/dialogs/FramelessInputDialog.cpp
    src/ui/dialogs/FramelessInputDialog.h
    src/ui/dialogs/FramelessMessageBox.cpp
    src/ui/dialogs/FramelessMessageBox.h
    src/ui/dialogs/SmartFolderDialog.cpp
    src/ui/dialogs/SmartFolderDialog.h
    src/ui/dialogs/SmartFolderRuleRow.cpp
    src/ui/dialogs/SmartFolderRuleRow.h
    src/ui/dialogs/SmartFolderValueWidgets.cpp
    src/ui/dialogs/SmartFolderValueWidgets.h
    src/ui/models/DiskItemModel.cpp
    src/util/AppDirectoryInitializer.h
    src/util/BatchQuickLookService.cpp
    src/util/BatchQuickLookService.h
    src/util/DeepThumbnailExtractor.cpp
    src/util/DeepThumbnailExtractor.h
    src/util/DiskIoService.h
    src/util/DiskMediaExtractor.cpp
    src/util/DiskMediaExtractor.h
    src/util/HardwareInfoHelper.cpp
    src/util/HardwareInfoHelper.h
    src/util/SecureFileEraser.h
    src/util/ShellHelper.cpp
    src/util/ThumbnailPipelineService.cpp
    src/util/ThumbnailPipelineService.h
    src/util/VolumePathResolver.cpp
    src/util/VolumePathResolver.h
    resources/resources.qrc
    resources/app.manifest
)

if(WIN32)
    list(APPEND SOURCES resources/app_icon.rc)
endif()

add_executable(RapidNotes ${SOURCES})

# 从静态 Qt 中排除无用的 PostgreSQL 驱动，彻底消除对 libpq.dll、libssl-3-x64.dll、libintl-9.dll 的依赖
qt_import_plugins(RapidNotes
    INCLUDE_BY_DEFAULT
    EXCLUDE Qt6::QPSQLDriverPlugin
)

# 显式指定 Target 级别的静态运行时
set_property(TARGET RapidNotes PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# 强力锁定目标二进制输出路径，确保无论何种生成器与配置均绝对输出至 RapidNotes_Build/RapidNotes
set_target_properties(RapidNotes PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TARGET_APP_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${TARGET_APP_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${TARGET_APP_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${TARGET_APP_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${TARGET_APP_OUTPUT_DIR}"
)

target_link_libraries(RapidNotes PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::Network
    Qt6::Concurrent
    Qt6::Svg
)

if(WIN32)
    # 静态 Qt6 依赖的原生 Windows 系统库及 WinRT OCR API 支持
    target_link_libraries(RapidNotes PRIVATE
        user32 gdi32 shell32 ole32
        comdlg32 winspool winmm imm32
        dwmapi uxtheme ws2_32 psapi
        setupapi netapi32 userenv
        version crypt32 windowsapp dbghelp
    )
    set_target_properties(RapidNotes PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# ==============================================================================
# 方案 A: 独立看门狗守护进程 (RapidNotesWatchdog)
# ==============================================================================
add_executable(RapidNotesWatchdog
    src/watchdog/main.cpp
)

set_property(TARGET RapidNotesWatchdog PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_target_properties(RapidNotesWatchdog PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${TARGET_APP_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${TARGET_APP_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${TARGET_APP_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${TARGET_APP_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${TARGET_APP_OUTPUT_DIR}"
)

target_link_libraries(RapidNotesWatchdog PRIVATE
    Qt6::Core
)

if(WIN32)
    # 看门狗无窗口运行
    set_target_properties(RapidNotesWatchdog PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
