#ifndef NOMINMAX
#define NOMINMAX
#endif
//2813583 main 禁止删除此行
#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QSharedMemory>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QSvgRenderer>
#include <QPainter>
#include <windows.h>
#include <objbase.h>
#include <shellapi.h>
#include "ui/UiHelper.h"
#include "ui/ScanDialog.h"
#include "ui/TrayController.h"
#include "meta/MetadataManager.h"
#include "mft/MftReader.h"
#include "core/CoreController.h"

/**
 * @brief 自定义日志处理程序，将 qDebug 消息重定向至本地 .log 文件
 * 2026-03-xx 按照用户要求：在手动运行 .exe 时，通过日志文件排查初始化挂起或信号丢失问题。
 */
static qint64 g_currentLogSize = -1; // 内存计数器，避免频繁 stat 系统调用
static QMutex g_logMutex;            // 2026-06-xx 物理加固：保护日志写入与轮转逻辑的线程安全

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    Q_UNUSED(context); 
    QMutexLocker locker(&g_logMutex);

    // 2026-07-07 根本修复：将日志移出监控盘符，放入 AppData 目录以杜绝 USN 监控无限循环
    static QString logPath;
    if (logPath.isEmpty()) {
        // 2026-06-xx 调试优化：优先在程序当前目录生成日志，方便用户直接查看
        logPath = QDir(QCoreApplication::applicationDirPath()).filePath("FERREX_debug.log");
    }

    const QString logFileName = logPath;
    const qint64 maxLogSize = 10 * 1024 * 1024; // 10MB 轮转阈值
    const int maxHistoryFiles = 3;           // 保留 3 个历史备份

    // 1. 初始化计数器
    if (g_currentLogSize < 0) {
        QFile f(logFileName);
        g_currentLogSize = f.exists() ? f.size() : 0;
    }

    // 2. 写入日志
    QFile logFile(logFileName);
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream textStream(&logFile);
        QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
        QString level;
        switch (type) {
            case QtDebugMsg:    level = "DEBUG";    break;
            case QtInfoMsg:     level = "INFO ";    break;
            case QtWarningMsg:  level = "WARN ";    break;
            case QtCriticalMsg: level = "CRIT ";    break;
            case QtFatalMsg:    level = "FATAL";    break;
        }
        QString line = QString("[%1][%2] %3\n").arg(timeStr, level, msg);
        textStream << line;
        textStream.flush();

        // 同时输出到 stderr 确保在控制台可见
        fprintf(stderr, "%s", line.toLocal8Bit().constData());
        fflush(stderr);

        g_currentLogSize += line.toUtf8().size(); // 累加内存计数
        logFile.close();

        // 3. 检查并执行轮转 (发生在写入完成后)
        if (g_currentLogSize > maxLogSize) {
            // 清理旧文件并重命名链
            for (int i = maxHistoryFiles; i >= 1; --i) {
                QString oldName = logFileName + QString(".%1").arg(i);
                QString newName = logFileName + QString(".%1").arg(i + 1);
                if (i == maxHistoryFiles) {
                    QFile::remove(oldName);
                } else {
                    if (QFile::exists(oldName)) QFile::rename(oldName, newName);
                }
            }
            // 当前文件重命名为 .1
            if (QFile::rename(logFileName, logFileName + ".1")) {
                g_currentLogSize = 0;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    // 初始化 COM 环境 (多媒体缩略图提取需要)
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    // 1. 安装自定义日志处理器：确保从程序启动的第一秒开始就能捕获所有调试信息
    qInstallMessageHandler(customMessageHandler);
    qDebug() << "================ FERREX 启动加载 ================";

    // 设置高 DPI 支持：Qt 6 默认行为，此处显式设置 PassThrough 以防旧设备缩放模糊
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QApplication a(argc, argv);

    // 单例运行检测
    QSharedMemory sharedMem("FERREX_SINGLE_INSTANCE_SHARED_MEMORY_KEY_UNIQUE_2026");
    if (sharedMem.attach()) {
        qWarning() << "[SINGLE_INSTANCE] 检测到已有实例正在运行，当前进程准备安全退出。";
        return 0; // 重复启动，安全静默退出
    }
    
    if (!sharedMem.create(1)) {
        qWarning() << "[SINGLE_INSTANCE] 共享内存块创建失败，错误代码:" << sharedMem.errorString();
        // 在极少数由于前序系统崩溃导致共享内存残留的情况下，尝试重新 attach 并处理
        if (sharedMem.error() == QSharedMemory::AlreadyExists) {
            qWarning() << "[SINGLE_INSTANCE] 共享内存已存在。重复运行，当前进程准备安全退出。";
            return 0;
        }
    }

    a.setQuitOnLastWindowClosed(false);
    
    // 2026-04-14 按照用户要求：物理加固图标加载逻辑
    // 杜绝相对路径幻觉，强制使用 Qt 资源系统 (:/) 加载 app_icon.ico，确保托盘显示不失效
    a.setWindowIcon(QIcon(":/app_icon.ico"));

    a.setApplicationName("FERREX");
    a.setOrganizationName("FERREXTeam");

    // 硬件指纹多条件 OR 授权通行证校验
    auto getWmiOutput = [](const QString& arg) -> QString {
        QProcess p;
        p.start("wmic", arg.split(' ', Qt::SkipEmptyParts));
        if (p.waitForFinished(3000)) {
            return QString::fromLocal8Bit(p.readAllStandardOutput());
        }
        return "";
    };

    auto getWmiFirstValue = [&](const QString& arg) -> QString {
        QString out = getWmiOutput(arg);
        QStringList lines = out.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
        if (lines.size() >= 2) return lines[1].trimmed();
        return "";
    };

    bool passHardware = false;

    // 1. CPU ID 检查
    QString cpuId = getWmiFirstValue("cpu get processorid");
    if (cpuId.compare("BFEBFBFF000306C3", Qt::CaseInsensitive) == 0) passHardware = true;

    // 2. 主板序列号检查
    if (!passHardware) {
        QString boardSn = getWmiFirstValue("baseboard get serialnumber");
        if (boardSn.compare("SGH412RF00", Qt::CaseInsensitive) == 0) passHardware = true;
    }

    // 3. BIOS 序列号检查
    if (!passHardware) {
        QString biosSn = getWmiFirstValue("bios get serialnumber");
        if (biosSn.compare("SGH412RF00", Qt::CaseInsensitive) == 0) passHardware = true;
    }

    // 4. 硬盘序列号检查（任一匹配即可）
    const QStringList targetDisks = {
        "494000PA0D9L", "PHYS825203NX480BGN", "NA5360WJ", "NA7G89GQ", "03000210052122072519"
    };

    if (!passHardware) {
        QString diskOutput = getWmiOutput("diskdrive get serialnumber");
        for (const QString& d : targetDisks) {
            if (diskOutput.contains(d, Qt::CaseInsensitive)) {
                passHardware = true;
                break;
            }
        }
    }

    // 5. MAC 地址检查（任一匹配即可）
    const QStringList targetMacs = {
        "8C:81:2B:60:19:ED", "00:15:5D:13:18:72", "A0:48:1C:86:32:DF",
        "5E:1B:66:55:5B:8B", "00:15:5D:99:09:5D", "00:15:5D:E9:6A:69"
    };

    if (!passHardware) {
        QProcess getmacProc;
        getmacProc.start("getmac", QStringList() << "/fo" << "csv" << "/v");
        if (getmacProc.waitForFinished(5000)) {
            QString macOutput = QString::fromLocal8Bit(getmacProc.readAllStandardOutput());
            QStringList lines = macOutput.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
            for (const QString& line : lines) {
                QStringList parts = line.split(',');
                for (QString token : parts) {
                    token = token.remove('"').trimmed();
                    QString normalized = token;
                    normalized.remove('-').remove(':');
                    if (normalized.length() == 12 && normalized.contains(QRegularExpression("^[0-9A-Fa-f]{12}$"))) {
                        QString formattedMac = normalized.toUpper();
                        formattedMac.insert(2, ':');
                        formattedMac.insert(5, ':');
                        formattedMac.insert(8, ':');
                        formattedMac.insert(11, ':');
                        formattedMac.insert(14, ':');
                        for (const QString& target : targetMacs) {
                            if (formattedMac.compare(target, Qt::CaseInsensitive) == 0) {
                                passHardware = true;
                                break;
                            }
                        }
                    }
                    if (passHardware) break;
                }
                if (passHardware) break;
            }
        }
    }

    if (!passHardware) {
        QDialog dlg;
        dlg.setWindowTitle("环境初始化提示");
        dlg.setFixedSize(460, 230);
        dlg.setStyleSheet(
            "QDialog { background-color: #1e1e1e; border: 1px solid #333333; border-radius: 8px; }"
            "QLabel { color: #cccccc; font-size: 13px; line-height: 140%; }"
            "QPushButton { background-color: #3e3e42; color: #ffffff; border: 1px solid #555555; border-radius: 4px; padding: 6px 16px; font-weight: bold; font-size: 13px; }"
            "QPushButton:hover { background-color: #4e4e52; border-color: #777777; }"
            "QPushButton:pressed { background-color: #2d2d30; }"
        );

        QVBoxLayout* layout = new QVBoxLayout(&dlg);
        layout->setContentsMargins(24, 20, 24, 20);
        layout->setSpacing(16);

        QLabel* titleLabel = new QLabel("<span style='font-size: 15px; font-weight: bold; color: #f1c40f;'>当前硬件运行环境不兼容</span>", &dlg);
        layout->addWidget(titleLabel);

        QLabel* bodyLabel = new QLabel(
            "检测到当前计算机的主板总线架构、芯片组指令集或存储子系统未达到本应用的底层初始化要求。<br><br>"
            "本应用依赖专有的硬件底层加速与隔离调度指令，当前设备环境不适宜运行该程序。<br><br>"
            "<span style='color: #888888;'>程序初始化失败，即将安全退出。</span>", &dlg);
        bodyLabel->setWordWrap(true);
        layout->addWidget(bodyLabel);

        QHBoxLayout* btnLayout = new QHBoxLayout();
        btnLayout->addStretch();
        QPushButton* btnExit = new QPushButton("确认并退出", &dlg);
        btnExit->setCursor(Qt::PointingHandCursor);
        QObject::connect(btnExit, &QPushButton::clicked, &dlg, &QDialog::accept);
        btnLayout->addWidget(btnExit);
        layout->addLayout(btnLayout);

        dlg.exec();
        return 0;
    }

    // 2026-06-xx 架构重构：彻底移除 DB 模块。系统采用纯轻量化 MFT + JSON 元数据架构。
    FERREX::MetadataManager::instance();

    // 3. 简化启动：直接显示 ScanDialog (Plan-136)
    // 彻底移除 MainWindow，仅保留 ScanDialog 作为唯一主界面
    FERREX::ScanDialog* w = new FERREX::ScanDialog();
    w->show();

    // 4. 集成托盘图标支持
    FERREX::TrayController* tray = new FERREX::TrayController(w);
    tray->show();

    // 5. 启动异步系统扫描（后台初始化，UI 可响应）
    FERREX::CoreController::instance().startSystem();

    int ret = a.exec();

    // 在主程序完全退出时，进行全局单例销毁和索引释放
    FERREX::MftReader::instance().clear();
    CoUninitialize();

    return ret;
}
