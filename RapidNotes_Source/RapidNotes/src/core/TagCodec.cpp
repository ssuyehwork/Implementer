#include "TagCodec.h"
#include <QRegularExpression>

namespace RapidNotes {

QStringList TagCodec::normalize(const QStringList& tags) {
    QStringList result;
    for (const QString& t : tags) {
        QString trimmed = t.trimmed();
        if (!trimmed.isEmpty() && !result.contains(trimmed)) {
            result.append(trimmed);
        }
    }
    return result;
}

QStringList TagCodec::split(const QString& tagsStr) {
    if (tagsStr.trimmed().isEmpty()) return QStringList();
    static const QRegularExpression splitRegex(QStringLiteral("[,，]"));
    QStringList rawList = tagsStr.split(splitRegex, Qt::SkipEmptyParts);
    return normalize(rawList);
}

QString TagCodec::join(const QStringList& tags) {
    return tags.join(QLatin1Char(','));
}

QString TagCodec::joinDisplay(const QStringList& tags) {
    return tags.join(QStringLiteral(", "));
}

} // namespace RapidNotes
