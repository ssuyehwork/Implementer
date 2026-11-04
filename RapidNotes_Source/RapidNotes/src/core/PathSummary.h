#pragma once

#include <QString>
#include <QStringList>

namespace RapidNotes {

/**
 * @brief 路径种类与图标归并唯一真理源 (SSOT)
 * 对应 docs/SSOT接口索引.md 第 3 章。
 */
class PathSummary {
public:
    enum class Kind {
        SingleFile,
        SingleFolder,
        MultipleFilesOrMixed,
        MultipleFolders
    };

    static Kind summarize(const QStringList& paths);
    static Kind summarizeFromSemicolonString(const QString& content);

    static QString getIconName(Kind kind);
    static QString getIconColor(Kind kind);

private:
    PathSummary() = default;
};

} // namespace RapidNotes
