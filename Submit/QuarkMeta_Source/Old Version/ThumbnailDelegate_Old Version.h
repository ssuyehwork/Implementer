#pragma once

#include "RenameCapableDelegate.h" 

namespace QuarkMeta {

class ThumbnailDelegate : public RenameCapableDelegate {
    Q_OBJECT
public:
    explicit ThumbnailDelegate(QObject* parent = nullptr);

    void setHasThumbnailRole(int role);
    void setRatingRole(int role);
    void setPathRole(int role);
    void setPinnedRole(int role);
    void setTypeRole(int role);
    void setIsEmptyRole(int role);
    void setColorRole(int role);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool helpEvent(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& option, const QModelIndex& index) override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

    struct Metrics {
        QRect cardRect;
        QRect textRect;
        QRect banRect;
        int starsStartX;
        int starSize;
        int starSpacing;
        int ratingY;
        int ratingH;

        QRect starRect(int index) const {
            return QRect(starsStartX + index * (starSize + starSpacing), 
                         ratingY + (ratingH - starSize) / 2, 
                         starSize, starSize);
        }
    };
    Metrics calculateMetrics(const QStyleOptionViewItem& option) const;

private:
    void drawFileNameText(QPainter* painter, const QRect& textRect, bool isSelected, const QModelIndex& index, const QStyleOptionViewItem& option) const;

    int m_hasThumbnailRole = Qt::UserRole + 1;
    int m_ratingRole = -1;
    int m_pathRole = -1;
    int m_pinnedRole = -1;
    int m_typeRole = -1;
    int m_isEmptyRole = -1;
    int m_colorRole = -1;
};

} // namespace QuarkMeta
