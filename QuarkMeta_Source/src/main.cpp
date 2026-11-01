#ifndef NOMINMAX
#define NOMINMAX
#endif
//2813583 main 禁止删除此行
#include "ui/QuarkApplication.h"
#include "FramelessDialog.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QSvgRenderer>
#include <QPainter>
#include <QLockFile>
#include <QDir>
#include <QMutex>
#include <QTimer>
#include <QThreadPool>

#ifdef Q_OS_WIN
#include <windows.h>
#include <objbase.h>
#include <shellapi.h>
#endif
#include "ui/UiHelper.h"
#include "ui/ThemeManager.h"
#include "ui/Logger.h"
#include "ui/MainWindow.h"

#include "meta/MetadataManager.h"
#include "meta/MediaExtractorPipeline.h"
#include "meta/DatabaseManager.h"
#include "core/CoreController.h"
#include "core/DirectoryWatchService.h"
#include "util/HardwareInfoHelper.h"
#include <QMessageBox>

/**
 * @brief 自定义日志重定向。极速格式化日志内容后投递到异步缓冲区，杜绝同步磁盘等待。
 */
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    Q_UNUSED(context);
    QString level;
    switch (type) {
        case QtDebugMsg:    level = "DEBUG";    break;
        case QtInfoMsg:     level = "INFO ";    break;
        case QtWarningMsg:  level = "WARN ";    break;
        case QtCriticalMsg: level = "CRIT ";    break;
        case QtFatalMsg:    level = "FATAL";    break;
    }
    // 投递至异步 RingBuffer 日志引擎写出，线程安全且极其高效
    QuarkMeta::Logger::log(QString("[%1] %2").arg(level, msg));
}

/**
 * @brief 退出时调用的清场函数，优雅停止各子系统线程、确保数据完整落盘不损坏。
 */
void onApplicationAboutToQuit(HANDLE hMutex) {
    // 1. 立即设置停机原子标记
    QuarkMeta::CoreController::requestShutdown();

    // 2. 停止所有目录变更监听
    QuarkMeta::DirectoryWatchService::instance().stopAll();

    // 3. 立即熔断提图后台流水线
    QuarkMeta::MediaExtractorPipeline::instance().cancelAll();

    // 3. 限时 200ms 排空线程池，绝不无限期死等
    QThreadPool::globalInstance()->waitForDone(200);

    // 4. 全系统唯一权威的数据库安全落盘与闭卷
    QuarkMeta::DatabaseManager::instance().shutdown();

#ifdef Q_OS_WIN
    CoUninitialize();

    if (hMutex) {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
    }
#endif
}

int main(int argc, char *argv[]) {
    // -------------------------------------------------------------
    // 重构 2：启动安全。最顶端优先执行单实例互斥量哨兵检测
    // -------------------------------------------------------------
    HANDLE hMutex = nullptr;
#ifdef Q_OS_WIN
    hMutex = CreateMutexA(NULL, TRUE, "QuarkMeta_SingleInstance_Mutex");
    if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
        if (hMutex) CloseHandle(hMutex);
        // 单实例检测失败，由于尚未影响任何运行状态及日志，直接优雅退出
        return 0;
    }
#else
    QString lockPath = QDir::tempPath() + "/QuarkMeta_SingleInstance.lock";
    static QLockFile lockFile(lockPath);
    if (!lockFile.tryLock(100)) {
        return 0;
    }
#endif


    // 1. 安装自定义日志处理器（使用超高吞吐无阻塞的内存队列异步写入）
    qInstallMessageHandler(customMessageHandler);

    // 设置高 DPI 支持：Qt 6 默认行为，此处显式设置 PassThrough 以防旧设备缩放模糊
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QuarkMeta::QuarkApplication a(argc, argv);

    // 全局统一设置深色 QPalette，防止原生 Windows 调色板在斑马纹/Base/AlternateBase 露底纯白
    QPalette p;
    p.setColor(QPalette::Window, QColor("#1E1E1E"));
    p.setColor(QPalette::WindowText, QColor("#EEEEEE"));
    p.setColor(QPalette::Base, QColor("#1E1E1E"));
    p.setColor(QPalette::AlternateBase, QColor("#252526"));
    p.setColor(QPalette::ToolTipBase, QColor("#252526"));
    p.setColor(QPalette::ToolTipText, QColor("#EEEEEE"));
    p.setColor(QPalette::Text, QColor("#EEEEEE"));
    p.setColor(QPalette::Button, QColor("#252526"));
    p.setColor(QPalette::ButtonText, QColor("#EEEEEE"));
    p.setColor(QPalette::Highlight, QColor(52, 152, 219));      // #3498db (蓝色)
    p.setColor(QPalette::HighlightedText, Qt::white);
    a.setPalette(p);

    a.setQuitOnLastWindowClosed(false);
    
    // 2026-04-14 按照用户要求：物理加固图标加载逻辑
    // 杜绝相对路径幻觉，强制使用 Qt 资源系统 (:/) 加载 app_icon.ico，确保托盘显示不失效
    a.setWindowIcon(QIcon(":/app_icon.ico"));

    a.setApplicationName("QuarkMeta");

    // -------------------------------------------------------------
    // 重构 4：COM 亲和性。在 QApplication 实例化后，安全初始化 COM 环境
    // -------------------------------------------------------------
#ifdef Q_OS_WIN
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
#endif

    // -------------------------------------------------------------
    // 硬件指纹多条件 OR 授权通行证校验（支持移动硬盘随插随用）
    // -------------------------------------------------------------
    if (!QuarkMeta::HardwareInfoHelper::isHardwareAuthorized()) {
        QMessageBox::critical(
            nullptr,
            "授权提示",
            "<b>[安全拦截] 检测到硬件指纹不匹配。</b><br><br>"
            "当前运行环境未检测到合法的硬件授权特征（CPU / 主板 / BIOS / 授权移动硬盘）。<br><br>"
            "程序将立即退出。"
        );
        return 0;
    }

    // -------------------------------------------------------------
    // 重构 3：生命期。引入 AppCoreController 统一拓扑预热
    // -------------------------------------------------------------
    QuarkMeta::CoreController::initializeCoreComponents();

    QuarkMeta::ThemeManager::instance().initialize(&a);

    QuarkMeta::MainWindow w;
    
    // 启动异步系统扫描与监控监听
    QuarkMeta::CoreController::instance().startSystem();

    // 利用主线程第一个 Tick 调度，平滑显示窗口，消解首帧信号洪暴导致的渲染卡顿
    QTimer::singleShot(0, [&w]() {
        w.show();
        w.raise();
        w.activateWindow();
    });

    // -------------------------------------------------------------
    // 重构 6：Clean Shutdown 退出清场机制挂接
    // -------------------------------------------------------------
    QObject::connect(&a, &QApplication::aboutToQuit, [&a, hMutex]() {
        onApplicationAboutToQuit(hMutex);
    });

    return a.exec();
}
 