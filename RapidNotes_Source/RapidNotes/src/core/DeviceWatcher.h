#pragma once

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QString>

namespace QuarkMeta {

class DeviceWatcher : public QObject, public QAbstractNativeEventFilter {
    Q_OBJECT

public:
    static DeviceWatcher& instance();

    void startListening();
    void stopListening();

    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;

signals:
    void driveMounted(const QString& driveLetter);
    void driveUnmounted(const QString& driveLetter);

private:
    explicit DeviceWatcher(QObject* parent = nullptr);
    ~DeviceWatcher() override;
    DeviceWatcher(const DeviceWatcher&) = delete;
    DeviceWatcher& operator=(const DeviceWatcher&) = delete;

    bool m_isListening = false;
};

} // namespace QuarkMeta
