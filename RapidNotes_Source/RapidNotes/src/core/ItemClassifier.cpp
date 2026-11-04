#include "ItemClassifier.h"
#include <QFileInfo>
#include <QTextDocument>

namespace RapidNotes {

static QString stripHtml(const QString& text) {
    if (!text.contains('<')) return text;
    QTextDocument doc;
    doc.setHtml(text);
    return doc.toPlainText();
}

bool ItemClassifier::isUrl(const QString& text) {
    QString stripped = text.trimmed();
    return stripped.startsWith(QStringLiteral("http://"), Qt::CaseInsensitive) ||
           stripped.startsWith(QStringLiteral("https://"), Qt::CaseInsensitive) ||
           stripped.startsWith(QStringLiteral("www."), Qt::CaseInsensitive);
}

bool ItemClassifier::isHexColor(const QString& text) {
    static const QRegularExpression hexRegex(QStringLiteral("^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$"));
    return hexRegex.match(text.trimmed()).hasMatch();
}

bool ItemClassifier::isRgbTriplet(const QString& text, int* r, int* g, int* b) {
    static const QRegularExpression rgbRegex(QStringLiteral(R"(^(\d{1,3}),\s*(\d{1,3}),\s*(\d{1,3})$)"));
    QRegularExpressionMatch match = rgbRegex.match(text.trimmed());
    if (match.hasMatch()) {
        int rVal = match.captured(1).toInt();
        int gVal = match.captured(2).toInt();
        int bVal = match.captured(3).toInt();
        if (rVal <= 255 && gVal <= 255 && bVal <= 255) {
            if (r) *r = rVal;
            if (g) *g = gVal;
            if (b) *b = bVal;
            return true;
        }
    }
    return false;
}

bool ItemClassifier::isCodeLike(const QString& text) {
    QString plain = stripHtml(text).trimmed();
    if (plain.startsWith(QStringLiteral("import ")) ||
        plain.startsWith(QStringLiteral("class ")) ||
        plain.startsWith(QStringLiteral("def ")) ||
        plain.startsWith(QStringLiteral("function")) ||
        plain.startsWith(QStringLiteral("var ")) ||
        plain.startsWith(QStringLiteral("const "))) {
        return true;
    }

    if (plain.startsWith(QStringLiteral("<"))) {
        static const QRegularExpression htmlTagRegex(
            QStringLiteral("^<(html|div|p|span|table|h[1-6]|script|!DOCTYPE|body|head)"),
            QRegularExpression::CaseInsensitiveOption);
        if (htmlTagRegex.match(plain).hasMatch()) {
            return true;
        }
    }

    if (plain.startsWith(QStringLiteral("{")) &&
        plain.contains(QStringLiteral("\":")) &&
        plain.contains(QStringLiteral("}"))) {
        return true;
    }

    return false;
}

bool ItemClassifier::isAbsolutePathText(const QString& text) {
    QString cleanPath = text.trimmed();
    if ((cleanPath.startsWith('"') && cleanPath.endsWith('"')) ||
        (cleanPath.startsWith('\'') && cleanPath.endsWith('\''))) {
        cleanPath = cleanPath.mid(1, cleanPath.length() - 2);
    }
    return cleanPath.length() < 260 && (
        (cleanPath.length() > 2 && cleanPath[1] == ':') ||
        cleanPath.startsWith(QStringLiteral("\\")) ||
        cleanPath.startsWith('/') ||
        cleanPath.startsWith(QStringLiteral("./")) ||
        cleanPath.startsWith(QStringLiteral("../")));
}

ItemType::Type ItemClassifier::detectIngestType(const QString& text) {
    QString stripped = text.trimmed();
    if (isUrl(stripped)) {
        return ItemType::Type::Link;
    }

    QString plain = stripHtml(text).trimmed();
    if (isHexColor(plain)) {
        return ItemType::Type::Color;
    }

    if (isCodeLike(text)) {
        return ItemType::Type::Code;
    }

    if (isAbsolutePathText(stripped)) {
        QString cleanPath = stripped;
        if ((cleanPath.startsWith('"') && cleanPath.endsWith('"')) ||
            (cleanPath.startsWith('\'') && cleanPath.endsWith('\''))) {
            cleanPath = cleanPath.mid(1, cleanPath.length() - 2);
        }
        QFileInfo info(cleanPath);
        if (info.exists()) {
            return info.isDir() ? ItemType::Type::Folder : ItemType::Type::File;
        }
    }

    return ItemType::Type::Text;
}

DisplayCategory ItemClassifier::detectDisplayCategory(const QString& itemTypeStr, const QString& content) {
    ItemType::Type type = ItemType::fromString(itemTypeStr);
    QString strippedContent = content.trimmed();

    switch (type) {
    case ItemType::Type::Link:
        return {QStringLiteral("link"), QStringLiteral("#17B345")};
    case ItemType::Type::Code:
        return {QStringLiteral("code"), QStringLiteral("#00FF00")};
    case ItemType::Type::Color:
    case ItemType::Type::Palette:
        return {QStringLiteral("palette"), strippedContent};
    case ItemType::Type::OcrText:
        return {QStringLiteral("screenshot_ocr"), QStringLiteral("#00FFFF")};
    case ItemType::Type::CapturedMessage:
        return {QStringLiteral("message"), QStringLiteral("#00FFFF")};
    case ItemType::Type::PixelRuler:
        return {QStringLiteral("pixel_ruler"), QStringLiteral("#FF5722")};
    case ItemType::Type::LocalFile:
        return {QStringLiteral("file_import"), QStringLiteral("#FFFF00")};
    case ItemType::Type::LocalFolder:
        return {QStringLiteral("folder_import"), QStringLiteral("#FF8C00")};
    case ItemType::Type::LocalBatch:
        return {QStringLiteral("batch_import"), QStringLiteral("#FFFF00")};
    default:
        break;
    }

    // 纯文本兜底形态检测（完全不访问磁盘）
    if (isUrl(strippedContent)) {
        return {QStringLiteral("link"), QStringLiteral("#17B345")};
    }

    QString plain = stripHtml(content).trimmed();
    if (isHexColor(plain)) {
        return {QStringLiteral("palette"), plain};
    }

    if (isCodeLike(content)) {
        return {QStringLiteral("code"), QStringLiteral("#00FF00")};
    }

    return {QStringLiteral("text"), QStringLiteral("#95A5A6")};
}

} // namespace RapidNotes
