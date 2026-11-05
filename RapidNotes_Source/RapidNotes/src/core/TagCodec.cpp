#include "TagCodec.h"
#include <QRegularExpression>

namespace RapidNotes {

QStringList TagCodec::normalize(const QStringList& tags) {
    QStringList result;
    merge(result, tags);
    return result;
}

void TagCodec::merge(QStringList& target, const QStringList& incoming, bool* changed) {
    bool hasChanged = false;
    for (const QString& t : incoming) {
        QString trimmed = t.trimmed();
        if (!trimmed.isEmpty() && !target.contains(trimmed)) {
            target.append(trimmed);
            hasChanged = true;
        }
    }
    if (changed) {
        *changed = hasChanged;
    }
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
