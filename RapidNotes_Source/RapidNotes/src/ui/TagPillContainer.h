#pragma once

#include <QFrame>
#include <QStringList>
#include "components/FlowLayout.h"
#include "TagSelectorOverlay.h"

namespace QuarkMeta {

class TagPillContainer : public QFrame {
    Q_OBJECT
public:
    explicit TagPillContainer(QWidget* parent = nullptr, TagTriggerSource source = TagTriggerSource::PresetTagsDialog);
    ~TagPillContainer() override = default;

    void setTags(const QStringList& tags);
    QStringList tags() const;


signals:
    void tagsChanged(const QStringList& tags);
    void containerHeightChanged();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void populateTagPills();
    void onTagContainerClicked();
    void onTagDeleted(const QString& tag);

    QStringList m_tags;
    TagTriggerSource m_triggerSource;
    FlowLayout* m_flowLayout = nullptr;
    TagSelectorOverlay* m_selectorOverlay = nullptr;
};

} // namespace QuarkMeta
