#include "NoteIngestService.h"
#include "DatabaseManager.h"
#include "TagCodec.h"
#include "WordBucket.h"
#include "ItemType.h"
#include <QCryptographicHash>

namespace RapidNotes {

NoteIngestService& NoteIngestService::instance() {
    static NoteIngestService s_instance;
    return s_instance;
}

NoteIngestService::NoteIngestService(QObject* parent) : QObject(parent) {
}

int NoteIngestService::ingestNote(const QString& title, const QString& content, const QStringList& tags,
                                 const QString& color, int categoryId, const QString& itemType,
                                 const QByteArray& blob, const QString& sourceApp,
                                 const QString& sourceTitle, const QString& remark) {
    // 履行入库业务指挥中心职责：执行清洗规范化与类型核验
    QStringList normalizedTags = TagCodec::normalize(tags);
    QString effectiveType = itemType.isEmpty() ? ItemType::toString(ItemType::Type::Text) : itemType;

    // 委托底层完成单事务哈希入库
    return DatabaseManager::instance().addNote(title, content, normalizedTags, color, categoryId,
                                               effectiveType, blob, sourceApp, sourceTitle, remark);
}

bool NoteIngestService::updateNote(int noteId, const QString& title, const QString& content, const QStringList& tags,
                                  const QString& color, int categoryId, const QString& itemType,
                                  const QByteArray& blob, const QString& sourceApp, const QString& sourceTitle,
                                  const QString& remark) {
    QStringList normalizedTags = TagCodec::normalize(tags);
    bool ok = DatabaseManager::instance().updateNote(noteId, title, content, normalizedTags, color,
                                                    categoryId, itemType, blob, sourceApp, sourceTitle, remark);
    if (ok) {
        DatabaseManager::instance().recordAccess(noteId);
    }
    return ok;
}

} // namespace RapidNotes
