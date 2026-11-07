#include "BatchRenameService.h"
#include "OperationSnapshotEngine.h"
#include "UndoManager.h"
#include "BasicCommands.h"
#include "commands/BatchRenameCommand.h"
#include "../meta/MetadataManager.h"
#include "../meta/QuarkMetaJson.h"
#include "../meta/FileOperationHelper.h"
#include "../util/DiskMediaExtractor.h"
#include "../ui/UndoToastOverlay.h"
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QtConcurrent>
#include <QCoreApplication>

namespace QuarkMeta {

BatchRenameService& BatchRenameService::instance() {
    static BatchRenameService s_instance;
    return s_instance;
}

BatchRenameService::BatchRenameService(QObject* parent) : QObject(parent) {}

QString BatchRenameService::processOne(const QString& path, int index, const std::vector<RenameRule>& rules) {
    QFileInfo info(path);
    QString newName = "";

    for (const auto& rule : rules) {
        switch (rule.type) {
            case RenameComponentType::Text:
                newName += rule.value;
                break;
            case RenameComponentType::Sequence: {
                int val = rule.start + (index * rule.step);
                newName += QString::number(val).rightJustified(rule.padding, '0');
                break;
            }
            case RenameComponentType::Date:
                newName += QDateTime::currentDateTime().toString(rule.value.isEmpty() ? "yyyyMMdd" : rule.value);
                break;
            case RenameComponentType::OriginalName:
                newName += info.baseName();
                break;
            case RenameComponentType::Metadata:
                newName += "[QuarkMeta]";
                break;
            case RenameComponentType::ParentFolderName:
                newName += info.dir().dirName();
                break;
        }
    }

    QString ext = info.suffix();
    if (!ext.isEmpty()) newName += "." + ext;
    return newName;
}

std::vector<std::wstring> BatchRenameService::computePreview(const std::vector<std::wstring>& originalPaths,
                                                             const std::vector<RenameRule>& rules) {
    std::vector<std::wstring> results;
    results.reserve(originalPaths.size());

    for (size_t i = 0; i < originalPaths.size(); ++i) {
        QString path = QString::fromStdWString(originalPaths[i]);
        results.push_back(processOne(path, static_cast<int>(i), rules).toStdWString());
    }
    return results;
}

void BatchRenameService::executeAsync(const std::vector<std::wstring>& originalPaths,
                                      const std::vector<std::wstring>& newNames,
                                      DiskOperationMode mode,
                                      const QString& targetDir,
                                      QWidget* parentWidget,
                                      std::function<void(int successCount)> callback) {
    if (originalPaths.empty() || originalPaths.size() != newNames.size()) {
        if (callback) callback(0);
        return;
    }

    std::vector<std::wstring> oldPathsSnap = originalPaths;
    std::vector<std::wstring> newPathsSnap;
    newPathsSnap.reserve(originalPaths.size());

    for (size_t i = 0; i < originalPaths.size(); ++i) {
        QString oldPath = QString::fromStdWString(originalPaths[i]);
        QFileInfo oldInfo(oldPath);
        QString destDir = (mode == DiskOperationMode::Rename) ? oldInfo.absolutePath() : targetDir;
        QString newPathStr = QDir(destDir).filePath(QString::fromStdWString(newNames[i]));
        newPathsSnap.push_back(QDir::toNativeSeparators(newPathStr).toStdWString());
    }

    (void)QtConcurrent::run([oldPathsSnap, newPathsSnap, mode, targetDir, parentWidget, callback]() {
        int successCount = 0;
        std::vector<std::pair<std::wstring, std::wstring>> rawPairs;

        for (size_t i = 0; i < oldPathsSnap.size(); ++i) {
            QString oldPath = QString::fromStdWString(oldPathsSnap[i]);
            QString newPath = QString::fromStdWString(newPathsSnap[i]);

            bool ok = false;
            if (mode == DiskOperationMode::Copy) {
                ok = QFile::copy(oldPath, newPath);
            } else if (mode == DiskOperationMode::Move) {
                ok = FileOperationHelper::safeMove(oldPath, newPath);
            } else {
                ok = FileOperationHelper::safeRename(oldPath, newPath);
            }

            if (ok) {
                successCount++;

                bool isMoveOperation = (mode != DiskOperationMode::Copy);
                QuarkMetaJson::roamItemMetadata(oldPath, newPath, isMoveOperation);
                DiskMediaExtractor::roamThumbnailCache(oldPath, newPath, isMoveOperation);

                QString oldThumbHashPath = DiskMediaExtractor::getDiskThumbCachePath(oldPath);
                QString newThumbHashPath = DiskMediaExtractor::getDiskThumbCachePath(newPath);

                if (QFile::exists(oldThumbHashPath)) {
                    if (mode == DiskOperationMode::Copy) {
                        QFile::copy(oldThumbHashPath, newThumbHashPath);
                    } else if (mode == DiskOperationMode::Move) {
                        FileOperationHelper::safeMove(oldThumbHashPath, newThumbHashPath);
                    } else {
                        FileOperationHelper::safeRename(oldThumbHashPath, newThumbHashPath);
                    }
                }

                if (mode != DiskOperationMode::Copy) {
                    rawPairs.push_back({oldPathsSnap[i], newPathsSnap[i]});
                }
            }
        }

        auto onFinishedInMain = [oldPathsSnap, newPathsSnap, mode, parentWidget, callback, successCount]() {
            if (successCount > 0) {
                UndoManager::instance().pushCommand(
                    std::make_unique<BatchRenameCommand>(mode, oldPathsSnap, newPathsSnap)
                );

                UndoToastOverlay::instance()->showToast(
                    parentWidget,
                    QString("成功处理 %1 个项目").arg(successCount),
                    [successCount]() {
                        if (successCount > 0) {
                            UndoManager::instance().undo();
                        }
                    },
                    7000
                );
            }

            if (callback) callback(successCount);
        };

        if (mode == DiskOperationMode::Copy) {
            QMetaObject::invokeMethod(qApp, onFinishedInMain, Qt::QueuedConnection);
        } else {
            MetadataManager::instance().renameBatchAsync(rawPairs, [onFinishedInMain](int) {
                onFinishedInMain();
            });
        }
    });
}

} // namespace QuarkMeta
