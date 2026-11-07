#pragma once

#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>

struct sqlite3;

namespace QuarkMeta {

enum class SmartRuleType {
    Rating = 0,
    Color = 1,
    Tag = 2,
    Extension = 3,
    Name = 4,        // 原 Keyword，保持枚举整数值 4 不变
    Folder = 5,
    FolderName = 6,
    AddedDate = 7,
    ModifiedDate = 8,
    CreatedDate = 9,
    Link = 10,
    Note = 11,
    Width = 12,
    Height = 13,
    FileSize = 14,
    AutoColor = 15    // 图像颜色 (自动算法提取)
};

enum class SmartRuleOperator {
    Equals = 0,
    GreaterOrEqual = 1,
    LessOrEqual = 2,
    Contains = 3,
    NotContains = 4,
    StartsWith = 5,
    EndsWith = 6,
    IsFolder = 7,
    IsNotFolder = 8,
    Before = 9,
    After = 10,
    Between = 11,
    RecentDays = 12,
    IsEmpty = 13,
    IsNotEmpty = 14
};

struct SmartFolderRule {
    SmartRuleType type = SmartRuleType::Rating;
    SmartRuleOperator op = SmartRuleOperator::GreaterOrEqual;
    QString value;
    QStringList tags;

    // 存取属性字段
    QString date1;  // yyyy-MM-dd
    QString date2;  // yyyy-MM-dd
    int days = 0;   // 最近 N 天
    qint64 num1 = 0;
    qint64 num2 = 0;
    QString unit;   // "KB", "MB", "GB"
    int tolerance = 50; // 图像颜色精准度 (原名 tolerance)
    int minArea = 0;    // 图像颜色占比 (0~100)

    QJsonObject toJson() const;
    static SmartFolderRule fromJson(const QJsonObject& obj);
};

struct SmartFolderRecord {
    int id = 0;
    QString name = "新智能文件夹";
    QString matchMode = "ALL"; // "ALL" (AND) 或 "ANY" (OR)
    QString iconKey = "smart-folder-1.svg";
    QString colorHex = "#888888";
    int sortOrder = 0;
    QList<SmartFolderRule> rules;
};

class SmartFolderDao {
public:
    static bool initTable();
    static QList<SmartFolderRecord> getAllSmartFolders();
    static SmartFolderRecord getSmartFolderById(int id);
    static int addSmartFolder(const SmartFolderRecord& rec);
    static bool updateSmartFolder(const SmartFolderRecord& rec);
    static bool updateSmartFolderOrder(const QList<QPair<int, int>>& idOrderList);
    static bool removeSmartFolderById(int id);
    static int countMatchingItems(const SmartFolderRecord& rec);
    static QMap<int, int> countMatchingItemsBatchReadOnly(const QString& dbPath, const QList<SmartFolderRecord>& records);
    static QStringList getMatchingPaths(const SmartFolderRecord& rec);

    // SQL 条件独立构建单元
    static QString buildRatingCondition(const SmartFolderRule& rule);
    static QString buildNameCondition(const SmartFolderRule& rule);
    static QString buildFolderNameCondition(const SmartFolderRule& rule);
    static QString buildFolderCondition(const SmartFolderRule& rule);
    static QString buildDateCondition(const QString& colName, const SmartFolderRule& rule);
    static QString buildNumericCondition(const QString& colName, const SmartFolderRule& rule);
    static QString buildFileSizeCondition(const SmartFolderRule& rule);
    static QString buildTextPropertyCondition(const QString& colName, const SmartFolderRule& rule);
    static QString buildColorCondition(const SmartFolderRule& rule);
    static QString buildAutoColorCondition(const SmartFolderRule& rule);
    static QString buildExtensionCondition(const SmartFolderRule& rule);
    static QString buildTagCondition(const SmartFolderRule& rule);

    static bool registerColorFunctions(sqlite3* db);

private:
    static QString buildAutoColorAreaExpression(const QString& targetHexEscaped, int precision);
    static QString buildWhereClause(const SmartFolderRecord& rec);
};

} // namespace QuarkMeta
