#pragma once

#include <QString>

namespace RapidNotes {

/**
 * @brief 字数阶梯计算唯一真理源 (SSOT)
 * 对应 docs/SSOT接口索引.md 第 4 章。
 */
class WordBucket {
public:
    static int calculate(const QString& content, const QString& itemType);

private:
    WordBucket() = default;
};

} // namespace RapidNotes
