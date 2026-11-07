#pragma once
#include "ActionCommand.h"
#include "../../meta/MetadataManager.h"
#include <QString>
#include <QVariant>

namespace QuarkMeta {

class MetadataCommand : public ActionCommand {
public:
    enum Type { Rating, Color };
    MetadataCommand(const QString& path, Type type, const QVariant& oldVal, const QVariant& newVal)
        : m_path(path), m_type(type), m_oldVal(oldVal), m_newVal(newVal) {}

    void execute() override {}

    void undo() override {
        applyValue(m_oldVal);
    }

    void redo() override {
        applyValue(m_newVal);
    }

    QString description() const override { return m_type == Rating ? "更改星级" : "更改颜色"; }

    bool affectsPath(const QString& path) const override {
        return m_path == path;
    }

private:
    void applyValue(const QVariant& val) {
        if (m_type == Rating) {
            MetadataManager::instance().setRating(m_path.toStdWString(), val.toInt());
        } else {
            MetadataManager::instance().setColor(m_path.toStdWString(), val.toString().toStdWString());
        }
    }

    QString m_path;
    Type m_type;
    QVariant m_oldVal;
    QVariant m_newVal;
};

} // namespace QuarkMeta
