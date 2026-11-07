#pragma once
#include "ActionCommand.h"
#include "../../meta/MetadataManager.h"
#include "../../meta/FileOperationHelper.h"
#include "../../util/DiskMediaExtractor.h"
#include "../BatchRenameService.h"
#include <QString>
#include <QFile>
#include <QDir>
#include <QtConcurrent>
#include <vector>
#include <string>
#include <utility>

namespace QuarkMeta {

class BatchRenameCommand : public ActionCommand {
public:
    BatchRenameCommand(DiskOperationMode mode,
                       const std::vector<std::wstring>& oldPaths,
                       const std::vector<std::wstring>& newPaths)
        : m_mode(mode), m_oldPaths(oldPaths), m_newPaths(newPaths) {}

    void execute() override {}

    void undo() override {
        std::vector<std::wstring> oldPaths = m_oldPaths;
        std::vector<std::wstring> newPaths = m_newPaths;
        DiskOperationMode mode = m_mode;

        (void)QtConcurrent::run([oldPaths, newPaths, mode]() {
            std::vector<std::pair<std::wstring, std::wstring>> rawPairs;
            for (size_t i = 0; i < oldPaths.size(); ++i) {
                QString oldPathStr = QString::fromStdWString(oldPaths[i]);
                QString newPathStr = QString::fromStdWString(newPaths[i]);

                if (mode == DiskOperationMode::Copy) {
                    QFile::remove(newPathStr);
                    QString newThumbHashPath = DiskMediaExtractor::getDiskThumbCachePath(newPathStr);
                    if (QFile::exists(newThumbHashPath)) {
                        QFile::remove(newThumbHashPath);
                    }
                } else {
                    bool ok = false;
                    if (mode == DiskOperationMode::Move) {
                        ok = FileOperationHelper::safeMove(newPathStr, oldPathStr);
                    } else {
                        ok = FileOperationHelper::safeRename(newPathStr, oldPathStr);
                    }

                    if (ok) {
                        QString oldThumbHashPath = DiskMediaExtractor::getDiskThumbCachePath(oldPathStr);
                        QString newThumbHashPath = DiskMediaExtractor::getDiskThumbCachePath(newPathStr);
                        if (QFile::exists(newThumbHashPath)) {
                            if (mode == DiskOperationMode::Move) {
                                FileOperationHelper::safeMove(newThumbHashPath, oldThumbHashPath);
                            } else {
                                FileOperationHelper::safeRename(newThumbHashPath, oldThumbHashPath);
                            }
                        }
                        rawPairs.push_back({newPaths[i], oldPaths[i]});
                    }
                }
            }

            if (!rawPairs.empty()) {
                MetadataManager::instance().renameBatchAsync(rawPairs);
            }
        });
    }

    void redo() override {
        std::vector<std::wstring> oldPaths = m_oldPaths;
        std::vector<std::wstring> newPaths = m_newPaths;
        DiskOperationMode mode = m_mode;

        (void)QtConcurrent::run([oldPaths, newPaths, mode]() {
            std::vector<std::pair<std::wstring, std::wstring>> rawPairs;
            for (size_t i = 0; i < oldPaths.size(); ++i) {
                QString oldPathStr = QString::fromStdWString(oldPaths[i]);
                QString newPathStr = QString::fromStdWString(newPaths[i]);

                if (mode == DiskOperationMode::Copy) {
                    if (QFile::copy(oldPathStr, newPathStr)) {
                        QString oldThumbHashPath = DiskMediaExtractor::getDiskThumbCachePath(oldPathStr);
                        QString newThumbHashPath = DiskMediaExtractor::getDiskThumbCachePath(newPathStr);
                        if (QFile::exists(oldThumbHashPath)) {
                            QFile::copy(oldThumbHashPath, newThumbHashPath);
                        }
                    }
                } else {
                    bool ok = false;
                    if (mode == DiskOperationMode::Move) {
                        ok = FileOperationHelper::safeMove(oldPathStr, newPathStr);
                    } else {
                        ok = FileOperationHelper::safeRename(oldPathStr, newPathStr);
                    }

                    if (ok) {
                        QString oldThumbHashPath = DiskMediaExtractor::getDiskThumbCachePath(oldPathStr);
                        QString newThumbHashPath = DiskMediaExtractor::getDiskThumbCachePath(newPathStr);
                        if (QFile::exists(oldThumbHashPath)) {
                            if (mode == DiskOperationMode::Move) {
                                FileOperationHelper::safeMove(oldThumbHashPath, newThumbHashPath);
                            } else {
                                FileOperationHelper::safeRename(oldThumbHashPath, newThumbHashPath);
                            }
                        }
                        rawPairs.push_back({oldPaths[i], newPaths[i]});
                    }
                }
            }

            if (!rawPairs.empty()) {
                MetadataManager::instance().renameBatchAsync(rawPairs);
            }
        });
    }

    QString description() const override {
        switch (m_mode) {
            case DiskOperationMode::Rename: return "批量重命名 (磁盘)";
            case DiskOperationMode::Move: return "批量移动";
            case DiskOperationMode::Copy: return "批量复制";
        }
        return "批量操作";
    }

    bool affectsPath(const QString& path) const override {
        std::wstring p = QDir::toNativeSeparators(path).toStdWString();
        for (const auto& op : m_oldPaths) {
            if (op == p) return true;
        }
        for (const auto& np : m_newPaths) {
            if (np == p) return true;
        }
        return false;
    }

private:
    DiskOperationMode m_mode;
    std::vector<std::wstring> m_oldPaths;
    std::vector<std::wstring> m_newPaths;
};

} // namespace QuarkMeta
