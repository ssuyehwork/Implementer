#ifndef QUICKSHORTCUTHANDLER_H
#define QUICKSHORTCUTHANDLER_H

#include <QObject>
#include <QKeyEvent>

class QuickWindow;

namespace RapidNotes {

class QuickShortcutHandler : public QObject {
    Q_OBJECT
public:
    explicit QuickShortcutHandler(QuickWindow* window, QObject* parent = nullptr);
    ~QuickShortcutHandler() override = default;

    bool handleKeyPress(QKeyEvent* event);

private:
    bool isEditingFocus() const;

    QuickWindow* m_window = nullptr;
};

} // namespace RapidNotes

#endif // QUICKSHORTCUTHANDLER_H
