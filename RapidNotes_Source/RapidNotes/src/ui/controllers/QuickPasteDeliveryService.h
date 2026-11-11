#ifndef QUICKPASTEDELIVERYSERVICE_H
#define QUICKPASTEDELIVERYSERVICE_H

#include <QObject>
#include <QVariantMap>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace RapidNotes {

class QuickPasteDeliveryService : public QObject {
    Q_OBJECT
public:
    static QuickPasteDeliveryService& instance();

#ifdef Q_OS_WIN
    void recordActiveWindow(HWND captureHwnd = nullptr);
    void deliverNoteToActiveWindow(const QVariantMap& note, HWND targetHwnd = nullptr);
#else
    void deliverNoteToActiveWindow(const QVariantMap& note);
#endif

private:
    explicit QuickPasteDeliveryService(QObject* parent = nullptr);
    ~QuickPasteDeliveryService() override = default;

#ifdef Q_OS_WIN
    HWND m_lastActiveHwnd = nullptr;
    HWND m_lastFocusHwnd = nullptr;
    DWORD m_lastThreadId = 0;
#endif
};

} // namespace RapidNotes

#endif // QUICKPASTEDELIVERYSERVICE_H
