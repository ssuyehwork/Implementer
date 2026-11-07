#include "PresetTagsDialog.h"
#include "UiHelper.h"
#include "../meta/FavoriteDao.h"
#include "../meta/FavoriteService.h"
#include "../meta/LibraryDao.h"
#include "../meta/MetadataManager.h"
#include "../core/CoreEngine.h"
#include "../core/PresetTagService.h"
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

namespace QuarkMeta {

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

    QLabel* folderNameLabel = new QLabel("分类/文件夹名", this);
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
    if (m_isLibrary) {
        LibraryCategoryRecord rec = LibraryDao::getCategoryById(m_categoryId);
        if (rec.id == m_categoryId) {
            m_categoryName = rec.name;
            m_presetTags = rec.presetTags;
            m_initialPresetTags = rec.presetTags;
        }
    } else {
        auto list = FavoriteDao::getAllFavorites();
        for (const auto& rec : list) {
            if (rec.id == m_categoryId) {
                m_categoryName = rec.name;
                m_presetTags = rec.presetTags;
                m_initialPresetTags = rec.presetTags;
                break;
            }
        }
    }
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
    // 独占落盘：非阻塞异步提交，点击“保存设置”后对话框立刻关闭，所有数据库与 Sidecar 写入在后台执行
    if (m_isLibrary) {
        PresetTagService::instance().saveLibraryPresetTags(
            m_categoryId,
            m_presetTags,
            m_initialPresetTags,
            [](int newlyAddedCount, int associatedPathsCount) {
                if (newlyAddedCount > 0 && associatedPathsCount > 0) {
                    ToolTipOverlay::instance()->showText(
                        QCursor::pos(),
                        QString("已自动追溯将 %1 个新预设标签绑定至既有 %2 个项目").arg(newlyAddedCount).arg(associatedPathsCount),
                        2000,
                        QColor("#2ecc71")
                    );
                }
            }
        );
    } else {
        FavoriteDao::updatePresetTags(m_categoryId, m_presetTags);
    }
    accept();
}

void PresetTagsDialog::onCancelClicked() {
    // 点击取消：纯内存放弃本次全部改动，绝不触碰任何数据库和文件
    reject();
}

} // namespace QuarkMeta