#pragma once

#include <QString>

namespace RapidNotes {

class ItemType {
public:
    enum class Type {
        Text,
        Image,
        Code,
        Link,
        Color,
        Palette,
        OcrText,
        CapturedMessage,
        PixelRuler,
        DeletedCategory,
        // 旧文件类中间状态（保留至步骤3统一合并为 Path）
        File,
        Files,
        Folder,
        Folders,
        LocalFile,
        LocalFolder,
        LocalBatch,
        Unknown
    };

    static QString toString(Type type);
    static Type fromString(const QString& str);

    static bool isFileOrFolder(Type type);
    static bool isPureText(Type type);

private:
    ItemType() = default;
};

} // namespace RapidNotes
