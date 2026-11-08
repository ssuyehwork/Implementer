#include "LibraryModelBuilder.h"
#include "UiHelper.h"
#include "../meta/SmartFolderCountService.h"

namespace QuarkMeta {

LibraryModelBuilder::BuildResult LibraryModelBuilder::populateModel(QStandardItemModel* model, SmartFolderCountService* countService) {
    BuildResult result;
    if (!model) return result;

    // 一次性批量获取全部系统项与分类计数
    QMap<int, int> categoryCounts = LibraryDao::getCategoryPathCountsBatch();

    // 1. 注入 2 个固定系统分类 (全部数据, 未分类)
    auto addSystemItem = [&](const QString& name, const QString& iconKey, const QString& colorHex, int sysId, LibraryNodeKind kind) {
        int count = categoryCounts.value(sysId, 0);
        QIcon icon = UiHelper::getIcon(iconKey, QColor(colorHex), 18);
        QStandardItem* item = new QStandardItem(icon, name);
        item->setData(static_cast<int>(kind), NodeKindRole);
        item->setData(sysId, CategoryIdRole);
        item->setData(iconKey, IconKeyRole);
        item->setData(colorHex, ColorHexRole);
        item->setData(count, CountRole);

        // 向后兼容写入旧角色（防止外部读取受损）
        item->setData("system", TypeRole);
        item->setData(sysId, IdRole);
        item->setData(sysId, Qt::UserRole + 1);
        item->setData(iconKey, Qt::UserRole + 2);
        item->setData(colorHex, Qt::UserRole + 3);
        item->setData(count, Qt::UserRole + 9);

        item->setEditable(false);
        model->appendRow(item);
    };

    addSystemItem("全部数据", "all_data", "#3498db", kSystemAllId, LibraryNodeKind::SystemAll);
    addSystemItem("未分类", "uncategorized-2", "#95a5a6", kSystemUncategorizedId, LibraryNodeKind::SystemUncategorized);

    auto list = LibraryDao::getAllCategories();

    // 计算置顶项数量
    int pinnedCount = 0;
    for (const auto& rec : list) {
        if (rec.isPinned) pinnedCount++;
    }

    // 加载所有智能文件夹
    result.smartFolders = SmartFolderDao::getAllSmartFolders();

    // 2. 构建三大折叠分组标头节点
    // 快速访问标头
    QStandardItem* quickAccessHeader = new QStandardItem();
    quickAccessHeader->setData(static_cast<int>(LibraryNodeKind::SectionQuickAccess), NodeKindRole);
    quickAccessHeader->setData(pinnedCount, CountRole);
    quickAccessHeader->setData("section", TypeRole);
    quickAccessHeader->setData(-10, IdRole);
    quickAccessHeader->setData(-10, Qt::UserRole + 1);
    quickAccessHeader->setData(pinnedCount, Qt::UserRole + 9);
    quickAccessHeader->setFlags(Qt::ItemIsEnabled);
    model->appendRow(quickAccessHeader);
    result.quickAccessHeader = quickAccessHeader;

    // 智能文件夹标头
    QStandardItem* smartFolderHeader = new QStandardItem();
    smartFolderHeader->setData(static_cast<int>(LibraryNodeKind::SectionSmartFolder), NodeKindRole);
    smartFolderHeader->setData(result.smartFolders.size(), CountRole);
    smartFolderHeader->setData("section", TypeRole);
    smartFolderHeader->setData(-20, IdRole);
    smartFolderHeader->setData(-20, Qt::UserRole + 1);
    smartFolderHeader->setData(result.smartFolders.size(), Qt::UserRole + 9);
    smartFolderHeader->setFlags(Qt::ItemIsEnabled);
    model->appendRow(smartFolderHeader);
    result.smartFolderHeader = smartFolderHeader;

    // 挂载智能文件夹子项
    for (const auto& sfRec : result.smartFolders) {
        int countVal = countService ? countService->cachedCount(sfRec.id) : -999;
        QString sfIconKey = sfRec.iconKey;
        if (sfIconKey.isEmpty() || sfIconKey == "folder_filled" || sfIconKey == "folder") {
            sfIconKey = "smart-folder-1.svg";
        }
        QIcon sfIcon = UiHelper::getIcon(sfIconKey, QColor(sfRec.colorHex), 18);
        QStandardItem* sfItem = new QStandardItem(sfIcon, sfRec.name);
        sfItem->setData(static_cast<int>(LibraryNodeKind::SmartFolder), NodeKindRole);
        sfItem->setData(sfRec.id, SmartFolderIdRole);
        sfItem->setData(sfIconKey, IconKeyRole);
        sfItem->setData(sfRec.colorHex, ColorHexRole);
        sfItem->setData(countVal, CountRole);

        // 向后兼容
        sfItem->setData("smart_folder", TypeRole);
        sfItem->setData(sfRec.id, IdRole);
        sfItem->setData(sfRec.id, Qt::UserRole + 1);
        sfItem->setData(sfIconKey, Qt::UserRole + 2);
        sfItem->setData(sfRec.colorHex, Qt::UserRole + 3);
        sfItem->setData(countVal, Qt::UserRole + 9);

        sfItem->setEditable(true);
        smartFolderHeader->appendRow(sfItem);
    }

    int customCount = list.size();

    // 自定义分类“文件夹”标头
    QStandardItem* customSectionHeader = new QStandardItem();
    customSectionHeader->setData(static_cast<int>(LibraryNodeKind::SectionCustom), NodeKindRole);
    customSectionHeader->setData(customCount, CountRole);
    customSectionHeader->setData("section", TypeRole);
    customSectionHeader->setData(0, IdRole);
    customSectionHeader->setData(0, Qt::UserRole + 1);
    customSectionHeader->setData(customCount, Qt::UserRole + 9);
    customSectionHeader->setFlags(Qt::ItemIsEnabled);
    model->appendRow(customSectionHeader);
    result.customSectionHeader = customSectionHeader;

    // 3. 构建用户自定义分类并挂载在 customSectionHeader 及其子分类节点下
    QMap<int, QStandardItem*> itemMap;
    for (const auto& rec : list) {
        int count = categoryCounts.value(rec.id, 0);
        QString catIconKey = rec.iconKey;
        if (catIconKey.isEmpty() || catIconKey == "folder_filled" || catIconKey == "folder") {
            catIconKey = "Data-Classification.svg";
        }
        QIcon icon = UiHelper::getIcon(catIconKey, QColor(rec.colorHex), 18);
        QStandardItem* item = new QStandardItem(icon, rec.name);
        item->setData(static_cast<int>(LibraryNodeKind::Category), NodeKindRole);
        item->setData(rec.id, CategoryIdRole);
        item->setData(catIconKey, IconKeyRole);
        item->setData(rec.colorHex, ColorHexRole);
        item->setData(rec.isPinned, PinnedRole);
        item->setData(count, CountRole);

        // 向后兼容
        item->setData("category", TypeRole);
        item->setData(rec.id, IdRole);
        item->setData(rec.id, Qt::UserRole + 1);
        item->setData(catIconKey, Qt::UserRole + 2);
        item->setData(rec.colorHex, Qt::UserRole + 3);
        item->setData(rec.isPinned, Qt::UserRole + 4);
        item->setData(count, Qt::UserRole + 9);

        itemMap.insert(rec.id, item);

        // 若被置顶，在快速访问标头下挂载镜像节点 (PinnedMirror)
        if (rec.isPinned) {
            QStandardItem* pinnedMirrorItem = new QStandardItem(icon, rec.name);
            pinnedMirrorItem->setData(static_cast<int>(LibraryNodeKind::PinnedMirror), NodeKindRole);
            pinnedMirrorItem->setData(rec.id, CategoryIdRole);
            pinnedMirrorItem->setData(catIconKey, IconKeyRole);
            pinnedMirrorItem->setData(rec.colorHex, ColorHexRole);
            pinnedMirrorItem->setData(true, PinnedRole);
            pinnedMirrorItem->setData(count, CountRole);

            // 向后兼容
            pinnedMirrorItem->setData("category", TypeRole);
            pinnedMirrorItem->setData(rec.id, IdRole);
            pinnedMirrorItem->setData(rec.id, Qt::UserRole + 1);
            pinnedMirrorItem->setData(catIconKey, Qt::UserRole + 2);
            pinnedMirrorItem->setData(rec.colorHex, Qt::UserRole + 3);
            pinnedMirrorItem->setData(true, Qt::UserRole + 4);
            pinnedMirrorItem->setData(count, Qt::UserRole + 9);

            quickAccessHeader->appendRow(pinnedMirrorItem);
        }
    }

    for (const auto& rec : list) {
        if (!itemMap.contains(rec.id)) continue;
        QStandardItem* item = itemMap.value(rec.id);

        if (rec.parentId == kParentIdSmartFolderSection) {
            smartFolderHeader->appendRow(item);
        } else if (rec.parentId > 0 && itemMap.contains(rec.parentId)) {
            itemMap.value(rec.parentId)->appendRow(item);
        } else {
            customSectionHeader->appendRow(item);
        }
    }

    return result;
}

} // namespace QuarkMeta
