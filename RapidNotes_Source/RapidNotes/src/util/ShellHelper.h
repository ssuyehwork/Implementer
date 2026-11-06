#pragma once

#include <QString>
#include <QStringList>

namespace RapidNotes {

class ShellHelper {
public:
    static bool showInExplorer(const QString& path) { Q_UNUSED(path); return false; }
    static bool openTerminal(const QString& dirPath) { Q_UNUSED(dirPath); return false; }
    static bool openWithDefaultApp(const QString& filePath) { Q_UNUSED(filePath); return false; }
    static bool copyPaths(const QStringList& paths) { Q_UNUSED(paths); return false; }
};

} // namespace RapidNotes
