#include "PresetTagsDialog.h"
#include "UiHelper.h"
#include "../core/DatabaseManager.h"
#include "../core/TagCodec.h"
#include "ToolTipOverlay.h"
#include <QCursor>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <QTimer>
#include <QKeyEvent>
#include <QListWidget>
#include <QScrollArea>

namespace RapidNotes {

PresetTagsDialog::PresetTagsDialog(int categoryId, QWidget* parent, bool isLibrary)
    : FramelessDialog("设置自动标签", parent), m_categoryId(categoryId), m_isLibrary(isLibrary) {
    setFixedWidth(420);
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);

    initUi();
    loadTags();
}

void PresetTagsDialog::initUi() {
    QVBoxLayout* mainL = new QVBoxLayout(m_contentArea);
    mainL->setContentsMargins(15, 15, 15, 15);
    mainL->setSpacing(12);

    QLabel* folderNameLabel = new QLabel("分类名称", this);
    folderNameLabel->setObjectName("PresetFolderLabel");
    folderNameLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mainL->addWidget(folderNameLabel);

    m_folderNameEdit = new QLineEdit(this);
    m_folderNameEdit->setReadOnly(true);
    m_folderNameEdit->setFixedHeight(28);
    m_folderNameEdit->setObjectName("PresetFolderNameEdit");
    mainL->addWidget(m_folderNameEdit);

    QLabel* tagLabel = new QLabel("自动添加标签", this);
    tagLabel->setObjectName("PresetTagLabel");
    tagLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mainL->addWidget(tagLabel);

    m_tagContainer = new TagPillContainer(this, TagTriggerSource::PresetTagsDialog);
    connect(m_tagContainer, &TagPillContainer::tagsChanged, this, [this](const QStringList& tags) {
        m_presetTags = tags;
    });

    mainL->addWidget(m_tagContainer);

    QHBoxLayout* bottomL = new QHBoxLayout();
    bottomL->setSpacing(10);
    bottomL->addStretch();

    QPushButton* btnSave = new QPushButton("保存设置", this);
    btnSave->setFixedSize(90, 28);
    btnSave->setCursor(Qt::PointingHandCursor);
    btnSave->setObjectName("PresetBtnSave");
    connect(btnSave, &QPushButton::clicked, this, &PresetTagsDialog::onSaveClicked);
    bottomL->addWidget(btnSave);

    QPushButton* btnCancel = new QPushButton("取消", this);
    btnCancel->setFixedSize(70, 28);
    btnCancel->setCursor(Qt::PointingHandCursor);
    btnCancel->setObjectName("PresetBtnCancel");
    connect(btnCancel, &QPushButton::clicked, this, &PresetTagsDialog::onCancelClicked);
    bottomL->addWidget(btnCancel);

    mainL->addLayout(bottomL);
}

void PresetTagsDialog::loadTags() {
    m_categoryName = DatabaseManager::instance().getCategoryNameById(m_categoryId);
    QString tagsStr = DatabaseManager::instance().getCategoryPresetTags(m_categoryId);
    m_presetTags = TagCodec::split(tagsStr);
    m_initialPresetTags = m_presetTags;

    m_folderNameEdit->setText(m_categoryName);
    m_tagContainer->setTags(m_presetTags);
}

bool PresetTagsDialog::event(QEvent* event) {
    bool result = FramelessDialog::event(event);
    if (event->type() == QEvent::LayoutRequest) {
        if (layout()) {
            layout()->activate();
            int h = layout()->heightForWidth(420);
            if (h > 0) {
                resize(420, h);
            }
        }
    }
    return result;
}

void PresetTagsDialog::onSaveClicked() {
    QString joined = TagCodec::join(m_presetTags);
    DatabaseManager::instance().setCategoryPresetTags(m_categoryId, joined);
    ToolTipOverlay::instance()->showToast("预设标签已保存", 1500, QColor("#2ecc71"));
    accept();
}

void PresetTagsDialog::onCancelClicked() {
    reject();
}

} // namespace RapidNotes
