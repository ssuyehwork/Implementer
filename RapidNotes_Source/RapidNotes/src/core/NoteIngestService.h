#ifndef RAPIDNOTES_NOTEINGESTSERVICE_H
#define RAPIDNOTES_NOTEINGESTSERVICE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QVariantMap>

namespace RapidNotes {

class NoteIngestService : public QObject {
    Q_OBJECT
public:
    static NoteIngestService& instance();

    int ingestNote(const QString& title, const QString& content, const QStringList& tags,
                   const QString& color, int categoryId, const QString& itemType,
                   const QByteArray& blob = QByteArray(), const QString& sourceApp = "",
                   const QString& sourceTitle = "", const QString& remark = "");

    bool updateNote(int noteId, const QString& title, const QString& content, const QStringList& tags,
                    const QString& color, int categoryId, const QString& itemType,
                    const QByteArray& blob, const QString& sourceApp, const QString& sourceTitle,
                    const QString& remark);

private:
    explicit NoteIngestService(QObject* parent = nullptr);
    ~NoteIngestService() override = default;
};

} // namespace RapidNotes

#endif // RAPIDNOTES_NOTEINGESTSERVICE_H
