#include "MetaPanelPresenter.h"
#include "ActivePaneCoordinator.h"
#include "../meta/MetadataManager.h"
#include "UiHelper.h"
#include <QFileInfo>
#include <QDir>
#include <QIcon>
#include <QPixmap>

namespace QuarkMeta {

MetaPanelPresenter::MetaPanelPresenter(ActivePaneCoordinator* activeCoordinator, MetaPanel* metaPanel, ContentPanel* rootPanel, QObject* parent)
    : QObject(parent), m_activeCoordinator(activeCoordinator), m_metaPanel(metaPanel), m_rootPanel(rootPanel) {
}

void MetaPanelPresenter::renderEmptySelection() {
    if (!m_metaPanel) return;
    m_metaPanel->setImagePreview(QPixmap());
    m_metaPanel->updateInfo("-", "-", "-", "-", "-", "-", "-", false, 0, 0);
    m_metaPanel->setRating(0, false);
    m_metaPanel->setColor(QString(""), false);
    m_metaPanel->setTags(QStringList());
    m_metaPanel->setNote(QString(""));
    m_metaPanel->setURL(QString(""));
    m_metaPanel->setPalettes({});
}

void MetaPanelPresenter::renderSingleSelection(ContentPanel* panel, const QString& path) {
    if (!m_metaPanel || !panel) return;

    QModelIndexList selectedIndices = panel->getSelectedIndexes();
    QModelIndex idx = selectedIndices.isEmpty() ? QModelIndex() : selectedIndices.first();

    QFileInfo fi(path);
    QString name = idx.isValid() ? idx.sibling(idx.row(), static_cast<int>(FileListColumn::Name)).data(Qt::DisplayRole).toString() : fi.fileName();
    QString type = idx.isValid() ? ((idx.data(TypeRole).toString() == "folder") ? "文件夹" : idx.sibling(idx.row(), static_cast<int>(FileListColumn::Type)).data(Qt::DisplayRole).toString() + " 文件") : (fi.isDir() ? "文件夹" : fi.suffix().toUpper() + " 文件");
    QString sizeStr = idx.isValid() ? idx.sibling(idx.row(), static_cast<int>(FileListColumn::Size)).data(Qt::DisplayRole).toString() : "-";
    QString mtimeStr = idx.isValid() ? idx.sibling(idx.row(), static_cast<int>(FileListColumn::ModifiedDate)).data(Qt::DisplayRole).toString() : "-";
    bool encrypted = idx.isValid() ? idx.data(EncryptedRole).toBool() : false;

    auto meta = MetadataManager::instance().getMeta(path.toStdWString());

    m_metaPanel->updateInfo(
        name, type, sizeStr, "-", mtimeStr, "-",
        path, encrypted, meta.width, meta.height
    );

    QVector<QPair<QColor, float>> qPalettes;
    qPalettes.reserve(static_cast<int>(meta.palettes.size()));
    for (const auto& entry : meta.palettes) {
        qPalettes.append(qMakePair(entry.color, entry.ratio));
    }

    if (idx.isValid()) {
        int rating = idx.data(RatingRole).toInt();
        QString color = idx.data(ColorRole).toString();
        QStringList tags = idx.data(TagsRole).toStringList();
        QString note = idx.data(NoteRole).toString();
        QString url = idx.data(UrlRole).toString();

        int finalRating = rating > 0 ? rating : meta.rating;
        QString finalColor = !color.isEmpty() ? color : QString::fromStdWString(meta.manualColor);
        QStringList finalTagsList = !tags.isEmpty() ? tags : meta.tags;
        QString finalNote = !note.isEmpty() ? note : QString::fromStdWString(meta.note);
        QString finalUrl = !url.isEmpty() ? url : QString::fromStdWString(meta.url);

        m_metaPanel->setRating(finalRating, false);
        m_metaPanel->setColor(finalColor, false);
        m_metaPanel->setTags(finalTagsList);
        m_metaPanel->setNote(finalNote);
        m_metaPanel->setURL(finalUrl);
        m_metaPanel->setPalettes(qPalettes);

        QVariant decData = idx.data(Qt::DecorationRole);
        bool hasThumb = idx.data(HasThumbnailRole).toBool();
        if (hasThumb) {
            QPixmap previewPixmap;
            if (decData.canConvert<QIcon>()) {
                previewPixmap = decData.value<QIcon>().pixmap(128, 128);
            } else if (decData.canConvert<QPixmap>()) {
                previewPixmap = decData.value<QPixmap>();
            }
            m_metaPanel->setImagePreview(previewPixmap);
        } else {
            if (decData.canConvert<QIcon>()) {
                m_metaPanel->setDefaultIconPreview(decData.value<QIcon>());
            } else {
                m_metaPanel->setImagePreview(QPixmap());
            }
        }
    } else {
        m_metaPanel->setRating(meta.rating, false);
        m_metaPanel->setColor(QString::fromStdWString(meta.manualColor), false);
        m_metaPanel->setTags(meta.tags);
        m_metaPanel->setNote(QString::fromStdWString(meta.note));
        m_metaPanel->setURL(QString::fromStdWString(meta.url));
        m_metaPanel->setPalettes(qPalettes);
        m_metaPanel->setImagePreview(QPixmap());
    }
}

void MetaPanelPresenter::renderMultiSelection(ContentPanel* panel, const QStringList& paths) {
    if (!m_metaPanel || !panel) return;

    QModelIndexList selectedIndices = panel->getSelectedIndexes();
    QMap<QString, QModelIndex> pathToIndexMap;
    for (const QModelIndex& idx : selectedIndices) {
        if (idx.isValid()) {
            QString p = idx.data(PathRole).toString();
            if (!p.isEmpty()) {
                pathToIndexMap[p] = idx;
            }
        }
    }

    bool firstItem = true;
    bool sameRating = true;
    bool sameColor = true;
    bool sameNote = true;
    bool sameUrl = true;

    int commonRating = 0;
    QString commonColor;
    QString commonNote;
    QString commonUrl;

    QSet<QString> commonTags;
    bool commonTagsInitialized = false;

    QList<MetaPreviewItem> multiPreviewItems;
    int previewStartIdx = qMax(0, paths.size() - MetaPanel::kMultiPreviewMaxCards);

    int phase1EndIdx = paths.size();
    for (int i = 0; i < paths.size(); ++i) {
        const QString& path = paths[i];
        QModelIndex idx = pathToIndexMap.value(path, QModelIndex());

        int itemRating = 0;
        QString itemColor;
        QStringList itemTags;
        QString itemNote;
        QString itemUrl;

        auto meta = MetadataManager::instance().getMeta(path.toStdWString());

        if (idx.isValid()) {
            int r = idx.data(RatingRole).toInt();
            QString c = idx.data(ColorRole).toString();
            QStringList t = idx.data(TagsRole).toStringList();
            QString n = idx.data(NoteRole).toString();
            QString u = idx.data(UrlRole).toString();

            itemRating = r > 0 ? r : meta.rating;
            itemColor = !c.isEmpty() ? c : QString::fromStdWString(meta.manualColor);
            itemTags = !t.isEmpty() ? t : meta.tags;
            itemNote = !n.isEmpty() ? n : QString::fromStdWString(meta.note);
            itemUrl = !u.isEmpty() ? u : QString::fromStdWString(meta.url);

            if (i >= previewStartIdx && multiPreviewItems.size() < MetaPanel::kMultiPreviewMaxCards) {
                MetaPreviewItem previewItem;
                QVariant decData = idx.data(Qt::DecorationRole);
                bool hasThumb = idx.data(HasThumbnailRole).toBool();
                if (hasThumb) {
                    previewItem.hasThumbnail = true;
                    if (decData.canConvert<QIcon>()) {
                        previewItem.thumbnail = decData.value<QIcon>().pixmap(128, 128);
                    } else if (decData.canConvert<QPixmap>()) {
                        previewItem.thumbnail = decData.value<QPixmap>();
                    }
                } else {
                    previewItem.hasThumbnail = false;
                    if (decData.canConvert<QIcon>()) {
                        previewItem.icon = decData.value<QIcon>();
                    }
                }
                if ((previewItem.hasThumbnail && !previewItem.thumbnail.isNull()) || (!previewItem.hasThumbnail && !previewItem.icon.isNull())) {
                    multiPreviewItems.append(previewItem);
                }
            }
        } else {
            itemRating = meta.rating;
            itemColor = QString::fromStdWString(meta.manualColor);
            itemTags = meta.tags;
            itemNote = QString::fromStdWString(meta.note);
            itemUrl = QString::fromStdWString(meta.url);

            if (i >= previewStartIdx && multiPreviewItems.size() < MetaPanel::kMultiPreviewMaxCards) {
                MetaPreviewItem previewItem;
                previewItem.hasThumbnail = false;
                QFileInfo fi(path);
                if (fi.isDir()) {
                    previewItem.icon = UiHelper::getIcon("folder", QColor("#3498db"), 128);
                } else {
                    previewItem.icon = UiHelper::getIcon("file_eps", QColor("#888888"), 128);
                }
                if (!previewItem.icon.isNull()) {
                    multiPreviewItems.append(previewItem);
                }
            }
        }

        if (firstItem) {
            firstItem = false;
            commonRating = itemRating;
            commonColor = itemColor;
            commonNote = itemNote;
            commonUrl = itemUrl;

            commonTags = QSet<QString>(itemTags.begin(), itemTags.end());
            commonTagsInitialized = true;
        } else {
            if (sameRating && itemRating != commonRating) sameRating = false;
            if (sameColor && itemColor.compare(commonColor, Qt::CaseInsensitive) != 0) sameColor = false;
            if (sameNote && itemNote != commonNote) sameNote = false;
            if (sameUrl && itemUrl != commonUrl) sameUrl = false;

            if (commonTagsInitialized) {
                QSet<QString> currentSet(itemTags.begin(), itemTags.end());
                commonTags.intersect(currentSet);
            }
        }

        // 阶段一提前收敛判断
        if (!sameRating && !sameColor && !sameNote && !sameUrl && commonTags.isEmpty()) {
            phase1EndIdx = i + 1;
            break;
        }
    }

    // 阶段二：若阶段一提前退出，仅为后序叠卡片窗口内的项提取预览图，不再计算元数据字段
    int remainingPreviewStart = qMax(phase1EndIdx, previewStartIdx);
    for (int i = remainingPreviewStart; i < paths.size() && multiPreviewItems.size() < MetaPanel::kMultiPreviewMaxCards; ++i) {
        const QString& path = paths[i];
        QModelIndex idx = pathToIndexMap.value(path, QModelIndex());
        if (idx.isValid()) {
            MetaPreviewItem previewItem;
            QVariant decData = idx.data(Qt::DecorationRole);
            bool hasThumb = idx.data(HasThumbnailRole).toBool();
            if (hasThumb) {
                previewItem.hasThumbnail = true;
                if (decData.canConvert<QIcon>()) {
                    previewItem.thumbnail = decData.value<QIcon>().pixmap(128, 128);
                } else if (decData.canConvert<QPixmap>()) {
                    previewItem.thumbnail = decData.value<QPixmap>();
                }
            } else {
                previewItem.hasThumbnail = false;
                if (decData.canConvert<QIcon>()) {
                    previewItem.icon = decData.value<QIcon>();
                }
            }
            if ((previewItem.hasThumbnail && !previewItem.thumbnail.isNull()) || (!previewItem.hasThumbnail && !previewItem.icon.isNull())) {
                multiPreviewItems.append(previewItem);
            }
        } else {
            MetaPreviewItem previewItem;
            previewItem.hasThumbnail = false;
            QFileInfo fi(path);
            if (fi.isDir()) {
                previewItem.icon = UiHelper::getIcon("folder", QColor("#3498db"), 128);
            } else {
                previewItem.icon = UiHelper::getIcon("file_eps", QColor("#888888"), 128);
            }
            if (!previewItem.icon.isNull()) {
                multiPreviewItems.append(previewItem);
            }
        }
    }

    m_metaPanel->setRating(sameRating ? commonRating : 0, false);
    m_metaPanel->setColor(sameColor ? commonColor : QString(""), false);
    m_metaPanel->setNote(sameNote ? commonNote : QString(""));
    m_metaPanel->setURL(sameUrl ? commonUrl : QString(""));
    m_metaPanel->setTags(commonTags.values());

    m_metaPanel->setPalettes({});
    m_metaPanel->setMultiSelectionPreview(multiPreviewItems);
}

void MetaPanelPresenter::updateMetaPanel(ContentPanel* panel) {
    if (!panel || !m_metaPanel) return;
    QStringList paths = panel->getSelectedPaths();
    m_metaPanel->setSelectedPaths(paths);

    if (paths.isEmpty()) {
        renderEmptySelection();
    } else if (paths.size() == 1) {
        renderSingleSelection(panel, paths.first());
    } else {
        renderMultiSelection(panel, paths);
    }
}

void MetaPanelPresenter::bindPane(ContentPanel* pane) {
    if (!pane || !m_metaPanel) return;

    connect(pane, &ContentPanel::selectionChanged, this, [this, pane](const QStringList&) {
        if (m_activeCoordinator && m_activeCoordinator->isCurrentTarget(pane)) {
            updateMetaPanel(pane);
        }
    });

    if (pane->model()) {
        connect(pane->model(), &QAbstractItemModel::dataChanged, this,
                [this, pane](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
            if (!roles.isEmpty() && !roles.contains(RatingRole) && !roles.contains(ColorRole) && !roles.contains(TagsRole) && !roles.contains(NoteRole) && !roles.contains(UrlRole)) {
                return;
            }

            auto& debounceState = m_debounceMap[pane];
            if (!debounceState.timer) {
                debounceState.timer = new QTimer(pane);
                debounceState.timer->setSingleShot(true);
                connect(pane, &QObject::destroyed, this, [this, pane]() {
                    m_debounceMap.remove(pane);
                });
                connect(debounceState.timer, &QTimer::timeout, this, [this, pane]() {
                    auto it = m_debounceMap.find(pane);
                    if (it == m_debounceMap.end()) return;

                    if (m_activeCoordinator && m_activeCoordinator->isCurrentTarget(pane)) {
                        QSet<QString> selSet;
                        for (const QString& sp : pane->getSelectedPaths()) {
                            selSet.insert(QDir::cleanPath(sp).toLower());
                        }

                        bool hasOverlap = selSet.isEmpty();
                        if (!hasOverlap) {
                            for (const QString& cp : it->pendingCleanPaths) {
                                if (selSet.contains(cp)) {
                                    hasOverlap = true;
                                    break;
                                }
                            }
                        }

                        if (hasOverlap) {
                            updateMetaPanel(pane);
                        }
                    }
                    it->pendingCleanPaths.clear();
                });
            }

            QAbstractItemModel* model = pane->model();
            int startRow = topLeft.row();
            int endRow = bottomRight.row();
            for (int r = startRow; r <= endRow; ++r) {
                QModelIndex idx = model->index(r, 0, topLeft.parent());
                if (idx.isValid()) {
                    QString path = idx.data(PathRole).toString();
                    if (!path.isEmpty()) {
                        debounceState.pendingCleanPaths.insert(QDir::cleanPath(path).toLower());
                    }
                }
            }

            // 原注释写50ms，现修正为与实际一致的30ms
            if (!debounceState.timer->isActive()) {
                debounceState.timer->start(30);
            }
        });
    }
}

void MetaPanelPresenter::wire() {
    connect(&CentralEventHub::instance(), &CentralEventHub::eventOccurred, this, &MetaPanelPresenter::handleEvent);
}

void MetaPanelPresenter::handleEvent(const QuarkMeta::AppEvent& event) {
    if (event.type != QuarkMeta::AppEventType::MetadataUpdated) {
        return;
    }

    ContentPanel* target = m_activeCoordinator ? m_activeCoordinator->currentTarget() : nullptr;
    if (!target) return;

    if (!event.targetPath.isEmpty()) {
        target->updateItemMetadata(event.targetPath);
        if (m_rootPanel && m_rootPanel != target) {
            m_rootPanel->updateItemMetadata(event.targetPath);
        }
        if (m_metaPanel) {
            QString targetClean = QDir::cleanPath(event.targetPath);
            for (const QString& p : target->getSelectedPaths()) {
                if (QString::compare(QDir::cleanPath(p), targetClean, Qt::CaseInsensitive) == 0) {
                    if (event.payload.contains("field") && event.payload["field"].toString() == "color") {
                        QString newColor = event.payload.value("value").toString();
                        m_metaPanel->setColor(newColor, false);
                    } else if (event.payload.contains("field") && event.payload["field"].toString() == "rating") {
                        int newRating = event.payload.value("value").toInt();
                        m_metaPanel->setRating(newRating, false);
                    }
                    break;
                }
            }
        }
    } else if (!event.paths.isEmpty()) {
        target->updateItemsMetadata(event.paths);
        if (m_rootPanel && m_rootPanel != target) {
            m_rootPanel->updateItemsMetadata(event.paths);
        }
    } else {
        target->refreshAll();
        if (m_rootPanel && m_rootPanel != target) {
            m_rootPanel->refreshAll();
        }
    }
}

} // namespace QuarkMeta
