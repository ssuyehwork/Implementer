#ifndef RECENTNOTESDELEGATE_H
#define RECENTNOTESDELEGATE_H

#include <QStyledItemDelegate>

class RecentNotesDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit RecentNotesDelegate(QObject* parent = nullptr);

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool helpEvent(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& option, const QModelIndex& index) override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // RECENTNOTESDELEGATE_H
