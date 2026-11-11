#ifndef QUICKMEDIATOR_H
#define QUICKMEDIATOR_H

#include <QObject>
#include <QList>
#include <QStringList>
#include <QVariantMap>

class QuickWindow;

namespace RapidNotes {

class QuickMediator : public QObject {
    Q_OBJECT
public:
    explicit QuickMediator(QuickWindow* window, QObject* parent = nullptr);
    ~QuickMediator() override = default;

    void requestMoveToCategory(const QList<int>& ids, int categoryId);
    void requestToggleFavorite(const QList<int>& ids);
    void requestMoveToTrash(const QList<int>& ids);
    void requestPermanentDelete(const QList<int>& ids);
    void requestSetRating(const QList<int>& ids, int rating);
    void requestSetColor(const QList<int>& ids, const QString& color);
    void requestSetTags(const QList<int>& ids, const QStringList& tags);

private slots:
    void onMetadataUpdated(const QStringList& paths);
    void onItemsDeleted(const QStringList& paths);

private:
    QuickWindow* m_window = nullptr;
};

} // namespace RapidNotes

#endif // QUICKMEDIATOR_H
