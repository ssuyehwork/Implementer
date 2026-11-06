#include "MetaPanel.h"
#include <QDebug>
#include "ColorPicker.h"
#include "CardPainterHelper.h"
#include "UiHelper.h"
#include <memory>
#include "ToolTipOverlay.h"
#include "Logger.h"
#include <QElapsedTimer>
#include "components/FlowLayout.h"
#include "../util/ShellHelper.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QHBoxLayout>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFileInfo>
#include <QLabel>
#include <QDir>
#include <QFile>
#include <QApplication>
#include <QScreen>
#include <QClipboard>
#include <QDesktopServices>
#include <QUrl>
#include <QCursor>
#include <QKeyEvent>
#include <QRegularExpression>
#include <QPainter>
#include <QPainterPath>
#include <algorithm>
#include <cmath>

namespace RapidNotes {

namespace {
constexpr double kMultiPreviewAngleStep = 3.0;

QPixmap renderThumbnailLayer(const QPixmap& pixmap) {
    QSize canvasSize(200, 200);
    QPixmap canvas(canvasSize);
    canvas.fill(Qt::transparent);

    QSize targetSize(200, 200);
    QPixmap scaled = pixmap.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    {
        QPainter painter(&canvas);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        int x = (200 - scaled.width()) / 2;
        int y = (200 - scaled.height()) / 2;

        QPainterPath path;
        path.addRoundedRect(QRectF(x, y, scaled.width(), scaled.height()), 4.0, 4.0);
        painter.setClipPath(path);
        painter.drawPixmap(x, y, scaled);
    }
    return canvas;
}

QPixmap renderIconLayer(const QIcon& icon, qreal dpr) {
    QSize logicalCanvasSize(200, 200);
    QSize physicalCanvasSize = logicalCanvasSize * dpr;

    QPixmap canvas(physicalCanvasSize);
    canvas.fill(Qt::transparent);
    canvas.setDevicePixelRatio(dpr);

    QPixmap iconPx = CardPainterHelper::renderDefaultIcon(icon, logicalCanvasSize, dpr);
    if (!iconPx.isNull()) {
        QPainter painter(&canvas);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        QSizeF iconLogicalSize = iconPx.deviceIndependentSize();
        int x = qRound((200.0 - iconLogicalSize.width()) / 2.0);
        int y = qRound((200.0 - iconLogicalSize.height()) / 2.0);

        painter.drawPixmap(x, y, iconPx);
    }
    return canvas;
}
}

MetaPanel::MetaPanel(QWidget* parent) : QFrame(parent) {
    setObjectName("MetadataContainer");
    setAttribute(Qt::WA_StyledBackground, true);
    setMinimumWidth(230);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    m_adjustTimer = new QTimer(this);
    m_adjustTimer->setSingleShot(true);
    m_adjustTimer->setInterval(50);
    connect(m_adjustTimer, &QTimer::timeout, this, &MetaPanel::adjustFlowHeights);

    setContextMenuPolicy(Qt::CustomContextMenu);
    initUi();
}

QWidget* MetaPanel::createCollapsibleSection(const QString& title, QWidget* contentWidget, bool defaultExpanded) {
    QWidget* sectionWidget = new QWidget(m_container);
    QVBoxLayout* sectionLayout = new QVBoxLayout(sectionWidget);
    sectionLayout->setContentsMargins(0, 0, 0, 0);
    sectionLayout->setSpacing(4);

    QPushButton* btnHeader = new QPushButton(sectionWidget);
    btnHeader->setFixedHeight(22);
    btnHeader->setCursor(Qt::PointingHandCursor);
    btnHeader->setObjectName("CollapsibleHeaderBtn");

    auto updateHeaderText = [btnHeader, title](bool expanded) {
        btnHeader->setIcon(UiHelper::getIcon(expanded ? "chevron_down" : "chevron_right", QColor("#888888"), 12));
        btnHeader->setIconSize(QSize(12, 12));
        btnHeader->setText(" " + title);
    };

    updateHeaderText(defaultExpanded);
    contentWidget->setVisible(defaultExpanded);

    connect(btnHeader, &QPushButton::clicked, this, [contentWidget, updateHeaderText, btnHeader, this]() {
        bool nowVisible = !contentWidget->isVisible();
        contentWidget->setVisible(nowVisible);
        updateHeaderText(nowVisible);
        adjustFlowHeights();
    });

    sectionLayout->addWidget(btnHeader);
    sectionLayout->addWidget(contentWidget);
    return sectionWidget;
}

void MetaPanel::initUi() {
    setAcceptDrops(true);
    QWidget* header = new QWidget(this);
    header->setObjectName("ContainerHeader");
    header->setFixedHeight(32);
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(12, 0, 8, 0);
    headerLayout->setSpacing(6);
    
    QLabel* iconLabel = new QLabel(header);
    iconLabel->setPixmap(UiHelper::getIcon("all_data", QColor("#888888"), 16).pixmap(16, 16));
    headerLayout->addWidget(iconLabel);
    
    QLabel* titleLabel = new QLabel("元数据属性", header);
    titleLabel->setObjectName("MetaPanelTitleLabel");
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    m_mainLayout->addWidget(header);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setObjectName("MetaScrollArea");
    
    m_container = new QWidget(m_scrollArea);
    m_containerLayout = new QVBoxLayout(m_container);
    m_containerLayout->setContentsMargins(8, 8, 8, 8);
    m_containerLayout->setSpacing(8);

    // 1. 顶部预览区
    m_topPreviewBox = new QWidget(m_container);
    m_topPreviewBox->setObjectName("TopPreviewBox");
    m_topPreviewBox->setFixedSize(200, 200);
    QVBoxLayout* previewLayout = new QVBoxLayout(m_topPreviewBox);
    previewLayout->setContentsMargins(0, 0, 0, 0);
    previewLayout->setSpacing(0);

    m_lblImagePreview = new QLabel(m_topPreviewBox);
    m_lblImagePreview->setAlignment(Qt::AlignCenter);
    m_lblImagePreview->setMaximumSize(200, 200);
    m_lblImagePreview->setObjectName("MetaImagePreview");
    m_lblImagePreview->hide();
    previewLayout->addWidget(m_lblImagePreview, 0, Qt::AlignCenter);

    m_paletteContainer = new QWidget(m_container);
    m_paletteContainer->setObjectName("MetaPaletteContainer");
    m_paletteContainer->setFixedWidth(200);
    m_paletteFlowLayout = new FlowLayout(m_paletteContainer, 0, 4, 4);
    m_paletteFlowLayout->setContentsMargins(0, 2, 0, 0);
    m_paletteFlowLayout->setFlowAlignment(Qt::AlignHCenter);
    m_paletteContainer->hide();

    m_topPreviewBox->hide();
    m_containerLayout->addWidget(m_topPreviewBox, 0, Qt::AlignHCenter);
    m_containerLayout->addWidget(m_paletteContainer, 0, Qt::AlignHCenter);

    // 2. 文件名编辑区
    m_nameEdit = new ElasticEdit(m_container);
    m_nameEdit->setPlaceholderText("文件名...");
    m_nameEdit->setObjectName("MetaNameEdit");
    m_nameEdit->installEventFilter(this);
    m_containerLayout->addWidget(m_nameEdit);

    // 3. 备注说明区
    m_noteEdit = new ElasticEdit(m_container);
    m_noteEdit->setPlaceholderText("添加备注说明...");
    m_noteEdit->setObjectName("MetaNoteEdit");
    m_noteEdit->installEventFilter(this);
    m_containerLayout->addWidget(createCollapsibleSection("备注说明", m_noteEdit, true));

    // 4. 关联网址区
    m_linkEdit = new QLineEdit(m_container);
    m_linkEdit->setPlaceholderText("添加关联网址...");
    m_linkEdit->setFixedHeight(32);
    m_linkEdit->setObjectName("MetaLinkEdit");
    m_linkEdit->installEventFilter(this);

    m_actOpenLink = m_linkEdit->addAction(UiHelper::getIcon("link", QColor("#378ADD"), 14), QLineEdit::TrailingPosition);
    m_actOpenLink->setVisible(false);

    for (QToolButton* btn : m_linkEdit->findChildren<QToolButton*>()) {
        btn->setCursor(Qt::PointingHandCursor);
        btn->setObjectName("MetaLinkToolBtn");
    }

    connect(m_actOpenLink, &QAction::triggered, this, [this]() {
        QString urlStr = m_linkEdit->text().trimmed();
        if (!urlStr.isEmpty()) {
            QUrl url = QUrl::fromUserInput(urlStr);
            if (url.isValid() && (url.scheme().toLower() == "http" || url.scheme().toLower() == "https")) {
                QDesktopServices::openUrl(url);
            } else {
                ToolTipOverlay::instance()->showToast("无效的 Web 网址格式", 1500, QColor("#e81123"));
            }
        }
    });

    connect(m_linkEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        bool hasText = !text.trimmed().isEmpty();
        if (m_actOpenLink) m_actOpenLink->setVisible(hasText);
    });

    m_containerLayout->addWidget(createCollapsibleSection("关联网址", m_linkEdit, true));

    // 5. 星级评级 + 颜色色标条
    m_ratingColorBox = new QWidget(m_container);
    QVBoxLayout* ratingColorLayout = new QVBoxLayout(m_ratingColorBox);
    ratingColorLayout->setContentsMargins(0, 2, 0, 2);
    ratingColorLayout->setSpacing(6);

    QWidget* ratingRow = new QWidget(m_ratingColorBox);
    ratingRow->setObjectName("MetaRatingRow");
    QHBoxLayout* starLayout = new QHBoxLayout(ratingRow);
    starLayout->setContentsMargins(0, 2, 0, 2);
    starLayout->setSpacing(2);

    QPushButton* btnClearStar = new QPushButton(ratingRow);
    btnClearStar->setFixedSize(22, 22);
    btnClearStar->setCursor(Qt::PointingHandCursor);
    btnClearStar->setIcon(UiHelper::getIcon("no_color", QColor("#888888"), 16));
    btnClearStar->setIconSize(QSize(16, 16));
    btnClearStar->setProperty("tooltipText", "清除评级");
    btnClearStar->installEventFilter(this);
    btnClearStar->setObjectName("MetaBtnClearStar");
    connect(btnClearStar, &QPushButton::clicked, this, [this]() { setRating(0, true); });
    starLayout->addWidget(btnClearStar);

    for (int i = 1; i <= 5; ++i) {
        QPushButton* btnStar = new QPushButton(ratingRow);
        btnStar->setFixedSize(20, 20);
        btnStar->setCursor(Qt::PointingHandCursor);
        btnStar->setIcon(UiHelper::getIcon("star", QColor("#555555"), 16));
        btnStar->setIconSize(QSize(16, 16));
        btnStar->setObjectName("MetaBtnStar");
        connect(btnStar, &QPushButton::clicked, this, [this, i]() {
            int newRating = (m_currentRating == i) ? 0 : i;
            setRating(newRating, true);
        });
        m_starBtns.append(btnStar);
        starLayout->addWidget(btnStar);
    }
    starLayout->addStretch();
    ratingColorLayout->addWidget(ratingRow);

    m_colorStripPicker = new ColorStripPicker("", m_ratingColorBox);
    connect(m_colorStripPicker, &ColorStripPicker::colorSelected, this, [this](const QString& hex) {
        if (m_currentColorHex.compare(hex, Qt::CaseInsensitive) == 0) {
            setColor(QString(""), true);
        } else {
            setColor(hex, true);
        }
    });
    ratingColorLayout->addWidget(m_colorStripPicker);
    m_containerLayout->addWidget(m_ratingColorBox);

    // 6. 标签管理区
    m_tagBox = new QWidget(m_container);
    QVBoxLayout* tagL = new QVBoxLayout(m_tagBox);
    tagL->setContentsMargins(0, 0, 0, 0);
    tagL->setSpacing(6);

    m_btnAddTagBig = new QPushButton(UiHelper::getIcon("add", QColor("#AAAAAA"), 14), " 添加标签", m_tagBox);
    m_btnAddTagBig->setFixedHeight(32);
    m_btnAddTagBig->setCursor(Qt::PointingHandCursor);
    m_btnAddTagBig->setObjectName("BtnAddTagBig");
    connect(m_btnAddTagBig, &QPushButton::clicked, this, [this]() { openTagSelectorOverlay(m_btnAddTagBig); });
    tagL->addWidget(m_btnAddTagBig);

    m_tagContainer = new QWidget(m_tagBox);
    m_tagFlowLayout = new FlowLayout(m_tagContainer, 0, 4, 4);

    m_btnAddTagSmall = new QPushButton(m_tagContainer);
    m_btnAddTagSmall->setFixedSize(22, 22);
    m_btnAddTagSmall->setCursor(Qt::PointingHandCursor);
    m_btnAddTagSmall->setIcon(UiHelper::getIcon("add", QColor("#CCCCCC"), 12));
    m_btnAddTagSmall->setIconSize(QSize(12, 12));
    m_btnAddTagSmall->setProperty("tooltipText", "添加标签");
    m_btnAddTagSmall->installEventFilter(this);
    m_btnAddTagSmall->setObjectName("BtnAddTagSmall");
    connect(m_btnAddTagSmall, &QPushButton::clicked, this, [this]() { openTagSelectorOverlay(m_btnAddTagSmall); });
    m_btnAddTagSmall->hide();

    tagL->addWidget(m_tagContainer);
    m_containerLayout->addWidget(createCollapsibleSection("标签管理", m_tagBox, true));

    // 7. 基础物理属性区
    m_infoSectionWidget = new QWidget(m_container);
    QVBoxLayout* infoL = new QVBoxLayout(m_infoSectionWidget);
    infoL->setContentsMargins(0, 0, 0, 0);
    infoL->setSpacing(4);

    addInfoRow(infoL, "文件类型", lblType);
    addInfoRow(infoL, "文件大小", lblSize);
    addInfoRow(infoL, "图片尺寸", lblDimensions);
    addInfoRow(infoL, "创建时间", lblCtime);
    addInfoRow(infoL, "修改时间", lblMtime);
    addInfoRow(infoL, "访问时间", lblAtime);
    addInfoRow(infoL, "加密状态", lblEncrypted);

    m_containerLayout->addWidget(createCollapsibleSection("基础属性", m_infoSectionWidget, true));

    // 8. 物理路径区
    QWidget* pathBox = new QWidget(m_container);
    QVBoxLayout* pathL = new QVBoxLayout(pathBox);
    pathL->setContentsMargins(0, 0, 0, 0);
    pathL->setSpacing(5);

    m_pathEdit = new ElasticEdit(pathBox);
    m_pathEdit->setReadOnly(true);
    m_pathEdit->setObjectName("MetaPathEdit");
    pathL->addWidget(m_pathEdit);

    QHBoxLayout* pathBtnL = new QHBoxLayout();
    pathBtnL->setContentsMargins(0, 0, 0, 0);
    pathBtnL->setSpacing(6);

    m_btnCopyPath = new QPushButton("复制路径", pathBox);
    m_btnCopyPath->setFixedHeight(24);
    m_btnCopyPath->setCursor(Qt::PointingHandCursor);
    m_btnCopyPath->setObjectName("BtnCopyPath");
    connect(m_btnCopyPath, &QPushButton::clicked, this, [this]() {
        QString p = m_pathEdit->toPlainText().trimmed();
        p.remove(QChar(0x200B));
        if (!p.isEmpty() && !p.startsWith("已选中")) {
            QApplication::clipboard()->setText(QDir::toNativeSeparators(p));
            ToolTipOverlay::instance()->showToast("已复制路径", 1200, QColor("#2ecc71"));
        }
    });
    pathBtnL->addWidget(m_btnCopyPath);

    m_btnOpenLocation = new QPushButton("打开位置", pathBox);
    m_btnOpenLocation->setFixedHeight(24);
    m_btnOpenLocation->setCursor(Qt::PointingHandCursor);
    m_btnOpenLocation->setObjectName("BtnOpenLocation");
    connect(m_btnOpenLocation, &QPushButton::clicked, this, [this]() {
        QString p = m_pathEdit->toPlainText().trimmed();
        p.remove(QChar(0x200B));
        if (!p.isEmpty() && !p.startsWith("已选中")) {
            ShellHelper::openInExplorer(p);
        }
    });
    pathBtnL->addWidget(m_btnOpenLocation);

    pathL->addLayout(pathBtnL);
    m_containerLayout->addWidget(createCollapsibleSection("物理路径", pathBox, true));

    m_containerLayout->addStretch(1);
    m_scrollArea->setWidget(m_container);
    m_mainLayout->addWidget(m_scrollArea);

    updateControlsState(false, false, false);
}

void MetaPanel::openTagSelectorOverlay(QWidget* targetAnchor) {
    if (m_tagSelectorOverlay) {
        m_tagSelectorOverlay->closeOverlay();
        return;
    }

    QWidget* topWidget = this->topLevelWidget();
    const QStringList targetPathsSnapshot = m_selectedPaths;

    // 显式声明来自 MetaPanel 触发源
    m_tagSelectorOverlay = new TagSelectorOverlay(m_currentTagsSet.values(), targetPathsSnapshot, topWidget, TagTriggerSource::MetaPanel);

    connect(m_tagSelectorOverlay, &TagSelectorOverlay::overlayClosed, this, [this]() {
        m_tagSelectorOverlay = nullptr;
    });

    if (topWidget) {
        const int overlayW = m_tagSelectorOverlay->width();
        const int overlayH = m_tagSelectorOverlay->height();

        QRect topGeom = topWidget->frameGeometry(); 
        QPoint metaPanelGlobal = this->mapToGlobal(QPoint(0, 0)); 

        int targetX = metaPanelGlobal.x() - overlayW - 8;
        int targetY = topGeom.top() + (topGeom.height() - overlayH) / 2;

        int minX = topGeom.left();
        int maxX = qMax(minX, topGeom.right() - overlayW);
        int minY = topGeom.top();
        int maxY = qMax(minY, topGeom.bottom() - overlayH);

        targetX = qBound(minX, targetX, maxX);
        targetY = qBound(minY, targetY, maxY);

        m_tagSelectorOverlay->move(targetX, targetY);
    } else if (targetAnchor) {
        m_tagSelectorOverlay->move(targetAnchor->mapToGlobal(QPoint(0, targetAnchor->height() + 4)));
    }

    m_tagSelectorOverlay->show();

    // 1. 前端 UI 实时内存响应：点选过程即时在面板渲染胶囊，零 I/O 零写盘
    connect(m_tagSelectorOverlay, &TagSelectorOverlay::selectionChanged, this, [this, targetPathsSnapshot](const QStringList& currentSelected) {
        if (m_selectedPaths == targetPathsSnapshot) {
            setTags(currentSelected);
        }
    });

    // 2. 离场原子提交响应：失焦/关闭时单次计算差集并执行落盘
    auto appliedTagsSet = std::make_shared<QSet<QString>>(m_currentTagsSet);

    connect(m_tagSelectorOverlay, &TagSelectorOverlay::tagsCommitted, this, [this, targetPathsSnapshot, appliedTagsSet](const QStringList& finalSelectedTags) {
        if (targetPathsSnapshot.isEmpty()) return;

        QSet<QString> finalSet(finalSelectedTags.begin(), finalSelectedTags.end());

        QStringList addedTags;
        for (const QString& t : finalSet) {
            if (!appliedTagsSet->contains(t)) {
                addedTags.append(t);
            }
        }
        QStringList removedTags;
        for (const QString& t : *appliedTagsSet) {
            if (!finalSet.contains(t)) {
                removedTags.append(t);
            }
        }

        if (!addedTags.isEmpty()) {
            emit tagsAddRequested(targetPathsSnapshot, addedTags);
            for (const QString& t : addedTags) {
                emit tagAddRequested(targetPathsSnapshot, t);
            }
        }
        if (!removedTags.isEmpty()) {
            emit tagsRemoveRequested(targetPathsSnapshot, removedTags);
            for (const QString& t : removedTags) {
                emit tagRemoveRequested(targetPathsSnapshot, t);
            }
        }

        if (m_selectedPaths == targetPathsSnapshot) {
            setTags(finalSelectedTags);
        }
    });
}

void MetaPanel::setImagePreview(const QPixmap& pixmap) {
    if (!m_lblImagePreview) return;
    if (pixmap.isNull()) {
        m_lblImagePreview->clear();
        m_lblImagePreview->hide();
        if (m_topPreviewBox) m_topPreviewBox->hide();
    } else {
        QPixmap canvas = renderThumbnailLayer(pixmap);
        m_lblImagePreview->setPixmap(canvas);
        m_lblImagePreview->setMaximumSize(QSize(200, 200));

        m_lblImagePreview->show();
        if (m_topPreviewBox) {
            m_topPreviewBox->show();
        }
    }
    m_adjustTimer->start();
}

void MetaPanel::setMultiSelectionPreview(const QList<MetaPreviewItem>& items) {
    if (!m_lblImagePreview) return;

    QList<QPixmap> validLayers;
    for (const auto& item : items) {
        if (item.hasThumbnail && !item.thumbnail.isNull()) {
            validLayers.append(renderThumbnailLayer(item.thumbnail));
        } else if (!item.icon.isNull()) {
            validLayers.append(renderIconLayer(item.icon, devicePixelRatioF()));
        } else {
            continue;
        }
        if (validLayers.size() >= kMultiPreviewMaxCards) break;
    }

    if (validLayers.isEmpty()) {
        setImagePreview(QPixmap());
        return;
    }

    std::reverse(validLayers.begin(), validLayers.end());

    qreal dpr = devicePixelRatioF();
    QSize logicalCanvasSize(200, 200);
    QSize physicalCanvasSize = logicalCanvasSize * dpr;

    QPixmap canvas(physicalCanvasSize);
    canvas.fill(Qt::transparent);
    canvas.setDevicePixelRatio(dpr);

    QPainter painter(&canvas);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    for (int k = validLayers.size() - 1; k >= 0; --k) {
        const QPixmap& layer = validLayers[k];

        double stepMultiplier = std::ceil((k + 1) / 2.0);
        double baseAngle = kMultiPreviewAngleStep * stepMultiplier;
        double angle = (k % 2 == 0) ? -baseAngle : baseAngle;
        double scale = 0.8;
        double offsetY = 0.0 * k;

        painter.save();
        painter.translate(100.0, 100.0 + offsetY);
        painter.rotate(angle);
        if (scale < 1.0) {
            painter.scale(scale, scale);
        }
        painter.drawPixmap(QPointF(-100.0, -100.0), layer);
        painter.restore();
    }

    m_lblImagePreview->setPixmap(canvas);
    m_lblImagePreview->setMaximumSize(logicalCanvasSize);

    m_lblImagePreview->show();
    if (m_topPreviewBox) {
        m_topPreviewBox->show();
    }
    m_adjustTimer->start();
}

void MetaPanel::setDefaultIconPreview(const QIcon& icon) {
    if (!m_lblImagePreview) return;
    if (icon.isNull()) {
        m_lblImagePreview->clear();
        m_lblImagePreview->hide();
        if (m_topPreviewBox) m_topPreviewBox->hide();
    } else {
        QPixmap canvas = renderIconLayer(icon, devicePixelRatioF());
        m_lblImagePreview->setPixmap(canvas);
        m_lblImagePreview->setMaximumSize(QSize(200, 200));

        m_lblImagePreview->show();
        if (m_topPreviewBox) {
            m_topPreviewBox->show();
        }
    }
    m_adjustTimer->start();
}

void MetaPanel::setSelectedPaths(const QStringList& paths) {
    if (m_tagRemoveDebounceTimer && m_tagRemoveDebounceTimer->isActive()) {
        m_tagRemoveDebounceTimer->stop();
        flushPendingTagDeletions();
    }

    // 🚀【根除时序竞态】：若用户正在输入且尚未失焦，必须在快照切换前先向旧文件提交修改，杜绝串扰污染新选中的文件！
    if (m_isUserEditing && !m_editingPathsSnapshot.isEmpty()) {
        if (m_noteEdit && m_noteEdit->hasFocus()) {
            emit noteEdited(m_editingPathsSnapshot, m_noteEdit->toPlainText());
        } else if (m_linkEdit && m_linkEdit->hasFocus()) {
            emit linkEdited(m_editingPathsSnapshot, m_linkEdit->text().trimmed());
        }
        m_isUserEditing = false;
    }

    QElapsedTimer timer;
    timer.start();
    m_selectedPaths = paths;
    m_editingPathsSnapshot = paths;
    bool hasSelection = !m_selectedPaths.isEmpty();
    bool isMulti = m_selectedPaths.size() > 1;

    m_isReadOnlyMode = false;
    if (hasSelection && (m_selectedPaths.first().endsWith(".amenc", Qt::CaseInsensitive) || m_selectedPaths.first().contains("trash://"))) {
        m_isReadOnlyMode = true;
    }

    updateControlsState(hasSelection, isMulti, m_isReadOnlyMode);

    if (!hasSelection) {
        m_isInternalUpdating = true;
        setImagePreview(QPixmap());
        if (m_nameEdit) { m_nameEdit->clear(); m_nameEdit->setPlaceholderText("文件名..."); }
        if (m_noteEdit) { m_noteEdit->clear(); m_noteEdit->setPlaceholderText("添加备注说明..."); }
        
        if (m_pathEdit) { m_pathEdit->clear(); }
        if (lblType) lblType->setText("-");
        if (lblSize) lblSize->setText("-");
        if (lblDimensions) lblDimensions->setText("-");
        if (lblCtime) lblCtime->setText("-");
        if (lblMtime) lblMtime->setText("-");
        if (lblAtime) lblAtime->setText("-");
        if (lblEncrypted) lblEncrypted->setText("-");
        setRating(0, false);
        setColor(QString(""), false);
        setTags({});
        setPalettes({});
        m_isInternalUpdating = false;
    } else if (isMulti) {
        m_isInternalUpdating = true;
        setImagePreview(QPixmap());
        if (m_nameEdit) {
            m_nameEdit->setHtml(QString("已选中 <span style=\"color:#3498db;\">%1</span> 个项目").arg(m_selectedPaths.size()));
        }
        if (m_noteEdit) {
            m_noteEdit->clear();
            m_noteEdit->setPlaceholderText("添加批量备注...");
        }
        if (m_linkEdit) {
            m_linkEdit->clear();
            m_linkEdit->setPlaceholderText("添加批量关联网址...");
        }
        if (m_pathEdit) {
            m_pathEdit->setPlainText(QString("已选中 %1 个项目").arg(m_selectedPaths.size()));
        }
        if (lblType) lblType->setText("混合项目");
        if (lblSize) lblSize->setText("-");
        if (lblDimensions) lblDimensions->setText("-");
        if (lblCtime) lblCtime->setText("-");
        if (lblMtime) lblMtime->setText("-");
        if (lblAtime) lblAtime->setText("-");
        if (lblEncrypted) lblEncrypted->setText("-");
        
        setRating(0, false);
        setColor(QString(""), false);
        setPalettes({});
        m_isInternalUpdating = false;
    }
    qint64 ms = timer.elapsed();
    if (ms > 2) {
        qDebug() << QString("[Perf] MetaPanel::setSelectedPaths(%1 paths) took %2ms").arg(paths.size()).arg(ms);
    }
}

void MetaPanel::updateControlsState(bool hasSelection, bool isMultiSelection, bool isReadOnly) {
    if (m_nameEdit) {
        m_nameEdit->setEnabled(hasSelection && !isMultiSelection && !isReadOnly);
    }
    if (m_noteEdit) {
        m_noteEdit->setEnabled(hasSelection && !isReadOnly);
    }
    if (m_linkEdit) {
        m_linkEdit->setEnabled(hasSelection && !isReadOnly);
    }
    if (m_btnAddTagBig) {
        m_btnAddTagBig->setEnabled(hasSelection && !isReadOnly);
    }
    if (m_btnAddTagSmall) {
        m_btnAddTagSmall->setEnabled(hasSelection && !isReadOnly);
    }
    if (m_ratingColorBox) {
        m_ratingColorBox->setEnabled(hasSelection && !isReadOnly);
    }
    if (m_btnCopyPath) {
        m_btnCopyPath->setEnabled(hasSelection && !isMultiSelection);
    }
    if (m_btnOpenLocation) {
        m_btnOpenLocation->setEnabled(hasSelection && !isMultiSelection);
    }
}

void MetaPanel::addInfoRow(QVBoxLayout* layout, const QString& label, QLabel*& valueLabel) {
    QWidget* row = new QWidget(m_container);
    QHBoxLayout* rl = new QHBoxLayout(row);
    rl->setContentsMargins(0, 1, 0, 1);
    rl->setSpacing(6);
    
    QLabel* kl = new QLabel(label, row);
    kl->setFixedWidth(65);
    kl->setObjectName("MetaInfoKeyLabel");
    rl->addWidget(kl, 0, Qt::AlignTop);

    valueLabel = new QLabel("-", row);
    valueLabel->setWordWrap(true);
    valueLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    valueLabel->setObjectName("MetaInfoValueLabel");
    valueLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    rl->addWidget(valueLabel, 1);
    
    layout->addWidget(row);
}

void MetaPanel::onTagDeleted(const QString& text) {
    if (m_selectedPaths.isEmpty() || m_isReadOnlyMode || text.isEmpty()) return;

    if (!m_tagRemovalTargetSnapshot.isEmpty() && m_tagRemovalTargetSnapshot != m_selectedPaths) {
        if (m_tagRemoveDebounceTimer && m_tagRemoveDebounceTimer->isActive()) {
            m_tagRemoveDebounceTimer->stop();
        }
        flushPendingTagDeletions();
    }
    m_tagRemovalTargetSnapshot = m_selectedPaths;
    m_pendingRemovedTags.insert(text);

    QStringList remainingTags = m_currentTagsSet.values();
    remainingTags.removeAll(text);
    setTags(remainingTags);

    if (!m_tagRemoveDebounceTimer) {
        m_tagRemoveDebounceTimer = new QTimer(this);
        m_tagRemoveDebounceTimer->setSingleShot(true);
        connect(m_tagRemoveDebounceTimer, &QTimer::timeout, this, &MetaPanel::flushPendingTagDeletions);
    }
    m_tagRemoveDebounceTimer->start(500);
}

void MetaPanel::flushPendingTagDeletions() {
    if (m_pendingRemovedTags.isEmpty() || m_tagRemovalTargetSnapshot.isEmpty()) return;

    QStringList tagsToRemove = m_pendingRemovedTags.values();
    QStringList targetPaths = m_tagRemovalTargetSnapshot;
    m_pendingRemovedTags.clear();
    m_tagRemovalTargetSnapshot.clear();

    emit tagsRemoveRequested(targetPaths, tagsToRemove);
}

void MetaPanel::resizeEvent(QResizeEvent* event) {
    QFrame::resizeEvent(event);
    adjustFlowHeights();
}

void MetaPanel::adjustFlowHeights() {
    if (m_topPreviewBox) {
        bool hasPreview = (m_lblImagePreview && !m_lblImagePreview->pixmap().isNull());
        if (hasPreview) {
            m_topPreviewBox->show();
        } else {
            m_topPreviewBox->hide();
        }
    }

    if (m_paletteContainer) {
        bool hasPalette = (m_paletteFlowLayout && m_paletteFlowLayout->count() > 0);
        if (hasPalette) {
            m_paletteContainer->show();
        } else {
            m_paletteContainer->hide();
        }
    }

    if (m_tagContainer && m_tagFlowLayout) {
        bool hasTags = (m_tagFlowLayout->count() > (m_btnAddTagSmall ? 1 : 0));
        if (hasTags) {
            if (m_btnAddTagBig) m_btnAddTagBig->hide();
            if (m_btnAddTagSmall) m_btnAddTagSmall->show();
            m_tagContainer->show();
        } else {
            if (m_btnAddTagSmall) m_btnAddTagSmall->hide();
            if (m_btnAddTagBig) m_btnAddTagBig->show();
            m_tagContainer->hide();
        }
        m_tagFlowLayout->activate();
    }
}

void MetaPanel::showEvent(QShowEvent* event) {
    QFrame::showEvent(event);
    adjustFlowHeights();
}

void MetaPanel::updateInfo(const QString& n, const QString& t, const QString& s, 
                            const QString& ct, const QString& mt, const QString& at, 
                            const QString& p, bool e, int width, int height) {
    if (m_isUserEditing) return;

    m_isInternalUpdating = true;
    
    QFileInfo info(n);
    if (m_selectedPaths.size() <= 1) {
        m_nameEdit->setPlainText(info.completeBaseName());
        m_nameEdit->setProperty("oldPath", p);
        m_nameEdit->setProperty("suffix", info.suffix());
        
        lblType->setText(t);
        lblSize->setText(s);
        lblCtime->setText(ct);
        lblMtime->setText(mt);
        lblAtime->setText(at);
        
        QString prettyPath = p;
        prettyPath.replace("\\", "\\\u200B").replace("/", "/\u200B");
        m_pathEdit->setPlainText(prettyPath);
    }

    lblEncrypted->setText(e ? "已加密" : "未加密");
    
    if (width > 0 && height > 0 && m_selectedPaths.size() <= 1) {
        lblDimensions->setText(QString("%1 x %2 像素").arg(width).arg(height));
        if (lblDimensions->parentWidget()) lblDimensions->parentWidget()->show();
    } else {
        lblDimensions->setText("-");
        if (lblDimensions->parentWidget()) lblDimensions->parentWidget()->hide();
    }

    m_isInternalUpdating = false;
}

void MetaPanel::setTags(const QStringList& tags) {
    m_currentTagsSet = QSet<QString>(tags.begin(), tags.end());

    while (QLayoutItem* item = m_tagFlowLayout->takeAt(0)) {
        if (item->widget() && item->widget() != m_btnAddTagSmall) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    if (tags.isEmpty()) {
        m_btnAddTagBig->show();
        m_btnAddTagSmall->hide();
    } else {
        m_btnAddTagBig->hide();

        for (const QString& tag : tags) {
            TagPill* pill = new TagPill(tag, m_tagContainer);
            pill->setProperty("tagText", tag);
            connect(pill, &TagPill::deleteRequested, this, &MetaPanel::onTagDeleted);
            pill->show();
            m_tagFlowLayout->addWidget(pill);
        }

        m_btnAddTagSmall->show();
        m_tagFlowLayout->addWidget(m_btnAddTagSmall);
    }

    m_adjustTimer->start();
}

void MetaPanel::setRating(int rating, bool fromUser) {
    m_currentRating = rating;
    for (int i = 0; i < m_starBtns.size(); ++i) {
        bool active = (i < rating);
        m_starBtns[i]->setIcon(UiHelper::getIcon(
            active ? "star_filled" : "star",
            active ? QColor("#FF551C") : QColor("#555555"),
            16
        ));
        m_starBtns[i]->setIconSize(QSize(16, 16));
    }

    if (fromUser && !m_selectedPaths.isEmpty() && !m_isReadOnlyMode) {
        emit ratingChanged(m_selectedPaths, rating);
    }
}

void MetaPanel::setColor(const QString& hexColor, bool fromUser) {
    m_currentColorHex = hexColor;
    if (m_colorStripPicker) {
        m_colorStripPicker->setSelectedColor(hexColor);
    }

    if (fromUser && !m_selectedPaths.isEmpty() && !m_isReadOnlyMode) {
        emit colorChanged(m_selectedPaths, hexColor);
    }
}


void MetaPanel::setNote(const QString& note) {
    if (m_isUserEditing) return;
    m_isInternalUpdating = true;
    m_noteEdit->setPlainText(note);
    m_isInternalUpdating = false;
}


void MetaPanel::setURL(const QString& url) {
    if (m_isUserEditing) return;
    m_isInternalUpdating = true;
    m_linkEdit->setText(url);
    m_linkEdit->setCursorPosition(0);
    if (m_actOpenLink) {
        m_actOpenLink->setVisible(!url.trimmed().isEmpty());
    }
    m_isInternalUpdating = false;
}


void MetaPanel::setPalettes(const QVector<QPair<QColor, float>>& palette) {
    if (!m_paletteFlowLayout) return;

    while (QLayoutItem* item = m_paletteFlowLayout->takeAt(0)) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    if (palette.isEmpty()) {
        if (m_paletteContainer) m_paletteContainer->hide();
    } else {
        for (const auto& entry : palette) {
            ColorPill* pill = new ColorPill(entry.first, entry.second, m_paletteContainer);
            pill->setObjectName("MetaColorPill");
            connect(pill, &ColorPill::colorSelected, this, [this](const QColor& c){ emit searchByColor(c); });
            connect(pill, &ColorPill::requestSetAsPrimary, this, &MetaPanel::setAsPrimaryColor);
            pill->show();
            m_paletteFlowLayout->addWidget(pill);
        }
        if (m_paletteContainer) m_paletteContainer->show();
    }

    m_paletteFlowLayout->invalidate();
    m_adjustTimer->start();
}

void MetaPanel::setAsPrimaryColor(const QColor& color) {
    QString currentPath = m_pathEdit->toPlainText().trimmed();
    currentPath.remove(QChar(0x200B));
    if (!currentPath.isEmpty() && !currentPath.startsWith("已选中")) {
        emit primaryColorChanged(currentPath, color);
    }
}

bool MetaPanel::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::DragEnter) {
        auto* de = static_cast<QDragEnterEvent*>(event);
        if (de->mimeData() && de->mimeData()->hasUrls()) {
            dragEnterEvent(de);
            return true;
        }
    } else if (event->type() == QEvent::DragMove) {
        auto* dm = static_cast<QDragMoveEvent*>(event);
        if (dm->mimeData() && dm->mimeData()->hasUrls()) {
            dragMoveEvent(dm);
            return true;
        }
    } else if (event->type() == QEvent::DragLeave) {
        dragLeaveEvent(static_cast<QDragLeaveEvent*>(event));
        return true;
    } else if (event->type() == QEvent::Drop) {
        auto* dropEv = static_cast<QDropEvent*>(event);
        if (dropEv->mimeData() && dropEv->mimeData()->hasUrls()) {
            dropEvent(dropEv);
            return true;
        }
    }
    if (event->type() == QEvent::ToolTip) {
        QString text = watched->property("tooltipText").toString();
        if (!text.isEmpty()) {
            ToolTipOverlay::instance()->showText(QCursor::pos(), text, 0);
            return true;
        }
    } else if (event->type() == QEvent::Leave || event->type() == QEvent::MouseButtonPress) {
        ToolTipOverlay::hideTip();
    }

    if (m_isInternalUpdating || m_isReadOnlyMode) return QFrame::eventFilter(watched, event);

    if (watched == m_nameEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            m_nameEdit->clearFocus();
            return true;
        }
    }

    if (watched == m_linkEdit) {
        if (event->type() == QEvent::FocusIn) {
            m_isUserEditing = true;
            m_editingPathsSnapshot = m_selectedPaths;
        } else if (event->type() == QEvent::FocusOut) {
            m_isUserEditing = false;
        }
    } else if (event->type() == QEvent::FocusIn) {
        if (watched == m_noteEdit || watched == m_nameEdit) {
            m_isUserEditing = true;
            m_editingPathsSnapshot = m_selectedPaths;
        }
    } else if (event->type() == QEvent::FocusOut) {
        if (watched == m_noteEdit || watched == m_nameEdit) {
            m_isUserEditing = false;
        }
    }

    if (watched == m_noteEdit && event->type() == QEvent::FocusOut) {
        if (!m_editingPathsSnapshot.isEmpty()) {
            emit noteEdited(m_editingPathsSnapshot, m_noteEdit->toPlainText());
        }
    } else if (watched == m_linkEdit && event->type() == QEvent::FocusOut) {
        if (!m_editingPathsSnapshot.isEmpty()) {
            emit linkEdited(m_editingPathsSnapshot, m_linkEdit->text().trimmed());
        }
    } else if (watched == m_nameEdit && event->type() == QEvent::FocusOut) {
        if (m_editingPathsSnapshot.size() > 1) return true;

        QString oldPath = m_nameEdit->property("oldPath").toString();
        QString newName = m_nameEdit->toPlainText().trimmed();
        
        static const QRegularExpression illegalRegex("[\\\\/:*?\"<>|\r\n]");
        newName.remove(illegalRegex);
        m_nameEdit->setPlainText(newName);

        QString suffix = m_nameEdit->property("suffix").toString();
        if (!oldPath.isEmpty() && !newName.isEmpty()) {
            QFileInfo oldInfo(oldPath);
            if (newName != oldInfo.completeBaseName()) {
                QString newPath = oldInfo.absolutePath() + "/" + newName + (suffix.isEmpty() ? "" : "." + suffix);
                newPath = QDir::toNativeSeparators(newPath);
                
                if (QFile::exists(newPath)) {
                    m_nameEdit->setPlainText(oldInfo.completeBaseName());
                    return true;
                }

                emit renameRequested(oldPath, newPath);
            }
        }
    }
    return QFrame::eventFilter(watched, event);
}

} // namespace RapidNotes