#include "PresetManager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QHash>
#include <QtGlobal>
#include "../core/AppConfig.h"

namespace QuarkMeta {

namespace {
struct TypeMapping {
    static const QHash<RenameComponentType, QString>& enumToString() {
        static const QHash<RenameComponentType, QString> map = {
            { RenameComponentType::Text, "Text" },
            { RenameComponentType::Sequence, "Sequence" },
            { RenameComponentType::OriginalName, "OriginalName" },
            { RenameComponentType::Date, "Date" },
            { RenameComponentType::Metadata, "Metadata" },
            { RenameComponentType::ParentFolderName, "ParentFolderName" }
        };
        return map;
    }

    static const QHash<QString, RenameComponentType>& stringToEnum() {
        static const QHash<QString, RenameComponentType> map = {
            { "Text", RenameComponentType::Text },
            { "Sequence", RenameComponentType::Sequence },
            { "OriginalName", RenameComponentType::OriginalName },
            { "Date", RenameComponentType::Date },
            { "Metadata", RenameComponentType::Metadata },
            { "ParentFolderName", RenameComponentType::ParentFolderName }
        };
        return map;
    }
};
}

QString PresetManager::serializeRules(const std::vector<RenameRule>& rules, int* failedCount) {
    if (failedCount) *failedCount = 0;
    QJsonArray arr;
    const auto& map = TypeMapping::enumToString();

    for (const auto& rule : rules) {
        if (!map.contains(rule.type)) {
            Q_ASSERT_X(false, "PresetManager::serializeRules", "Unmapped RenameComponentType enum value!");
            if (failedCount) (*failedCount)++;
            continue;
        }

        QJsonObject obj;
        obj["type"] = map.value(rule.type);
        obj["value"] = rule.value;
        obj["start"] = rule.start;
        obj["step"] = rule.step;
        obj["padding"] = rule.padding;
        arr.append(obj);
    }
    return QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact));
}

std::vector<RenameRule> PresetManager::deserializeRules(const QString& jsonStr, int* unrecognizedCount) {
    if (unrecognizedCount) *unrecognizedCount = 0;
    std::vector<RenameRule> rules;
    if (jsonStr.isEmpty()) return rules;

    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (!doc.isArray()) return rules;

    const auto& map = TypeMapping::stringToEnum();
    QJsonArray arr = doc.array();

    for (const auto& val : arr) {
        QJsonObject obj = val.toObject();
        QString typeStr = obj["type"].toString();

        if (!map.contains(typeStr)) {
            if (unrecognizedCount) (*unrecognizedCount)++;
            continue;
        }

        RenameRule rule;
        rule.type = map.value(typeStr);
        rule.value = obj["value"].toString();
        rule.start = obj["start"].toInt(1);
        rule.step = obj["step"].toInt(1);
        if (rule.step <= 0) rule.step = 1;
        rule.padding = obj["padding"].toInt(3);
        rules.push_back(rule);
    }
    return rules;
}

bool PresetManager::exportToFile(const QString& filePath, const std::vector<RenameRule>& rules) {
    if (filePath.isEmpty()) return false;
    QString jsonStr = serializeRules(rules);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonStr.toUtf8());
        file.close();
        return true;
    }
    return false;
}

std::vector<RenameRule> PresetManager::importFromFile(const QString& filePath, int* unrecognizedCount) {
    if (unrecognizedCount) *unrecognizedCount = 0;
    std::vector<RenameRule> rules;
    if (filePath.isEmpty()) return rules;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QString jsonStr = QString::fromUtf8(file.readAll());
        file.close();
        return deserializeRules(jsonStr, unrecognizedCount);
    }
    return rules;
}

} // namespace QuarkMeta
