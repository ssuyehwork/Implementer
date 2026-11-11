#pragma once

#include <QString>
#include <QList>
#include <QVariantMap>
#include <QMap>
#include <memory>

namespace RapidNotes {

struct NoteItemDTO {
    qint64 id = 0;
    QString title;
    QString summary; // 前200字纯文本摘要
    QString tags;
    int rating = 0;
    QString colorLabel; // color_label
    bool isPinned = false;
    QString itemType;
    QString createdAt;
    QString updatedAt;
    QString remark;
    int wordCount = 0;
};

struct CategoryCountDTO {
    int categoryId = 0;
    int directCount = 0;
};

class NoteRepository {
public:
    static NoteRepository& instance();

    // 1. 分类统计（只调用一次取得所有分类数量，只统计直属笔记，不递归子分类）
    QMap<int, int> getCategoryCounts();

    // 2. 笔记轻量分页/分批加载（不读 content 大正文和 blob，只取前200字摘要与元数据）
    QList<NoteItemDTO> loadNotesLight(int categoryId, int offset = 0, int limit = 500, const QString& searchKeyword = "");

    // 3. 元数据单列精准更新（只更新被修改的一列，不改变 updated_at 与 last_accessed_at）
    bool updateRating(qint64 noteId, int rating);
    bool updateColorLabel(qint64 noteId, const QString& colorLabel);
    bool updateTags(qint64 noteId, const QString& tags);
    bool updateRemark(qint64 noteId, const QString& remark);
    bool updatePinned(qint64 noteId, bool isPinned);
    bool updateTitle(qint64 noteId, const QString& title);

    // 批量更新同一字段
    bool batchUpdateRating(const QList<qint64>& noteIds, int rating);
    bool batchUpdateColorLabel(const QList<qint64>& noteIds, const QString& colorLabel);
    bool batchUpdateTags(const QList<qint64>& noteIds, const QString& tags);

    // 4. 移动笔记到分类
    bool moveNotesToCategory(const QList<qint64>& noteIds, int targetCategoryId);

    // 5. 分类操作（拖拽重排、重命名、新增、置顶）
    bool updateCategoryOrder(const QList<int>& orderedIds);
    bool renameCategory(int categoryId, const QString& newName);
    bool setCategoryIcon(int categoryId, const QString& iconKey);
    bool toggleCategoryPinned(int categoryId, bool isPinned);

    // 笔记全量属性与元数据查询回显
    QVariantMap getNoteById(int id);

    // 数据库路径获取
    static QString getDatabasePath();

private:
    NoteRepository();
    ~NoteRepository() = default;
};

} // namespace RapidNotes
