#include "ItemType.h"

namespace RapidNotes {

QString ItemType::toString(Type type) {
    switch (type) {
    case Type::Text:            return QStringLiteral("text");
    case Type::Image:           return QStringLiteral("image");
    case Type::Code:            return QStringLiteral("code");
    case Type::Link:            return QStringLiteral("link");
    case Type::Color:           return QStringLiteral("color");
    case Type::Palette:         return QStringLiteral("palette");
    case Type::OcrText:         return QStringLiteral("ocr_text");
    case Type::CapturedMessage: return QStringLiteral("captured_message");
    case Type::PixelRuler:      return QStringLiteral("pixel_ruler");
    case Type::DeletedCategory: return QStringLiteral("deleted_category");
    case Type::File:            return QStringLiteral("file");
    case Type::Files:           return QStringLiteral("files");
    case Type::Folder:          return QStringLiteral("folder");
    case Type::Folders:         return QStringLiteral("folders");
    case Type::LocalFile:       return QStringLiteral("local_file");
    case Type::LocalFolder:     return QStringLiteral("local_folder");
    case Type::LocalBatch:      return QStringLiteral("local_batch");
    case Type::Unknown:         return QStringLiteral("text");
    }
    return QStringLiteral("text");
}

ItemType::Type ItemType::fromString(const QString& str) {
    QString s = str.trimmed().toLower();
    if (s == QStringLiteral("text"))             return Type::Text;
    if (s == QStringLiteral("image"))            return Type::Image;
    if (s == QStringLiteral("code"))             return Type::Code;
    if (s == QStringLiteral("link"))             return Type::Link;
    if (s == QStringLiteral("color"))            return Type::Color;
    if (s == QStringLiteral("palette"))          return Type::Palette;
    if (s == QStringLiteral("ocr_text"))         return Type::OcrText;
    if (s == QStringLiteral("captured_message")) return Type::CapturedMessage;
    if (s == QStringLiteral("pixel_ruler"))      return Type::PixelRuler;
    if (s == QStringLiteral("deleted_category")) return Type::DeletedCategory;
    if (s == QStringLiteral("file"))             return Type::File;
    if (s == QStringLiteral("files"))            return Type::Files;
    if (s == QStringLiteral("folder"))           return Type::Folder;
    if (s == QStringLiteral("folders"))          return Type::Folders;
    if (s == QStringLiteral("local_file"))       return Type::LocalFile;
    if (s == QStringLiteral("local_folder"))     return Type::LocalFolder;
    if (s == QStringLiteral("local_batch"))      return Type::LocalBatch;
    return Type::Unknown;
}

bool ItemType::isFileOrFolder(Type type) {
    return type == Type::File || type == Type::Files ||
           type == Type::Folder || type == Type::Folders ||
           type == Type::LocalFile || type == Type::LocalFolder ||
           type == Type::LocalBatch;
}

bool ItemType::isLegacyFiveFileTypes(Type type) {
    return type == Type::File || type == Type::LocalFile ||
           type == Type::LocalBatch || type == Type::Folder ||
           type == Type::LocalFolder;
}

bool ItemType::isPureText(Type type) {
    return type == Type::Text || type == Type::Code ||
           type == Type::OcrText || type == Type::CapturedMessage;
}

} // namespace RapidNotes
