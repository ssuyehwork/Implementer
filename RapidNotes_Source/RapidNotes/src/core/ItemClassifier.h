#pragma once

#include <QString>
#include <QRegularExpression>
#include "ItemType.h"

namespace RapidNotes {

struct DisplayCategory {
    QString iconName;
    QString iconColor;
};

/**
 * @brief 文本特征与类型判定唯一真理源 (SSOT)
 * 对应 docs/SSOT接口索引.md 第 2 章。
 */
class ItemClassifier {
public:
    static bool isUrl(const QString& text);
    static bool isHexColor(const QString& text);
    static bool isRgbTriplet(const QString& text, int* r = nullptr, int* g = nullptr, int* b = nullptr);
    static bool isCodeLike(const QString& text);
    static bool isAbsolutePathText(const QString& text);

    // 入库用的类型判定（支持纯文本与文件路径判定）
    static ItemType::Type detectIngestType(const QString& text);

    // 界面图标用的显示类别（严禁访问磁盘 I/O）
    static DisplayCategory detectDisplayCategory(const QString& itemTypeStr, const QString& content);

private:
    ItemClassifier() = default;
};

} // namespace RapidNotes
