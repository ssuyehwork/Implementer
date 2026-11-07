#include "VolumeOnlineManager.h"
#include "DeviceWatcher.h"
#include <QDebug>

namespace QuarkMeta {

VolumeOnlineManager& VolumeOnlineManager::instance() {
    static VolumeOnlineManager inst;
    return inst;
}

VolumeOnlineManager::VolumeOnlineManager(QObject* parent)
    : QObject(parent) {
    // 初始收集在线盘符
    auto drives = QDir::drives();
    for (const QFileInfo& drive : drives) {
        QString path = drive.absolutePath();
        if (!path.isEmpty()) {
            QString letter = extractDriveLetter(path);
            if (!letter.isEmpty()) {
                m_onlineDrives.insert(letter.toUpper());
            }
        }
    }

    // 绑定 DeviceWatcher 的原生 WM_DEVICECHANGE 操作系统硬件事件，消除定时器轮询
    connect(&DeviceWatcher::instance(), &DeviceWatcher::driveMounted, this, [this](const QString& driveLetter) {
        QString letter = extractDriveLetter(driveLetter);
        if (!letter.isEmpty()) {
            bool newlyAdded = false;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (!m_onlineDrives.contains(letter)) {
                    m_onlineDrives.insert(letter);
                    newlyAdded = true;
                }
            }
            if (newlyAdded) {
                emit volumeStateChanged(letter, true);
            }
        }
    });

    connect(&DeviceWatcher::instance(), &DeviceWatcher::driveUnmounted, this, [this](const QString& driveLetter) {
        QString letter = extractDriveLetter(driveLetter);
        if (!letter.isEmpty()) {
            bool newlyRemoved = false;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_onlineDrives.contains(letter)) {
                    m_onlineDrives.remove(letter);
                    newlyRemoved = true;
                }
            }
            if (newlyRemoved) {
                emit volumeStateChanged(letter, false);
            }
        }
    });

    DeviceWatcher::instance().startListening();
}

QSet<QString> VolumeOnlineManager::getOnlineDrives() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_onlineDrives;
}

QString VolumeOnlineManager::extractDriveLetter(const QString& str) {
    if (str.isEmpty()) return QString();

    // 针对 QuarkMeta.library_g 或 library_g 形态
    if (str.contains("library_", Qt::CaseInsensitive)) {
        int idx = str.indexOf("library_", 0, Qt::CaseInsensitive);
        QString sub = str.mid(idx + 8);
        if (!sub.isEmpty()) {
            return sub.left(1).toUpper();
        }
    }

    // 针对包含冒号的路径，如 "G:/..." 或 "G:"
    int colonIdx = str.indexOf(':');
    if (colonIdx > 0) {
        return str.mid(colonIdx - 1, 1).toUpper();
    }

    // 针对单字符，如 "G" 或 "g"
    if (str.length() == 1 && str.at(0).isLetter()) {
        return str.toUpper();
    }

    return QString();
}

bool VolumeOnlineManager::isLibraryOnline(const QString& libraryNameOrPath) const {
    QString letter = extractDriveLetter(libraryNameOrPath);
    if (letter.isEmpty()) return true; // 无法提取盘符的默认判定在线
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_onlineDrives.contains(letter.toUpper());
}

void VolumeOnlineManager::checkVolumeState() {
    auto drives = QDir::drives();
    QSet<QString> currentDrives;
    for (const QFileInfo& drive : drives) {
        QString path = drive.absolutePath();
        if (!path.isEmpty()) {
            QString letter = extractDriveLetter(path);
            if (!letter.isEmpty()) {
                currentDrives.insert(letter.toUpper());
            }
        }
    }

    QSet<QString> oldDrives;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        oldDrives = m_onlineDrives;
        m_onlineDrives = currentDrives;
    }

    // 拔盘广播
    for (const QString& letter : oldDrives) {
        if (!currentDrives.contains(letter)) {
            emit volumeStateChanged(letter, false);
        }
    }

    // 插盘广播
    for (const QString& letter : currentDrives) {
        if (!oldDrives.contains(letter)) {
            emit volumeStateChanged(letter, true);
        }
    }
}

} // namespace QuarkMeta
