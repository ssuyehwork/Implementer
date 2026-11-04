#include "ItemType.h"
#include "WordBucket.h"
#include <QRegularExpression>
#include <QtGlobal>

namespace RapidNotes {

int WordBucket::calculate(const QString& content, const QString& itemType) {
    // 1. 类型拦截：非正文相关类型直接归为“无字数” (0)
    if (!ItemType::isPureText(ItemType::fromString(itemType))) {
        return 0;
    }

    QString plainText = content;

    // 2. 连锅端：清除 <style> 和 <script> 及其内部的所有排版代码
    plainText.remove(QRegularExpression(QStringLiteral("<style[^>]*>.*?</style>"),
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption));
    plainText.remove(QRegularExpression(QStringLiteral("<script[^>]*>.*?</script>"),
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption));

    // 3. 剥离：清除普通 HTML 标签
    plainText.remove(QRegularExpression(QStringLiteral("<[^>]*>")));

    // 4. 清洗：清除常见的 HTML 实体乱码（如 &nbsp; &amp;）
    plainText.remove(QRegularExpression(QStringLiteral("&[a-zA-Z]+;")));

    // 5. 压缩：清除所有不可见的空白字符（空格、换行等）
    plainText.remove(QRegularExpression(QStringLiteral("\\s+")));

    int len = plainText.length();

    // 6. 阶梯映射
    if (len == 0) return 0;
    if (len <= 100) return (qMax(0, len - 1) / 10 + 1) * 10;
    if (len <= 1000) return ((len - 1) / 100 + 1) * 100;

    return 1001; // 1000 字以上
}

} // namespace RapidNotes
