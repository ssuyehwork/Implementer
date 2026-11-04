#pragma once

#include <QString>
#include <QStringList>

namespace RapidNotes {

/**
 * @brief 标签编解码与格式唯一真理源 (SSOT)
 * 对应 docs/SSOT接口索引.md 第 5 章。
 */
class TagCodec {
public:
    // 拆分：同时支持半角逗号 `,` 和全角逗号 `，`
    static QStringList split(const QString& tagsStr);

    // 拼接：严格使用半角逗号 `,` 连接，无空格
    static QString join(const QStringList& tags);

    // 界面显示用拼接：使用半角逗号加空格 `, ` 连接
    static QString joinDisplay(const QStringList& tags);

private:
    TagCodec() = default;
};

} // namespace RapidNotes
