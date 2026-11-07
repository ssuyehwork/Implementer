#include "FileCreationService.h"
#include "../ui/ContentPanel.h"
#include "../ui/models/DiskItemModel.h"
#include "../core/ItemRecord.h"
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QDebug>

namespace QuarkMeta {

FileCreationService& FileCreationService::instance() {
    static FileCreationService inst;
    return inst;
}

FileCreationService::FileCreationService(QObject* parent) : QObject(parent) {}

bool FileCreationService::createNewItem(ContentPanel* panel, const QString& type) {
    if (!panel) return false;
    if (!panel->isWritablePhysicalDirectoryView()) return false;
    QString currentPath = panel->activePath();

    QString baseName = (type == "folder") ? "新建文件夹" : "未命名";
    QString ext = (type == "md") ? ".md" : ((type == "txt") ? ".txt" : "");
    QString finalName = baseName + ext;
    QString fullPath = QDir(currentPath).filePath(finalName);
    int counter = 1;

    while (QFileInfo::exists(fullPath)) {
        finalName = baseName + QString(" (%1)").arg(counter++) + ext;
        fullPath = QDir(currentPath).filePath(finalName);
    }

    bool success = false;
    if (type == "folder") {
        success = QDir(currentPath).mkdir(finalName);
    } else {
        QFile f(fullPath);
        if (f.open(QIODevice::WriteOnly)) {
            f.close();
            success = true;
        }
    }

    if (!success) return false;

    // 1. 同步将新项目追加至 Model，彻底消除全盘异步扫描的时序脱节与状态遗失
    ItemRecord newRec = ItemRecord::create(fullPath);
    if (panel->model()) {
        panel->model()->appendRecord(newRec);
    }
    panel->applyFilters();

    // 2. 强锁定焦点并即时触发 Delegate 代理重命名编辑框 (100% 稳固)
    panel->selectAndEditPath(fullPath);
    return true;
}

} // namespace QuarkMeta
