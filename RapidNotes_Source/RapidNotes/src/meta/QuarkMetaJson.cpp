#include "QuarkMetaJson.h"
#include "QuarkMetaJsonStore.h"
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QMap>
#include <QUuid>
#include <mutex>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace QuarkMeta {

QuarkMetaJson::QuarkMetaJson(const std::wstring& folderPath)
    : m_folderPath(folderPath) {
    std::wstring path = folderPath;
    if (!path.empty() && path.back() != L'\\' && path.back() != L'/') {
        path += L'\\';
    }
    // 🚨 QuarkMeta 唯一物理离散缓存文件名：.QuarkMeta.json
    m_filePath = path + L".QuarkMeta.json";
}

bool QuarkMetaJson::load() {
    m_loadFailed = false;
    QFile file(toQString(m_filePath));
    if (!file.exists()) {
        m_folder = FolderMeta();
        m_folder.metaFileId = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
        m_items.clear();
        return true;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        m_loadFailed = true;
        return false;
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        m_loadFailed = true;
        return false;
    }

    QJsonObject root = doc.object();
    if (root.contains("folder") && root.value("folder").isObject()) {
        m_folder = entryToFolder(root.value("folder").toObject());
    }

    m_items.clear();
    if (root.contains("items") && root.value("items").isObject()) {
        QJsonObject itemsObj = root.value("items").toObject();
        for (auto it = itemsObj.begin(); it != itemsObj.end(); ++it) {
            std::wstring key = normalizeItemKey(toStdWString(it.key()));
            ItemMeta item = entryToItem(it.value().toObject());
            m_items[key] = item;
        }
    }
    return true;
}

bool QuarkMetaJson::save() const {
    QJsonObject itemsObj;
    for (const auto& [name, meta] : m_items) {
        if (meta.hasUserOperations()) {
            itemsObj.insert(toQString(name), itemToEntry(meta));
        }
    }

    // 零残留物理自愈：当所有子项目的元数据都被移除且文件夹自身为默认配置时，自动物理物理物理擦除 .QuarkMeta.json
    if (itemsObj.isEmpty() && m_folder.isDefault()) {
        QString realPath = toQString(m_filePath);
        if (QFile::exists(realPath)) {
            QFile::remove(realPath);
        }
        return true;
    }

    QJsonObject root;
    root.insert("version", "2");
    root.insert("folder", folderToEntry(m_folder));
    root.insert("items", itemsObj);

    QByteArray jsonData = QJsonDocument(root).toJson(QJsonDocument::Compact);
    QString tmpPath = toQString(m_filePath) + ".tmp";

    QFile tmpFile(tmpPath);
    if (!tmpFile.open(QIODevice::WriteOnly)) return false;
    tmpFile.write(jsonData);
    tmpFile.close();

    if (!MoveFileExW(tmpPath.toStdWString().c_str(), m_filePath.c_str(), MOVEFILE_REPLACE_EXISTING)) {
        QFile::remove(tmpPath);
        return false;
    }
    // 赋予 Windows 隐藏文件属性
    SetFileAttributesW(m_filePath.c_str(), FILE_ATTRIBUTE_HIDDEN);
    return true;
}

bool QuarkMetaJson::renameItem(const QString& folderPath, const QString& oldName, const QString& newName) {
    return QuarkMetaJsonStore::instance().renameItem(folderPath, oldName, newName);
}

std::unordered_map<std::wstring, ItemMeta> QuarkMetaJson::readFolderMeta(const std::wstring& folderPath) {
    std::unordered_map<std::wstring, ItemMeta> result;
    const ItemMap items = QuarkMetaJsonStore::instance().readFolderMeta(folderPath);
    for (const auto& pair : items) {
        result[pair.first] = pair.second;
    }
    return result;
}

void QuarkMetaJson::updateItemMeta(const std::wstring& filePath, std::function<void(ItemMeta&)> updater) {
    QuarkMetaJsonStore::instance().updateItemMeta(filePath, updater);
}

bool QuarkMetaJson::migrateItemMetadata(const QString& oldFilePath, const QString& newFilePath) {
    return roamItemMetadata(oldFilePath, newFilePath, true);
}

bool QuarkMetaJson::roamBatchMetadata(const QList<QPair<QString, QString>>& pairs, bool isMove) {
    if (pairs.isEmpty()) return false;
    return QuarkMetaJsonStore::instance().transferItems(pairs, isMove, false);
}

bool QuarkMetaJson::roamItemMetadata(const QString& oldFilePath, const QString& newFilePath, bool isMove) {
    if (oldFilePath == newFilePath) return true;
    QList<QPair<QString, QString>> pairs;
    pairs.append(qMakePair(oldFilePath, newFilePath));
    return QuarkMetaJsonStore::instance().transferItems(pairs, isMove, true);
}

bool QuarkMetaJson::removeItemMetadata(const QString& filePath) {
    if (filePath.isEmpty()) return true;
    return QuarkMetaJsonStore::instance().removeItem(filePath);
}

bool QuarkMetaJson::migrateFolderCache(const QString& oldFolderPath, const QString& newFolderPath) {
    if (oldFolderPath == newFolderPath) return true;

    // 物理自愈：当发生物理文件夹重命名时，自动原子迁移其目录内部隐藏的 .QuarkMeta.json 配置
    QString oldMetaFile = oldFolderPath + "/.QuarkMeta.json";
    QString newMetaFile = newFolderPath + "/.QuarkMeta.json";

    if (QFile::exists(oldMetaFile)) {
        // 创建新物理目录（如果不存在）
        QDir().mkpath(newFolderPath);

        if (QFile::exists(newMetaFile)) {
            QFile::remove(newMetaFile);
        }
        if (QFile::copy(oldMetaFile, newMetaFile)) {
            QFile::remove(oldMetaFile);

            // 赋予 Windows 环境隐藏文件属性
            SetFileAttributesW(newMetaFile.toStdWString().c_str(), FILE_ATTRIBUTE_HIDDEN);
            return true;
        }
    }
    return true;
}

// --- 内部 JSON 结构转换实现 ---

QJsonObject QuarkMetaJson::folderToEntry(const FolderMeta& meta) {
    QJsonObject obj;
    obj.insert("sort_by", toQString(meta.sortBy));
    obj.insert("sort_order", toQString(meta.sortOrder));
    obj.insert("rating", meta.rating);
    obj.insert("color", toQString(meta.color));
    obj.insert("pinned", meta.pinned);
    obj.insert("note", toQString(meta.note));
    obj.insert("url", toQString(meta.url));
    obj.insert("encrypted", meta.encrypted);
    if (!meta.encryptSalt.empty()) {
        obj.insert("encrypt_salt", QString::fromStdString(meta.encryptSalt));
    }
    if (!meta.encryptIv.empty()) {
        obj.insert("encrypt_iv", QString::fromStdString(meta.encryptIv));
    }
    if (!meta.encryptVerifyHash.empty()) {
        obj.insert("encrypt_verify_hash", QString::fromStdString(meta.encryptVerifyHash));
    }
    if (!meta.folderId.empty()) {
        obj.insert("folder_id", QString::fromStdString(meta.folderId));
    }
    if (!meta.metaFileId.empty()) {
        obj.insert("meta_file_id", QString::fromStdString(meta.metaFileId));
    }
    QJsonArray tagsArr; for (const auto& t : meta.tags) tagsArr.append(toQString(t));
    obj.insert("tags", tagsArr);
    if (!meta.palettes.empty()) {
        QJsonArray palArr;
        for (const auto& p : meta.palettes) {
            QJsonObject pObj;
            QJsonArray cArr;
            cArr.append(p.color.red()); cArr.append(p.color.green()); cArr.append(p.color.blue());
            pObj.insert("color", cArr);
            pObj.insert("ratio", (double)p.ratio);
            palArr.append(pObj);
        }
        obj.insert("palettes", palArr);
    }
    return obj;
}

FolderMeta QuarkMetaJson::entryToFolder(const QJsonObject& obj) {
    FolderMeta meta;
    meta.sortBy = toStdWString(obj.value("sort_by").toString("name"));
    meta.sortOrder = toStdWString(obj.value("sort_order").toString("asc"));
    meta.rating = obj.value("rating").toInt();
    meta.color = toStdWString(obj.value("color").toString());
    meta.pinned = obj.value("pinned").toBool();
    meta.note = toStdWString(obj.value("note").toString());
    meta.url = toStdWString(obj.value("url").toString());
    meta.encrypted = obj.value("encrypted").toBool();
    meta.encryptSalt = obj.value("encrypt_salt").toString().toStdString();
    meta.encryptIv = obj.value("encrypt_iv").toString().toStdString();
    meta.encryptVerifyHash = obj.value("encrypt_verify_hash").toString().toStdString();
    meta.folderId = obj.value("folder_id").toString().toStdString();
    meta.metaFileId = obj.value("meta_file_id").toString().toStdString();
    if (meta.metaFileId.empty()) {
        meta.metaFileId = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    }
    if (obj.contains("tags") && obj.value("tags").isArray()) {
        for (const auto& v : obj.value("tags").toArray()) meta.tags.push_back(toStdWString(v.toString()));
    }
    if (obj.contains("palettes") && obj.value("palettes").isArray()) {
        for (const auto& v : obj.value("palettes").toArray()) {
            QJsonObject pObj = v.toObject();
            QJsonArray cArr = pObj.value("color").toArray();
            if (cArr.size() >= 3) {
                meta.palettes.push_back({QColor(cArr.at(0).toInt(), cArr.at(1).toInt(), cArr.at(2).toInt()), (float)pObj.value("ratio").toDouble()});
            }
        }
    }
    return meta;
}

QJsonObject QuarkMetaJson::itemToEntry(const ItemMeta& meta) {
    QJsonObject obj;
    QString fid = QString::fromStdString(meta.fileId).trimmed();
    if (fid.isEmpty()) {
        fid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
    obj.insert("file_id", fid);
    obj.insert("type", toQString(meta.type));
    obj.insert("rating", meta.rating);
    obj.insert("color", toQString(meta.color));
    obj.insert("pinned", meta.pinned);
    obj.insert("note", toQString(meta.note));
    obj.insert("url", toQString(meta.url));
    obj.insert("encrypted", meta.encrypted);
    if (!meta.encryptSalt.empty()) {
        obj.insert("encrypt_salt", QString::fromStdString(meta.encryptSalt));
    }
    if (!meta.encryptIv.empty()) {
        obj.insert("encrypt_iv", QString::fromStdString(meta.encryptIv));
    }
    if (!meta.encryptVerifyHash.empty()) {
        obj.insert("encrypt_verify_hash", QString::fromStdString(meta.encryptVerifyHash));
    }
    if (!meta.folderId.empty()) {
        obj.insert("folder_id", QString::fromStdString(meta.folderId));
    }
    obj.insert("original_name", toQString(meta.originalName));
    obj.insert("volume", toQString(meta.volume));
    obj.insert("frn", toQString(meta.frn));

    // 2026-07-xx 1:1对等字段写入
    obj.insert("width", meta.width);
    obj.insert("height", meta.height);
    obj.insert("auto_color", toQString(meta.autoColor));
    obj.insert("added_at", meta.addedAt);
    if (meta.thumbStatus > 0) obj.insert("thumb_status", meta.thumbStatus);
    if (meta.size > 0) obj.insert("file_size", meta.size);
    if (meta.modificationTime > 0) obj.insert("file_mtime", meta.modificationTime);

    QJsonArray tagsArr; for (const auto& t : meta.tags) tagsArr.append(toQString(t));
    obj.insert("tags", tagsArr);
    if (!meta.palettes.empty()) {
        QJsonArray palArr;
        for (const auto& p : meta.palettes) {
            QJsonObject pObj;
            QJsonArray cArr;
            cArr.append(p.color.red()); cArr.append(p.color.green()); cArr.append(p.color.blue());
            pObj.insert("color", cArr);
            pObj.insert("ratio", (double)p.ratio);
            palArr.append(pObj);
        }
        obj.insert("palettes", palArr);
    }
    return obj;
}

ItemMeta QuarkMetaJson::entryToItem(const QJsonObject& obj) {
    ItemMeta meta;
    meta.fileId = obj.value("file_id").toString().toStdString();
    if (meta.fileId.empty()) {
        meta.fileId = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    }
    meta.type = toStdWString(obj.value("type").toString("file"));
    meta.rating = obj.value("rating").toInt();
    meta.color = toStdWString(obj.value("color").toString());
    meta.pinned = obj.value("pinned").toBool();
    meta.note = toStdWString(obj.value("note").toString());
    meta.url = toStdWString(obj.value("url").toString());
    meta.encrypted = obj.value("encrypted").toBool();
    meta.encryptSalt = obj.value("encrypt_salt").toString().toStdString();
    meta.encryptIv = obj.value("encrypt_iv").toString().toStdString();
    meta.encryptVerifyHash = obj.value("encrypt_verify_hash").toString().toStdString();
    meta.folderId = obj.value("folder_id").toString().toStdString();
    meta.originalName = toStdWString(obj.value("original_name").toString());
    meta.volume = toStdWString(obj.value("volume").toString());
    meta.frn = toStdWString(obj.value("frn").toString());

    // 2026-07-xx 1:1对等字段读取
    meta.width = obj.value("width").toInt(0);
    meta.height = obj.value("height").toInt(0);
    meta.autoColor = toStdWString(obj.value("auto_color").toString());
    meta.addedAt = obj.value("added_at").toVariant().toLongLong();
    meta.thumbStatus = obj.value("thumb_status").toInt(0);
    meta.size = obj.value("file_size").toVariant().toLongLong();
    meta.modificationTime = obj.value("file_mtime").toVariant().toLongLong();

    if (obj.contains("tags") && obj.value("tags").isArray()) {
        for (const auto& v : obj.value("tags").toArray()) meta.tags.push_back(toStdWString(v.toString()));
    }
    if (obj.contains("palettes") && obj.value("palettes").isArray()) {
        for (const auto& v : obj.value("palettes").toArray()) {
            QJsonObject pObj = v.toObject();
            QJsonArray cArr = pObj.value("color").toArray();
            if (cArr.size() >= 3) {
                meta.palettes.push_back({QColor(cArr.at(0).toInt(), cArr.at(1).toInt(), cArr.at(2).toInt()), (float)pObj.value("ratio").toDouble()});
            }
        }
    }
    return meta;
}

} // namespace QuarkMeta