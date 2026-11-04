#pragma once

#include <QString>
#include <QStringList>

namespace RapidNotes {

class PathSummary {
public:
    enum class Kind {
        SingleFile,
        SingleFolder,
        MultipleFilesOrMixed,
        MultipleFolders
    };

    static Kind summarize(const QStringList& paths);
    static Kind summarizeFromSemicolonString(const QString& content);

    static QString getIconName(Kind kind);
    static QString getIconColor(Kind kind);

private:
    PathSummary() = default;
};

} // namespace RapidNotes
