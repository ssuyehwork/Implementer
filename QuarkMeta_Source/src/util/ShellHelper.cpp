#include "ShellHelper.h"
#include <QFileInfo>
#include <QDateTime>
#include <atomic>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include <QImage>
#include <QPixmap>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#endif

#include "../meta/MetadataManager.h"
#include "../meta/StatisticsService.h"
#include "../meta/QuarkMetaJson.h"
#include "../meta/QuarkMetaJsonStore.h"
#include "../core/DiskTrashService.h"
#include "DiskMediaExtractor.h"

namespace QuarkMeta {

bool ShellHelper::moveToTrash(const QStringList& paths) {
    return DiskTrashService::moveToDiskTrash(paths);
}

bool ShellHelper::copyOrMoveItems(const QStringList& sourcePaths, const QString& destDir, bool isMove,
                                   bool overwriteAll, bool autoRenameAll,
                                   const QSet<QString>& overwriteFiles, const QSet<QString>& autoRenameFiles,
                                   QMap<QString, QString>* outDestMap) {
#ifdef Q_OS_WIN
    if (sourcePaths.isEmpty() || destDir.isEmpty()) return false;
    QuarkMetaJsonStore::instance().flushAllDirtyBuffers(true);

    QList<QPair<QString, QString>> pathPairs;
    pathPairs.reserve(sourcePaths.size());

    std::wstring fromBuffer;
    std::wstring toBuffer;
    QSet<QString> allocatedDestPaths;

    for (const QString& p : sourcePaths) {
        QFileInfo info(p);
        QString destPath = QDir(destDir).filePath(info.fileName());

        bool shouldAutoRename = autoRenameAll || autoRenameFiles.contains(p);
        bool shouldOverwrite = overwriteAll || overwriteFiles.contains(p);

        if (QFile::exists(destPath) || allocatedDestPaths.contains(destPath.toLower())) {
            if (shouldAutoRename) {
                QString baseName = info.completeBaseName();
                QString suffix = info.suffix();
                int counter = 1;
                while (QFile::exists(destPath) || allocatedDestPaths.contains(destPath.toLower())) {
                    QString newFileName = suffix.isEmpty() 
                        ? QString("%1-%2").arg(baseName).arg(counter++)
                        : QString("%1-%2.%3").arg(baseName).arg(counter++).arg(suffix);
                    destPath = QDir(destDir).filePath(newFileName);
                }
            } else if (shouldOverwrite) {
                QFile::remove(destPath);
            }
        }

        allocatedDestPaths.insert(destPath.toLower());
        pathPairs.append(qMakePair(p, destPath));

        std::wstring fromStr = QDir::toNativeSeparators(p).toStdWString();
        std::wstring toStr = QDir::toNativeSeparators(destPath).toStdWString();

        fromBuffer.append(fromStr);
        fromBuffer.push_back(L'\0');

        toBuffer.append(toStr);
        toBuffer.push_back(L'\0');
    }

    fromBuffer.push_back(L'\0');
    toBuffer.push_back(L'\0');

    SHFILEOPSTRUCTW fileOp = { 0 };
    fileOp.wFunc = isMove ? FO_MOVE : FO_COPY;
    fileOp.pFrom = fromBuffer.c_str();
    fileOp.pTo = toBuffer.c_str();
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMMKDIR | FOF_NOCONFIRMATION | FOF_SILENT | FOF_MULTIDESTFILES;

    bool ok = (SHFileOperationW(&fileOp) == 0 && !fileOp.fAnyOperationsAborted);
    if (ok && !pathPairs.isEmpty()) {
        QuarkMetaJson::roamBatchMetadata(pathPairs, isMove);
        DiskMediaExtractor::roamThumbnailCacheBatch(pathPairs, isMove);
        MetadataManager::instance().notifyUI(MetadataManager::RefreshLevel::PathUpdate, destDir);
        if (outDestMap) {
            for (const auto& pair : pathPairs) {
                outDestMap->insert(pair.first, pair.second);
            }
        }
    }
    return ok;
#else
    Q_UNUSED(sourcePaths);
    Q_UNUSED(destDir);
    Q_UNUSED(isMove);
    Q_UNUSED(overwriteAll);
    Q_UNUSED(autoRenameAll);
    Q_UNUSED(overwriteFiles);
    Q_UNUSED(autoRenameFiles);
    Q_UNUSED(outDestMap);
    return false;
#endif
}

void ShellHelper::showProperties(const QString& path) {
#ifdef Q_OS_WIN
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_INVOKEIDLIST;
    sei.lpVerb = L"properties";
    std::wstring wpath = QDir::toNativeSeparators(path).toStdWString();
    sei.lpFile = wpath.c_str();
    sei.nShow = SW_SHOW;
    ShellExecuteExW(&sei);
#else
    Q_UNUSED(path);
#endif
}

void ShellHelper::openDirectoryInExplorer(const QString& dirPath) {
#ifdef Q_OS_WIN
    QStringList args;
    args << QDir::toNativeSeparators(dirPath);
    QProcess::startDetached("explorer", args);
#else
    Q_UNUSED(dirPath);
#endif
}

QList<ShellHelper::AssocAppInfo> ShellHelper::getAssociatedApps(const QString& filePath) {
    QList<AssocAppInfo> result;
#ifdef Q_OS_WIN
    QFileInfo info(filePath);
    QString ext = "." + info.suffix().toLower();
    if (ext == ".") return result;

    IEnumAssocHandlers* pEnum = nullptr;
    HRESULT hr = SHAssocEnumHandlers(ext.toStdWString().c_str(), ASSOC_FILTER_RECOMMENDED, &pEnum);
    if (SUCCEEDED(hr) && pEnum) {
        IAssocHandler* pHandler = nullptr;
        ULONG fetched = 0;
        while (pEnum->Next(1, &pHandler, &fetched) == S_OK && fetched == 1) {
            LPWSTR pName = nullptr;
            LPWSTR pIconPath = nullptr;
            int iconIndex = 0;

            if (SUCCEEDED(pHandler->GetUIName(&pName)) && pName) {
                QString appName = QString::fromWCharArray(pName);
                CoTaskMemFree(pName);

                AssocAppInfo appInfo;
                appInfo.name = appName;

                LPWSTR pExec = nullptr;
                if (SUCCEEDED(pHandler->GetName(&pExec)) && pExec) {
                    appInfo.execPath = QString::fromWCharArray(pExec);
                    CoTaskMemFree(pExec);
                }

                if (SUCCEEDED(pHandler->GetIconLocation(&pIconPath, &iconIndex)) && pIconPath) {
                    QString iconFile = QString::fromWCharArray(pIconPath);
                    CoTaskMemFree(pIconPath);
                    HICON hIcon = ::ExtractIconW(GetModuleHandle(NULL), iconFile.toStdWString().c_str(), iconIndex);
                    if (hIcon && hIcon != (HICON)1) {
                        appInfo.icon = QIcon(QPixmap::fromImage(QImage::fromHICON(hIcon)));
                        DestroyIcon(hIcon);
                    }
                }
                result.append(appInfo);
            }
            pHandler->Release();
            if (result.size() >= 10) break;
        }
        pEnum->Release();
    }
#else
    Q_UNUSED(filePath);
#endif
    return result;
}

void ShellHelper::showOpenWithDialog(const QString& filePath, QWidget* parent) {
#ifdef Q_OS_WIN
    OPENASINFO oaInfo = {0};
    std::wstring wpath = QDir::toNativeSeparators(filePath).toStdWString();
    oaInfo.pcszFile = wpath.c_str();
    oaInfo.pcszClass = NULL;
    oaInfo.oaifInFlags = OAIF_EXEC | OAIF_REGISTER_EXT;
    SHOpenWithDialog(parent ? (HWND)parent->winId() : NULL, &oaInfo);
#else
    Q_UNUSED(parent);
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
#endif
}

void ShellHelper::openInExplorer(const QString& path) {
#ifdef Q_OS_WIN
    QStringList args;
    args << "/select," << QDir::toNativeSeparators(path);
    QProcess::startDetached("explorer", args);
#else
    Q_UNUSED(path);
#endif
}

bool ShellHelper::renameItem(const QString& oldPath, const QString& newPath) {
    QuarkMetaJsonStore::instance().flushAllDirtyBuffers(true);
    if (QFile::rename(oldPath, newPath)) {
        // 1. 物理漫游迁移 .QuarkMeta.json 元数据 
        QuarkMetaJson::migrateItemMetadata(oldPath, newPath);
        // 同步数据库
        MetadataManager::instance().renameItem(oldPath.toStdWString(), newPath.toStdWString());
        return true;
    }
    return false;
}

QString ShellHelper::formatSize(qint64 bytes) {
    if (bytes < 1024) return QString("%1 B").arg(bytes);
    if (bytes < 1024 * 1024) return QString("%1 KB").arg(bytes / 1024.0, 0, 'f', 2);
    if (bytes < 1024LL * 1024 * 1024) return QString("%1 MB").arg(bytes / (1024.0 * 1024.0), 0, 'f', 2);
    return QString("%1 GB").arg(bytes / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2);
}

void ShellHelper::ensureHidden(const std::wstring& path) {
#ifdef Q_OS_WIN
    SetFileAttributesW(path.c_str(), FILE_ATTRIBUTE_HIDDEN);
#else
    Q_UNUSED(path);
#endif
}

} // namespace QuarkMeta
