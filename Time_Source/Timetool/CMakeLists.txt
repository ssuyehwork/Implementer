cmake_minimum_required(VERSION 3.16)
project(Time_tool)

# 启用现代 MSVC 运行时库策略 (彻底解决 /MT 与 /MD 混用冲突)
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 全局设置 MSVC 为静态运行时库 (/MT Release, /MTd Debug)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_PREFIX_PATH "G:/Qt/6.10.2/msvc2022_64_static")

# 遵循“三权分立”规范：将生成的 exe 输出到固化运行层 Timetool_Build/Timetool
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../../Timetool_Build/Timetool")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/../../Timetool_Build/Timetool")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../../Timetool_Build/Timetool")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../../Timetool_Build/Timetool")

if(MSVC)
    add_compile_options(
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/GL>
    )
    add_link_options(
        $<$<CONFIG:Release>:/INCREMENTAL:NO>
        $<$<CONFIG:Release>:/LTCG>
        $<$<CONFIG:Release>:/OPT:REF>
        $<$<CONFIG:Release>:/OPT:ICF>
    )
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core Svg)

# 包含 main.cpp、Windows 图标资源 (.rc) 以及 Qt 内嵌资源 (.qrc)
add_executable(Time_tool WIN32 
    src/main.cpp
    resources/app.rc
    resources/resources.qrc
)

# 保证头文件搜索路径包含 src
target_include_directories(Time_tool PRIVATE src)

# 显式指定 Target 级别的静态运行时
set_property(TARGET Time_tool PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_link_libraries(Time_tool PRIVATE
    Qt6::Widgets
    Qt6::Gui
    Qt6::Core
    Qt6::Svg
)

if(WIN32)
    target_link_libraries(Time_tool PRIVATE
        User32 Gdi32 Shell32 Ole32
        Comdlg32 Winspool Winmm Imm32
        Dwmapi Uxtheme Ws2_32
        Setupapi Netapi32 Userenv
        Version Crypt32
    )
endif()