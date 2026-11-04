cmake_minimum_required(VERSION 3.16)

project(CountdownLock VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 遵循“三权分立”规范：将生成的 exe 输出到固化运行层 FocusLock_Build/FocusLock
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../FocusLock_Build/FocusLock")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../FocusLock_Build/FocusLock")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../FocusLock_Build/FocusLock")

# 寻找 Qt 6 库
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core Svg)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

# 源文件列表（按架构分层添加）
set(SOURCES
    src/main.cpp
    # core
    src/core/ConfigManager.h src/core/ConfigManager.cpp
    src/core/CountdownEngine.h src/core/CountdownEngine.cpp
    src/core/SessionLogger.h src/core/SessionLogger.cpp
    # system
    src/system/SystemHookManager.h src/system/SystemHookManager.cpp
    src/system/TopMostGuard.h src/system/TopMostGuard.cpp
    src/system/PowerManager.h src/system/PowerManager.cpp
    src/system/ProcessProtector.h src/system/ProcessProtector.cpp
    # ui
    src/ui/SetupDialog.h src/ui/SetupDialog.cpp
    src/ui/SettingsPanel.h src/ui/SettingsPanel.cpp
    src/ui/LockScreenWindow.h src/ui/LockScreenWindow.cpp
    src/ui/TrayManager.h src/ui/TrayManager.cpp
    src/ui/PreLockNotification.h src/ui/PreLockNotification.cpp
    # utils
    src/utils/SvgIcon.h src/utils/SvgIcon.cpp
    # resources
    resources/app_icon.rc
)

# 定义可执行文件（Windows GUI 程序）
add_executable(CountdownLock WIN32 ${SOURCES})

# 链接库
target_link_libraries(CountdownLock PRIVATE
    Qt6::Widgets
    Qt6::Gui
    Qt6::Core
    Qt6::Svg
    dwmapi
    user32
    kernel32
    advapi32
)

# 安装配置（可选）
if(WIN32)
    set_target_properties(CountdownLock PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
