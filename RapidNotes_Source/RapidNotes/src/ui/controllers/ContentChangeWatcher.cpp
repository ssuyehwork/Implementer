#include "ContentChangeWatcher.h"
#include "../ContentPanel.h"
#include "../ColumnViewWidget.h"
#include "models/DiskItemModel.h"
#include "../../core/DirectoryWatchService.h"
#include "../../core/FileFilterService.h"
#include "../../meta/MetadataManager.h"
#include "../../meta/MetaCacheDecorator.h"
#include "../../core/ItemRecord.h"
#include <QDir>
#include <QFileInfo>
#include <QApplication>
#include <QLineEdit>
#include <QThreadPool>

namespace QuarkMeta {

ContentChangeWatcher::ContentChangeWatcher(ContentPanel* panel)
    : QObject(panel), m_panel(panel) {
    m_delayedApplyTimer = new QTimer(this);
    m_delayedApplyTimer->setSingleShot(true);
    m_delayedApplyTimer->setInterval(300);
    connect(m_delayedApplyTimer, &QTimer::timeout, this, &ContentChangeWatcher::processAccumulatedChanges);

    connect(&DirectoryWatchService::instance(), &DirectoryWatchService::directoryChanged,
            this, &ContentChangeWatcher::onDirectoryChanged);
    connect(&DirectoryWatchService::instance(), &DirectoryWatchService::watchedDirectoryLost,
            this, &ContentChangeWatcher::onWatchedDirectoryLost);

    if (m_panel && m_panel->columnView()) {
        connect(m_panel->columnView(), &ColumnViewWidget::activeColumnRecordsChanged,
                this, &ContentChangeWatcher::updateTarget);
    }
}

ContentChangeWatcher::~ContentChangeWatcher() {
    unwatchAll();
}

void ContentChangeWatcher::unwatchAll() {
    for (const QString& p : m_watchedPaths) {
        DirectoryWatchService::instance().unwatch(p, m_watchedIncludeSubtree);
    }
    m_watchedPaths.clear();
    m_watchedIncludeSubtree = false;
}

bool ContentChangeWatcher::shouldWatchPath(const QString& path) const {
    if (path.isEmpty()) return false;
    if (path.contains("://")) return false;
    QFileInfo fi(path);
    return fi.exists() && fi.isDir();
}

void ContentChangeWatcher::updateTarget() {
    if (!m_panel) return;

    if (!m_panel->getCurrentCategoryType().isEmpty() ||
        m_panel->model() != m_panel->diskModel()) {
        unwatchAll();
        return;
    }

    if (m_panel->currentViewMode() == ContentPanel::ColumnView && m_panel->columnView()) {
        QStringList colPaths = m_panel->columnView()->columnPaths();
        QSet<QString> newTargetSet;
        for (const QString& cp : colPaths) {
            if (shouldWatchPath(cp)) {
                newTargetSet.insert(cp);
            }
        }

        if (newTargetSet == m_watchedPaths && !m_watchedIncludeSubtree) {
            return;
        }

        unwatchAll();
        for (const QString& tp : newTargetSet) {
            DirectoryWatchService::instance().watch(tp, false);
            m_watchedPaths.insert(tp);
        }
        m_watchedIncludeSubtree = false;
        return;
    }

    QString cPath = m_panel->currentPath();
    bool recursive = m_panel->isRecursive();

    if (!shouldWatchPath(cPath)) {
        unwatchAll();
        return;
    }

    QSet<QString> newTargetSet;
    newTargetSet.insert(cPath);

    if (m_watchedPaths == newTargetSet && m_watchedIncludeSubtree == recursive) {
        return;
    }

    unwatchAll();
    DirectoryWatchService::instance().watch(cPath, recursive);
    m_watchedPaths.insert(cPath);
    m_watchedIncludeSubtree = recursive;
}

void ContentChangeWatcher::onLoadFinished() {
    QTimer::singleShot(0, this, [this]() {
        if (!m_pendingDirtyNames.isEmpty()) {
            processAccumulatedChanges();
        }
    });
}

bool ContentChangeWatcher::isDelayed() const {
    if (!m_panel) return false;
    if (m_panel->isLoading()) return true;
    if (m_panel->isContextMenuActive()) return true;

    QWidget* fw = QApplication::focusWidget();
    if (fw && qobject_cast<QLineEdit*>(fw)) {
        QAbstractItemView* view = m_panel->activeItemView();
        if (view && view->viewport() && view->viewport()->isAncestorOf(fw)) {
            return true;
        }
    }

    return false;
}

void ContentChangeWatcher::onWatchedDirectoryLost(const QString&) {
    // 收到 watchedDirectoryLost 时，不做导航与弹窗，由现有逻辑负责
}

void ContentChangeWatcher::beginSuppress() {
    m_suppressCount++;
    m_pendingDirtyNames.clear();
    if (m_delayedApplyTimer) {
        m_delayedApplyTimer->stop();
    }
}

void ContentChangeWatcher::endSuppress() {
    if (m_suppressCount > 0) {
        m_suppressCount--;
    }
    if (m_suppressCount == 0) {
        for (const QString& wp : m_watchedPaths) {
            DirectoryWatchService::instance().discardPending(wp, m_watchedIncludeSubtree);
        }
    }
}

void ContentChangeWatcher::onDirectoryChanged(const QString& dirPath, const QStringList& dirtyNames, bool overflow) {
    if (!m_panel || m_suppressCount > 0) return;

    QString normDirPath = DirectoryWatchService::normalizePathKey(dirPath);
    bool hit = false;
    for (const QString& wp : m_watchedPaths) {
        if (DirectoryWatchService::normalizePathKey(wp) == normDirPath) {
            hit = true;
            break;
        }
    }
    if (!hit && !m_watchedIncludeSubtree) return;

    // 先过滤辅助文件：只含辅助文件的变化，不触发任何刷新，包括递归与分栏视图
    QStringList effectiveNames;
    if (!overflow) {
        for (const QString& rawName : dirtyNames) {
            QString absP = QDir(dirPath).filePath(rawName);
            if (!FileFilterService::isAuxiliaryFile(absP)) {
                effectiveNames.append(rawName);
            }
        }
        if (effectiveNames.isEmpty()) {
            return;
        }
    }

    if (m_panel->isRecursive()) {
        m_panel->refreshAll();
        return;
    }

    if (m_panel->currentViewMode() == ContentPanel::ColumnView && m_panel->columnView()) {
        m_panel->columnView()->refreshAllColumns();
        return;
    }

    if (overflow || effectiveNames.size() > 500) {
        m_pendingDirtyNames.clear();
        m_panel->refreshAll();
        return;
    }

    for (const QString& rawName : effectiveNames) {
        m_pendingDirtyNames.insert(rawName);
    }

    m_pendingDirPath = dirPath;

    if (isDelayed() || m_isTaskRunning.load()) {
        if (!m_delayedApplyTimer->isActive()) {
            m_delayedApplyTimer->start();
        }
        return;
    }

    processAccumulatedChanges();
}

void ContentChangeWatcher::processAccumulatedChanges() {
    if (m_pendingDirtyNames.isEmpty() || !m_panel) return;

    if (isDelayed() || m_isTaskRunning.load()) {
        if (!m_delayedApplyTimer->isActive()) {
            m_delayedApplyTimer->start();
        }
        return;
    }

    m_isTaskRunning.store(true);

    QStringList names = m_pendingDirtyNames.values();
    m_pendingDirtyNames.clear();
    QString dirPath = m_pendingDirPath;

    int initialReqId = m_panel->loadRequestId();
    QString initialPath = m_panel->currentPath();

    QThreadPool::globalInstance()->start([this, dirPath, names, initialReqId, initialPath]() {
        std::vector<ItemRecord> records;
        QStringList removedPaths;

        for (const QString& relName : names) {
            QString absPath = QDir(dirPath).filePath(relName);
            absPath = QFileInfo(absPath).absoluteFilePath();

            if (QFile::exists(absPath) || QDir(absPath).exists()) {
                records.push_back(ItemRecord::create(absPath, nullptr));
            } else {
                removedPaths.append(absPath);
            }
        }

        MetaCacheDecorator::decorate(records);

        QMetaObject::invokeMethod(this, [this, records = std::move(records), removedPaths = std::move(removedPaths), initialReqId, initialPath]() {
            m_isTaskRunning.store(false);

            if (!m_panel) return;
            if (m_panel->loadRequestId() != initialReqId || m_panel->currentPath() != initialPath) {
                // 发生过整体加载或目录切换，丢弃结果
                if (!m_pendingDirtyNames.isEmpty()) {
                    processAccumulatedChanges();
                }
                return;
            }

            QStringList addedPaths = m_panel->diskModel()->applyIncrementalChanges(records, removedPaths);

            if (!addedPaths.isEmpty()) {
                QStringList addedFiles;
                for (const QString& ap : addedPaths) {
                    if (!QFileInfo(ap).isDir()) {
                        addedFiles.append(ap);
                    }
                }
                if (!addedFiles.isEmpty()) {
                    MetadataManager::instance().registerItemsAsync(addedFiles);
                }
            }

            m_panel->notifyRecordsChanged();

            if (!m_pendingDirtyNames.isEmpty()) {
                processAccumulatedChanges();
            }
        }, Qt::QueuedConnection);
    });
}

} // namespace QuarkMeta
