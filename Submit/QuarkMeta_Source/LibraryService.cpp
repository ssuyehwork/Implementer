#include "LibraryService.h"
#include "LibrarySyncService.h"
#include "../ui/UiHelper.h"
#include "../ui/ToolTipOverlay.h"
#include <QCursor>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QHash>

namespace QuarkMeta {

LibraryService& LibraryService::instance() {
    static LibraryService s_instance;
    return s_instance;
}

LibraryService::LibraryService(QObject* parent) : QObject(parent) {
    LibraryDao::initTable();
}

bool LibraryService::isInLibrary(const QString& path) const {
    if (path.isEmpty()) return false;
    QStringList allPaths = LibraryDao::getCategoryPaths(-1);
    return allPaths.contains(path, Qt::CaseInsensitive);
}

int LibraryService::createCategory(const QString& name, int parentId) {
    int id = LibraryDao::addCategory(name, parentId);
    if (id > 0) {
        emit libraryChanged();
    }
    return id;
}

bool LibraryService::removeCategory(int id) {
    QStringList paths = LibraryDao::getCategoryPaths(id);
    bool ok = LibraryDao::removeCategoryById(id);
    if (ok) {
        LibrarySyncService::instance().removePaths(paths);
        emit libraryChanged();
    }
    return ok;
}

bool LibraryService::bindPathsToCategory(int id, const QStringList& paths) {
    bool ok = LibraryDao::bindPathsToCategory(id, paths);
    if (ok) {
        emit libraryChanged();
    }
    return ok;
}

int LibraryService::syncWatchedFolderHierarchy(const QString& rootFolderPath) {
    QString cleanRoot = QDir::toNativeSeparators(QDir::cleanPath(rootFolderPath));
    QFileInfo rootInfo(cleanRoot);
    if (!rootInfo.exists() || !rootInfo.isDir()) return 0;

    QString rootName = rootInfo.fileName();
    if (rootName.isEmpty()) {
        rootName = rootInfo.absoluteFilePath();
    }

    int rootCatId = LibraryDao::getOrCreateCategory(rootName, 0);
    if (rootCatId <= 0) return 0;

    // 批量映射：分类 ID -> 待绑定的所有路径列表
    QMap<int, QStringList> batchCatPaths;

    // 根目录仅用于命名分类和递归扫描，不作为库条目；仅绑定其中的直接子文件
    QStringList rootPaths;
    QDir rDir(cleanRoot);
    QFileInfoList rootEntries = rDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (const auto& fi : rootEntries) {
        rootPaths << QDir::toNativeSeparators(fi.absoluteFilePath());
    }
    batchCatPaths[rootCatId] = rootPaths;

    // 缓存已创建分类路径到 ID 的映射
    QHash<QString, int> folderCatMap;
    folderCatMap[cleanRoot.toLower()] = rootCatId;

    // 递归遍历所有子目录
    QDirIterator it(cleanRoot, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        QString subDirPath = QDir::toNativeSeparators(it.filePath());
        QFileInfo subInfo(subDirPath);
        QString parentDirPath = QDir::toNativeSeparators(subInfo.dir().absolutePath());

        int parentCatId = folderCatMap.value(parentDirPath.toLower(), rootCatId);
        int subCatId = LibraryDao::getOrCreateCategory(subInfo.fileName(), parentCatId);
        if (subCatId > 0) {
            folderCatMap[subDirPath.toLower()] = subCatId;
            QStringList subPaths;
            subPaths << subDirPath;
            QDir sDir(subDirPath);
            QFileInfoList fileEntries = sDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
            for (const auto& fi : fileEntries) {
                subPaths << QDir::toNativeSeparators(fi.absoluteFilePath());
            }
            batchCatPaths[subCatId] = subPaths;
        }
    }

    // 单次原子批量写入与物理落盘，彻底消除数百次 winSync 导致的主线程阻塞
    if (!batchCatPaths.isEmpty()) {
        LibraryDao::bindPathsToCategoriesBatch(batchCatPaths);
    }

    emit libraryChanged();
    return rootCatId;
}

bool LibraryService::addPathsToCategory(int id, const QStringList& paths) {
    bool ok = LibraryDao::addPathsToCategory(id, paths);
    if (ok) {
        LibrarySyncService::instance().syncPaths(paths);
        emit libraryChanged();
    }
    return ok;
}

bool LibraryService::removePathsFromCategory(int id, const QStringList& paths) {
    bool ok = LibraryDao::removePathsFromCategory(id, paths);
    if (ok) {
        LibrarySyncService::instance().removePaths(paths);
        emit libraryChanged();
    }
    return ok;
}

bool LibraryService::removePathsFromLibrary(const QStringList& paths) {
    if (paths.isEmpty()) return false;
    bool ok = LibraryDao::removePathsFromLibrary(paths);
    if (ok) {
        LibrarySyncService::instance().removePaths(paths);
        emit libraryChanged();
    }
    return ok;
}

QStringList LibraryService::getCategoryPaths(int id) const {
    return LibraryDao::getCategoryPaths(id);
}

QAction* LibraryService::buildLibraryAction(QMenu* parentMenu, const QStringList& targetPaths, QObject* receiver) {
    if (!parentMenu || targetPaths.isEmpty()) return nullptr;

    bool anyInLibrary = false;
    for (const QString& path : targetPaths) {
        if (isInLibrary(path)) {
            anyInLibrary = true;
            break;
        }
    }

    if (anyInLibrary) {
        QIcon removeIcon = UiHelper::getIcon("close", QColor("#EEEEEE"), 18);
        QAction* action = parentMenu->addAction(removeIcon, "从“库”中移除");

        connect(action, &QAction::triggered, receiver ? receiver : parentMenu, [this, targetPaths]() {
            if (removePathsFromLibrary(targetPaths)) {
                ToolTipOverlay::instance()->showToast(QString("已将 %1 个项目从库中移除").arg(targetPaths.size()), 1500, QColor("#2ecc71"));
            } else {
                ToolTipOverlay::instance()->showToast("移除失败", 1500, QColor("#e81123"));
            }
        });

        return action;
    } else {
        QIcon addIcon = UiHelper::getIcon("Database-1.svg", QColor("#EEEEEE"), 18);
        QAction* action = parentMenu->addAction(addIcon, "添加至“库”");

        connect(action, &QAction::triggered, receiver ? receiver : parentMenu, [this, targetPaths]() {
            if (addPathsToCategory(-2, targetPaths)) {
                ToolTipOverlay::instance()->showToast(QString("已将 %1 个项目归入未分类").arg(targetPaths.size()), 1500, QColor("#2ecc71"));
            } else {
                ToolTipOverlay::instance()->showToast("归入失败", 1500, QColor("#e81123"));
            }
        });

        return action;
    }
}

} // namespace QuarkMeta
