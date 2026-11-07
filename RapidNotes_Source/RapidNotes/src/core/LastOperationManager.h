#ifndef QuarkMeta_LAST_OPERATION_MANAGER_H
#define QuarkMeta_LAST_OPERATION_MANAGER_H

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QFileInfo>

namespace QuarkMeta {

enum class LastOperationType {
    None,
    SetRating,
    SetColor,
    PasteTags,
    MoveToFolder
};

class LastOperationManager {
public:
    static LastOperationManager& instance() {
        static LastOperationManager inst;
        return inst;
    }

    bool hasOperation() const {
        return m_type != LastOperationType::None;
    }

    LastOperationType type() const { return m_type; }
    int rating() const { return m_rating; }
    QString color() const { return m_color; }
    QStringList tags() const { return m_tags; }
    QString destination() const { return m_destination; }

    void recordSetRating(int rating) {
        m_type = LastOperationType::SetRating;
        m_rating = rating;
    }

    void recordSetColor(const QString& color) {
        m_type = LastOperationType::SetColor;
        m_color = color;
    }

    void recordPasteTags(const QStringList& tags) {
        m_type = LastOperationType::PasteTags;
        m_tags = tags;
    }

    void recordMoveToFolder(const QString& destination) {
        m_type = LastOperationType::MoveToFolder;
        m_destination = destination;
    }

    QString displayText() const {
        switch (m_type) {
            case LastOperationType::SetRating:
                return QString("重复上一次操作 (%1 星)").arg(m_rating);
            case LastOperationType::SetColor:
                return m_color.isEmpty() ? QString("重复上一次操作 (清除色标)") : QString("重复上一次操作 (标记颜色)");
            case LastOperationType::PasteTags:
                return QString("重复上一次操作 (粘贴 %1 个标签)").arg(m_tags.size());
            case LastOperationType::MoveToFolder: {
                QString folderName = QFileInfo(m_destination).fileName();
                return QString("重复上一次操作 (移入到: %1)").arg(folderName.isEmpty() ? m_destination : folderName);
            }
            default:
                return "重复上一次操作";
        }
    }

private:
    LastOperationManager() = default;
    ~LastOperationManager() = default;
    LastOperationManager(const LastOperationManager&) = delete;
    LastOperationManager& operator=(const LastOperationManager&) = delete;

    LastOperationType m_type{LastOperationType::None};
    int m_rating{0};
    QString m_color;
    QStringList m_tags;
    QString m_destination;
};

} // namespace QuarkMeta

#endif // QuarkMeta_LAST_OPERATION_MANAGER_H
