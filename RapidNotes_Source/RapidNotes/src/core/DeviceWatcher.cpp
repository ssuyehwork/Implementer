#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "DeviceWatcher.h"
#include <QCoreApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#include <dbt.h>
#endif

namespace QuarkMeta {

DeviceWatcher& DeviceWatcher::instance() {
    static DeviceWatcher s_instance;
    return s_instance;
}

DeviceWatcher::DeviceWatcher(QObject* parent) : QObject(parent) {}

DeviceWatcher::~DeviceWatcher() {
    stopListening();
}

void DeviceWatcher::startListening() {
    if (!m_isListening) {
        if (QCoreApplication::instance()) {
            QCoreApplication::instance()->installNativeEventFilter(this);
            m_isListening = true;
        }
    }
}

void DeviceWatcher::stopListening() {
    if (m_isListening) {
        if (QCoreApplication::instance()) {
            QCoreApplication::instance()->removeNativeEventFilter(this);
        }
        m_isListening = false;
    }
}

bool DeviceWatcher::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) {
    Q_UNUSED(eventType);
    Q_UNUSED(result);

#ifdef Q_OS_WIN
    MSG* msg = static_cast<MSG*>(message);
    if (msg && msg->message == WM_DEVICECHANGE) {
        WPARAM wParam = msg->wParam;
        LPARAM lParam = msg->lParam;

        if (wParam == DBT_DEVICEARRIVAL || wParam == DBT_DEVICEREMOVECOMPLETE) {
            PDEV_BROADCAST_HDR pHdr = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);
            if (pHdr && pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                PDEV_BROADCAST_VOLUME pVol = reinterpret_cast<PDEV_BROADCAST_VOLUME>(lParam);
                DWORD unitmask = pVol->dbcv_unitmask;

                for (char i = 0; i < 26; ++i) {
                    if (unitmask & (1 << i)) {
                        QString driveLetter = QString("%1:").arg(static_cast<char>('A' + i));
                        if (wParam == DBT_DEVICEARRIVAL) {
                            emit driveMounted(driveLetter);
                        } else {
                            emit driveUnmounted(driveLetter);
                        }
                    }
                }
            }
        }
    }
#endif

    return false;
}

} // namespace QuarkMeta
