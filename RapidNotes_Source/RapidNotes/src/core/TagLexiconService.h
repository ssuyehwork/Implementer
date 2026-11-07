#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>

namespace QuarkMeta {

struct TagEntry {
    int id = -1;
    QString name;
    int groupId = -1;
    QString colorHex;
    int sortOrder = 0;
};

struct TagGroup {
    int id = -1;
    QString name;
    QString colorHex;
    int sortOrder = 0;
    QList<TagEntry> tags;
};

using TagLexiconGroup = TagGroup;

class TagLexiconService : public QObject {
    Q_OBJECT

public:
    static TagLexiconService& instance();

    // 1. Fast auto-completion and querying (MSVC Rebuild Trigger 2026-08-28)
    QStringList querySuggestions(const QString& prefix = "", int limit = 20) const;
    QList<TagGroup> getAllTagGroups() const;
    QStringList getAllTagNames() const;
    QStringList getAllMasterTags() const;

    // 2. Lexicon dictionary CRUD
    bool addTag(const QString& tagName, int groupId = -1, const QString& colorHex = "");
    bool addTags(const QStringList& tags, int groupId = -1, const QString& colorHex = "");
    bool renameTag(const QString& oldName, const QString& newName);
    bool deleteTag(const QString& tagName);
    bool setTagColor(const QString& tagName, const QString& colorHex);

    // 3. Tag group management
    int createGroup(const QString& groupName, const QString& colorHex = "");
    bool renameGroup(int groupId, const QString& newName);
    bool deleteGroup(int groupId);
    bool moveTagToGroup(const QString& tagName, int targetGroupId);
    bool addTagToGroup(const QString& tagName, int targetGroupId);
    bool removeTagFromGroup(const QString& tagName, int groupId = -1);

signals:
    void lexiconChanged();

private:
    explicit TagLexiconService(QObject* parent = nullptr) : QObject(parent) {}
    ~TagLexiconService() override = default;
    TagLexiconService(const TagLexiconService&) = delete;
    TagLexiconService& operator=(const TagLexiconService&) = delete;
};

} // namespace QuarkMeta
