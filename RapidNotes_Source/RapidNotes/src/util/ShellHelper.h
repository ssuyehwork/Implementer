#pragma once

#include <QString>
#include <QStringList>
#include <QList>
#include <QWidget>

namespace RapidNotes {

class ShellHelper {
public:
    struct AssocAppInfo {
        QString name;
        QString exePath;
        QString iconKey;
    };

    static bool openInExplorer(const QString& path) { Q_UNUSED(path); return false; }
    static bool openDirectoryInExplorer(const QString& path) { Q_UNUSED(path); return false; }
    static bool showInExplorer(const QString& path) { Q_UNUSED(path); return false; }
    static bool openTerminal(const QString& dirPath) { Q_UNUSED(dirPath); return false; }
    static bool openWithDefaultApp(const QString& filePath) { Q_UNUSED(filePath); return false; }
    static bool copyPaths(const QStringList& paths) { Q_UNUSED(paths); return false; }
    static QList<AssocAppInfo> getAssociatedApps(const QString& filePath) { Q_UNUSED(filePath); return {}; }
    static bool showOpenWithDialog(const QString& filePath, QWidget* parent = nullptr) { Q_UNUSED(filePath); Q_UNUSED(parent); return false; }
};

} // namespace RapidNotes
