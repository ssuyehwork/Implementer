#include "QuickPasteDeliveryService.h"
#include "../../core/DatabaseManager.h"
#include "../../core/TagCodec.h"
#include "../StringUtils.h"
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QTimer>

namespace RapidNotes {

QuickPasteDeliveryService& QuickPasteDeliveryService::instance() {
    static QuickPasteDeliveryService s_instance;
    return s_instance;
}

QuickPasteDeliveryService::QuickPasteDeliveryService(QObject* parent)
    : QObject(parent) {}

#ifdef Q_OS_WIN
void QuickPasteDeliveryService::recordActiveWindow(HWND captureHwnd) {
    if (captureHwnd && IsWindow(captureHwnd)) {
        m_lastActiveHwnd = captureHwnd;
    } else {
        m_lastActiveHwnd = GetForegroundWindow();
    }
    if (m_lastActiveHwnd) {
        m_lastThreadId = GetWindowThreadProcessId(m_lastActiveHwnd, nullptr);
    }
}

void QuickPasteDeliveryService::deliverNoteToActiveWindow(const QVariantMap& note, HWND targetHwnd) {
    HWND hwndToUse = targetHwnd ? targetHwnd : m_lastActiveHwnd;
    QString content = note.value("content").toString();
    QString itemType = note.value("item_type").toString();
    int noteId = note.value("id").toInt();

    // 1. 剪贴板双模回写
    if (itemType == "text" || itemType.isEmpty()) {
        StringUtils::copyNoteToClipboard(content);
    } else {
        QStringList paths = content.split(';', Qt::SkipEmptyParts);
        QList<QUrl> urls;
        for (const QString& p : paths) {
            QString clean = p.trimmed().remove('\"');
            if (QFileInfo::exists(clean)) urls.append(QUrl::fromLocalFile(clean));
        }
        if (!urls.isEmpty()) {
            auto* mime = new QMimeData();
            mime->setUrls(urls);
            QApplication::clipboard()->setMimeData(mime);
        } else {
            QApplication::clipboard()->setText(content);
        }
    }

    if (noteId > 0) {
        DatabaseManager::instance().recordAccess(noteId);
    }

    // 2. Win32 模拟按键投递
    if (hwndToUse && IsWindow(hwndToUse)) {
        DWORD currThread = GetCurrentThreadId();
        bool attached = false;
        if (m_lastThreadId != 0 && m_lastThreadId != currThread) {
            attached = AttachThreadInput(currThread, m_lastThreadId, TRUE);
        }
        if (IsIconic(hwndToUse)) ShowWindow(hwndToUse, SW_RESTORE);
        SetForegroundWindow(hwndToUse);

        DWORD lastThread = m_lastThreadId;
        QTimer::singleShot(200, [lastThread, attached]() {
            INPUT inputs[4];
            memset(inputs, 0, sizeof(inputs));
            inputs[0].type = INPUT_KEYBOARD;
            inputs[0].ki.wVk = VK_LCONTROL;
            inputs[1].type = INPUT_KEYBOARD;
            inputs[1].ki.wVk = 'V';
            inputs[2].type = INPUT_KEYBOARD;
            inputs[2].ki.wVk = 'V';
            inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
            inputs[3].type = INPUT_KEYBOARD;
            inputs[3].ki.wVk = VK_LCONTROL;
            inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(4, inputs, sizeof(INPUT));

            if (attached) {
                AttachThreadInput(GetCurrentThreadId(), lastThread, FALSE);
            }
        });
    }
}
#else
void QuickPasteDeliveryService::deliverNoteToActiveWindow(const QVariantMap& note) {
    StringUtils::copyNoteToClipboard(note.value("content").toString());
}
#endif

} // namespace RapidNotes
