#pragma once

#include <QString>

namespace RapidNotes {

/**
 * @brief 条目类型词表唯一真理源 (SSOT)
 * 对应 docs/SSOT接口索引.md 第 1 章。全工程唯一允许出现 item_type 字符串字面量的地方。
 */
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
    // 判断是否为历史 5 种文件类型集合 (file, local_file, local_batch, folder, local_folder)，不含 files/folders
    static bool isLegacyFiveFileTypes(Type type);
    static bool isPureText(Type type);

private:
    ItemType() = default;
};

} // namespace RapidNotes
