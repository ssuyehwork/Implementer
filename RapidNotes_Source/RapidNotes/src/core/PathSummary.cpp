#include "PathSummary.h"
#include <QFileInfo>

namespace RapidNotes {

PathSummary::Kind PathSummary::summarize(const QStringList& paths) {
    if (paths.isEmpty()) {
        return Kind::SingleFile;
    }

    if (paths.size() == 1) {
        QString clean = paths.first().trimmed();
        if ((clean.startsWith('"') && clean.endsWith('"')) ||
            (clean.startsWith('\'') && clean.endsWith('\''))) {
            clean = clean.mid(1, clean.length() - 2);
        }
        if (clean.endsWith('/') || clean.endsWith('\\')) {
            clean.chop(1);
        }
        QFileInfo fi(clean);
        return fi.isDir() ? Kind::SingleFolder : Kind::SingleFile;
    }

    int dirCount = 0;
    for (const QString& p : paths) {
        QString clean = p.trimmed();
        if ((clean.startsWith('"') && clean.endsWith('"')) ||
            (clean.startsWith('\'') && clean.endsWith('\''))) {
            clean = clean.mid(1, clean.length() - 2);
        }
        if (clean.endsWith('/') || clean.endsWith('\\')) {
            clean.chop(1);
        }
        if (QFileInfo(clean).isDir()) {
            dirCount++;
        }
    }

    if (dirCount == paths.size()) {
        return Kind::MultipleFolders;
    }
    return Kind::MultipleFilesOrMixed;
}

PathSummary::Kind PathSummary::summarizeFromSemicolonString(const QString& content) {
    QStringList paths = content.split(';', Qt::SkipEmptyParts);
    return summarize(paths);
}

QString PathSummary::getIconName(Kind kind) {
    switch (kind) {
    case Kind::SingleFolder:         return QStringLiteral("folder");
    case Kind::MultipleFolders:      return QStringLiteral("folders_multiple");
    case Kind::MultipleFilesOrMixed: return QStringLiteral("files_multiple");
    case Kind::SingleFile:
    default:                         return QStringLiteral("file");
    }
}

QString PathSummary::getIconColor(Kind kind) {
    switch (kind) {
    case Kind::SingleFolder:         return QStringLiteral("#FF8C00"); // 橙色 (Hue 33)
    case Kind::MultipleFolders:      return QStringLiteral("#483D8B"); // 深紫蓝色 (Hue 248)
    case Kind::MultipleFilesOrMixed: return QStringLiteral("#FF0000"); // 纯红色 (Hue 0)
    case Kind::SingleFile:
    default:                         return QStringLiteral("#FFFF00"); // 纯黄色 (Hue 60)
    }
}

} // namespace RapidNotes
