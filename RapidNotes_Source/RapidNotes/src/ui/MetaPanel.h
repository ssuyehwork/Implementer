#pragma once

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QTimer>
#include <QEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QLineEdit>
#include <QPushButton>
#include <QPointer>
#include <QAction>
#include <QSet>
#include <QPixmap>
#include <QIcon>
#include <QList>
#include "components/ElasticEdit.h"
#include "TagSelectorOverlay.h"
#include "components/TagPill.h"
#include "components/FlowLayout.h"
#include "components/ColorPill.h"

namespace QuarkMeta {

struct MetaPreviewItem {
    bool hasThumbnail = false;
    QPixmap thumbnail;
    QIcon icon;
};

class ColorStripPicker;

class MetaPanel : public QFrame {
    Q_OBJECT
public:
    static constexpr int kMultiPreviewMaxCards = 5;

    explicit MetaPanel(QWidget* parent = nullptr);
    ~MetaPanel() override = default;

    // 🚀【物理契约】：刚性锁定 230px 下限，杜绝被 QSplitter 挤压偷扣像素
    QSize minimumSizeHint() const override { return QSize(230, 100); }

    void updateInfo(const QString& name, const QString& type, const QString& size,
                    const QString& ctime, const QString& mtime, const QString& atime,
                    const QString& path, bool encrypted, int width = 0, int height = 0);

    void setImagePreview(const QPixmap& pixmap);
    void setDefaultIconPreview(const QIcon& icon);
    void setMultiSelectionPreview(const QList<MetaPreviewItem>& items);
    void setSelectedPaths(const QStringList& paths);
    void setPalettes(const QVector<QPair<QColor, float>>& palette);
    void setTags(const QStringList& tags);
    void setNote(const QString& note);
    void setURL(const QString& url);
    void setRating(int rating, bool fromUser = false);
    void setColor(const QString& hexColor, bool fromUser = false);
    void setPinned(bool pinned) { Q_UNUSED(pinned); }

signals:
    void ratingChanged(const QStringList& paths, int rating);
    void colorChanged(const QStringList& paths, const QString& hexColor);
    void tagAddRequested(const QStringList& paths, const QString& tag);
    void tagRemoveRequested(const QStringList& paths, const QString& tag);
    void tagsAddRequested(const QStringList& paths, const QStringList& tags);
    void tagsRemoveRequested(const QStringList& paths, const QStringList& tags);
    void noteEdited(const QStringList& paths, const QString& newNote);
    void linkEdited(const QStringList& paths, const QString& newLink);
    void primaryColorChanged(const QString& path, const QColor& color);
    void renameRequested(const QString& oldPath, const QString& newPath);
    void searchByColor(const QColor& color);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void initUi();
    void updateControlsState(bool hasSelection, bool isMultiSelection, bool isReadOnly);
    void adjustFlowHeights();
    void addInfoRow(QVBoxLayout* layout, const QString& label, QLabel*& valueLabel);
    QWidget* createCollapsibleSection(const QString& title, QWidget* contentWidget, bool defaultExpanded = true);

    QVBoxLayout* m_mainLayout = nullptr;
    QScrollArea* m_scrollArea = nullptr;
    QWidget* m_container = nullptr;
    QVBoxLayout* m_containerLayout = nullptr;

    QWidget* m_topPreviewBox = nullptr;
    QLabel* m_lblImagePreview = nullptr;
    QWidget* m_paletteContainer = nullptr;
    FlowLayout* m_paletteFlowLayout = nullptr;

    ElasticEdit* m_nameEdit = nullptr;
    ElasticEdit* m_noteEdit = nullptr;

    QWidget* m_linkBox = nullptr;
    QLineEdit* m_linkEdit = nullptr;
    QAction* m_actOpenLink = nullptr;

    QWidget* m_ratingColorBox = nullptr;
    QList<QPushButton*> m_starBtns;
    ColorStripPicker* m_colorStripPicker = nullptr;
    int m_currentRating = 0;
    QString m_currentColorHex;

    QWidget* m_tagBox = nullptr;
    QWidget* m_tagContainer = nullptr;
    FlowLayout* m_tagFlowLayout = nullptr;
    QPushButton* m_btnAddTagBig = nullptr;
    QPushButton* m_btnAddTagSmall = nullptr;
    QPointer<TagSelectorOverlay> m_tagSelectorOverlay;

    QWidget* m_infoSectionWidget = nullptr;
    QLabel* lblType = nullptr;
    QLabel* lblSize = nullptr;
    QLabel* lblDimensions = nullptr;
    QLabel* lblCtime = nullptr;
    QLabel* lblMtime = nullptr;
    QLabel* lblAtime = nullptr;
    QLabel* lblEncrypted = nullptr;

    ElasticEdit* m_pathEdit = nullptr;
    QPushButton* m_btnCopyPath = nullptr;
    QPushButton* m_btnOpenLocation = nullptr;

    QStringList m_selectedPaths;
    QStringList m_editingPathsSnapshot;
    QSet<QString> m_currentTagsSet;
    QTimer* m_adjustTimer = nullptr;
    bool m_isInternalUpdating = false;
    bool m_isUserEditing = false;
    bool m_isReadOnlyMode = false;

    QSet<QString> m_pendingRemovedTags;
    QStringList m_tagRemovalTargetSnapshot;
    QTimer* m_tagRemoveDebounceTimer = nullptr;

    QWidget* m_dragOverlayWidget = nullptr;
    QLabel* m_dragOverlayLabel = nullptr;
    void showDragOverlay(int activeCount);
    void hideDragOverlay();
    void installRecursiveDragEventFilter(QWidget* widget);

private slots:
    void onTagDeleted(const QString& text);
    void flushPendingTagDeletions();
    void openTagSelectorOverlay(QWidget* targetAnchor);
    void setAsPrimaryColor(const QColor& color);
};

} // namespace QuarkMeta