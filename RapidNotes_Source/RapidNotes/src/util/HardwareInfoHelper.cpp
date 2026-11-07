#include "HardwareInfoHelper.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <winioctl.h>
#include <intrin.h>
#endif

#include <QProcess>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

#pragma pack(push, 1)
typedef struct _ATA_IDENTIFY_DATA {
    unsigned short Reserved1[10];
    unsigned short SerialNumber[10];      // Word 10-19
    unsigned short Reserved2[235];
} ATA_IDENTIFY_DATA;
#pragma pack(pop)

namespace QuarkMeta {


QString HardwareInfoHelper::getDiskPhysicalSerialNumberByDrive(const QString& drivePath) {
#ifdef Q_OS_WIN
    QString cleanDrive = drivePath.trimmed();
    if (cleanDrive.endsWith("\\")) cleanDrive.chop(1);
    if (cleanDrive.contains(":")) cleanDrive = cleanDrive.left(2);

    QString volumePath = "\\\\.\\" + cleanDrive;
    HANDLE hVolume = CreateFileW(reinterpret_cast<LPCWSTR>(volumePath.utf16()), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hVolume == INVALID_HANDLE_VALUE) {
        return "";
    }

    STORAGE_DEVICE_NUMBER sdn;
    DWORD br = 0;
    BOOL ok = DeviceIoControl(hVolume, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &sdn, sizeof(sdn), &br, NULL);
    CloseHandle(hVolume);
    if (!ok) return "";

    QString diskPath = QString("\\\\.\\PhysicalDrive%1").arg(sdn.DeviceNumber);
    HANDLE hDisk = CreateFileW(reinterpret_cast<LPCWSTR>(diskPath.utf16()), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDisk == INVALID_HANDLE_VALUE) {
        hDisk = CreateFileW(reinterpret_cast<LPCWSTR>(diskPath.utf16()), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    }
    if (hDisk == INVALID_HANDLE_VALUE) return "";

    STORAGE_PROPERTY_QUERY query;
    memset(&query, 0, sizeof(query));
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;

    BYTE buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    br = 0;

    QString cachedSerial;
    if (DeviceIoControl(hDisk, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query), buffer, sizeof(buffer), &br, NULL)) {
        STORAGE_DEVICE_DESCRIPTOR* desc = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(buffer);
        if (desc->SerialNumberOffset != 0 && desc->SerialNumberOffset < br) {
            const char* sn = reinterpret_cast<const char*>(buffer + desc->SerialNumberOffset);
            cachedSerial = QString::fromLatin1(sn).trimmed();
        }
    }

    CloseHandle(hDisk);
    return cachedSerial;
#else
    Q_UNUSED(drivePath);
    return "";
#endif
}

QString HardwareInfoHelper::getCDiskPhysicalSerialNumber() {
    static QString cachedC;
    if (cachedC.isEmpty()) cachedC = getDiskPhysicalSerialNumberByDrive("C:");
    return cachedC;
}

QString HardwareInfoHelper::getAppDrivePhysicalSerialNumber() {
    static QString cachedApp;
    if (cachedApp.isEmpty()) {
        QString path = QCoreApplication::applicationDirPath();
        if (path.contains(":")) {
            QString drive = path.left(2);
            cachedApp = getDiskPhysicalSerialNumberByDrive(drive);
        }
    }
    return cachedApp;
}

QString HardwareInfoHelper::getBoardSerialNumber() {
    static QString cachedBoard;
    if (!cachedBoard.isEmpty()) return cachedBoard;

    QProcess process;
    process.start("wmic", QStringList() << "baseboard" << "get" << "serialnumber");
    if (process.waitForFinished(3000)) {
        QString result = QString::fromLocal8Bit(process.readAllStandardOutput());
        QStringList lines = result.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
        if (lines.size() >= 2) {
            cachedBoard = lines[1].trimmed();
        }
    }

    if (cachedBoard.isEmpty() || cachedBoard.toUpper().contains("TO BE FILLED")) {
        process.start("wmic", QStringList() << "csproduct" << "get" << "uuid");
        if (process.waitForFinished(3000)) {
            QString result = QString::fromLocal8Bit(process.readAllStandardOutput());
            QStringList lines = result.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
            if (lines.size() >= 2) cachedBoard = lines[1].trimmed();
        }
    }
    return cachedBoard;
}

QString HardwareInfoHelper::getCpuId() {
    static QString cachedCpuId;
    if (!cachedCpuId.isEmpty()) return cachedCpuId;

#if defined(Q_OS_WIN) && (defined(_M_IX86) || defined(_M_X64))
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);
    // 标准 Windows ProcessorID 格式为 EDX 高位 + EAX 低位
    cachedCpuId = QString("%1%2")
                  .arg(cpuInfo[3], 8, 16, QChar('0'))
                  .arg(cpuInfo[0], 8, 16, QChar('0')).toUpper();
#endif
    return cachedCpuId;
}

QString HardwareInfoHelper::getBiosSerialNumber() {
    static QString cachedBiosSn;
    if (!cachedBiosSn.isEmpty()) return cachedBiosSn;

    QProcess process;
    process.start("wmic", QStringList() << "bios" << "get" << "serialnumber");
    if (process.waitForFinished(3000)) {
        QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
        QStringList lines = output.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
        if (lines.size() >= 2) {
            QString serialLine = lines[1].trimmed();
            if (!serialLine.isEmpty() && serialLine != "SerialNumber") {
                cachedBiosSn = serialLine;
            }
        }
    }
    return cachedBiosSn;
}

QStringList HardwareInfoHelper::getAllDiskSerialNumbers() {
    static QStringList cachedDiskSerials;
    if (!cachedDiskSerials.isEmpty()) return cachedDiskSerials;

    QProcess process;
    process.start("wmic", QStringList() << "diskdrive" << "get" << "serialnumber");
    if (process.waitForFinished(3000)) {
        QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
        QStringList lines = output.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
        for (int i = 1; i < lines.size(); ++i) {
            QString serial = lines[i].trimmed();
            if (!serial.isEmpty() && serial != "SerialNumber") {
                cachedDiskSerials << serial;
            }
        }
    }
    return cachedDiskSerials;
}

QStringList HardwareInfoHelper::getAllMacAddresses() {
    static QStringList cachedMacAddresses;
    if (!cachedMacAddresses.isEmpty()) return cachedMacAddresses;

    QProcess process;
    process.start("getmac", QStringList() << "/fo" << "csv" << "/v");
    if (process.waitForFinished(5000)) {
        QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
        QStringList lines = output.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
        for (const QString& line : lines) {
            QStringList parts = line.split(',');
            for (QString token : parts) {
                token = token.remove('"').trimmed();
                // 支持两种形式：XX-XX-XX-XX-XX-XX 或 XXXXXXXXXXXX
                QString normalized = token;
                normalized.remove('-').remove(':');
                if (normalized.length() == 12 && normalized.contains(QRegularExpression("^[0-9A-Fa-f]{12}$"))) {
                    QString formattedMac = normalized.toUpper();
                    formattedMac.insert(2, ':');
                    formattedMac.insert(5, ':');
                    formattedMac.insert(8, ':');
                    formattedMac.insert(11, ':');
                    formattedMac.insert(14, ':');
                    if (!cachedMacAddresses.contains(formattedMac)) {
                        cachedMacAddresses << formattedMac;
                    }
                }
            }
        }
    }
    return cachedMacAddresses;
}

bool HardwareInfoHelper::isHardwareAuthorized() {
    // 1. CPU ID 检查
    QString cpuId = getCpuId();
    if (cpuId.compare("BFEBFBFF000306C3", Qt::CaseInsensitive) == 0) {
        qDebug() << "[QuarkMeta-AUTH] CPU ID 匹配成功:" << cpuId;
        return true;
    }

    // 2. 主板序列号检查
    QString boardSn = getBoardSerialNumber();
    if (boardSn.compare("SGH412RF00", Qt::CaseInsensitive) == 0) {
        qDebug() << "[QuarkMeta-AUTH] 主板序列号匹配成功:" << boardSn;
        return true;
    }

    // 3. BIOS 序列号检查
    QString biosSn = getBiosSerialNumber();
    if (biosSn.compare("SGH412RF00", Qt::CaseInsensitive) == 0) {
        qDebug() << "[QuarkMeta-AUTH] BIOS 序列号匹配成功:" << biosSn;
        return true;
    }

    // 4. 硬盘序列号检查（授权硬盘列表）
    const QStringList targetDisks = {
        "494000PA0D9L",
        "PHYS825203NX480BGN",
        "NA5360WJ",
        "NA7G89GQ",
        "03000210052122072519"
    };

    auto checkDiskMatch = [&](const QString& sn) -> bool {
        if (sn.isEmpty()) return false;
        for (const QString& target : targetDisks) {
            if (sn.compare(target, Qt::CaseInsensitive) == 0) {
                return true;
            }
        }
        return false;
    };

    // 4.1 程序所在硬盘序列号检查（移动硬盘随插随用）
    QString appDriveSn = getAppDrivePhysicalSerialNumber();
    if (checkDiskMatch(appDriveSn)) {
        qDebug() << "[QuarkMeta-AUTH] 程序所在移动硬盘序列号匹配成功:" << appDriveSn;
        return true;
    }

    // 4.2 系统盘（C盘）硬盘序列号检查
    QString cDriveSn = getCDiskPhysicalSerialNumber();
    if (checkDiskMatch(cDriveSn)) {
        qDebug() << "[QuarkMeta-AUTH] 系统盘序列号匹配成功:" << cDriveSn;
        return true;
    }

    // 4.3 全量物理磁盘列表检查
    QStringList allDisks = getAllDiskSerialNumbers();
    for (const QString& d : allDisks) {
        if (checkDiskMatch(d)) {
            qDebug() << "[QuarkMeta-AUTH] 全量物理磁盘列表匹配成功:" << d;
            return true;
        }
    }

    // 5. MAC 地址检查（任一网卡匹配即可）
    const QStringList targetMacs = {
        "8C:81:2B:60:19:ED",
        "00:15:5D:13:18:72",
        "A0:48:1C:86:32:DF",
        "5E:1B:66:55:5B:8B",
        "00:15:5D:99:09:5D",
        "00:15:5D:E9:6A:69"
    };

    QStringList allMacAddresses = getAllMacAddresses();
    for (const QString& mac : allMacAddresses) {
        for (const QString& target : targetMacs) {
            if (mac.compare(target, Qt::CaseInsensitive) == 0) {
                qDebug() << "[QuarkMeta-AUTH] MAC 地址匹配成功:" << mac;
                return true;
            }
        }
    }

    qWarning() << "[QuarkMeta-AUTH] 当前环境未匹配任何授权硬件特征！";
    return false;
}

} // namespace QuarkMeta
