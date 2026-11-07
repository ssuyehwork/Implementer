#include "ExtensionColorDao.h"
#include "DatabaseManager.h"
#include <sqlite3.h>
#include <QDateTime>

namespace QuarkMeta {

bool ExtensionColorDao::initTable() {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "CREATE TABLE IF NOT EXISTS extension_colors ("
                      "extension TEXT PRIMARY KEY, "
                      "bg_color TEXT NOT NULL, "
                      "text_color TEXT NOT NULL, "
                      "is_custom INTEGER DEFAULT 0, "
                      "updated_at INTEGER);";

    char* errMsgs = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsgs);
    if (rc != SQLITE_OK) {
        if (errMsgs) sqlite3_free(errMsgs);
        return false;
    }
    return true;
}

bool ExtensionColorDao::getColorForExtension(const QString& ext, QColor& outBg, QColor& outText) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "SELECT bg_color, text_color FROM extension_colors WHERE extension = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    std::string extStd = ext.toLower().trimmed().toStdString();
    sqlite3_bind_text(stmt, 1, extStd.c_str(), -1, SQLITE_TRANSIENT);

    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* bgStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* textStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        if (bgStr && textStr) {
            outBg = QColor(QString::fromUtf8(bgStr));
            outText = QColor(QString::fromUtf8(textStr));
            found = outBg.isValid() && outText.isValid();
        }
    }
    sqlite3_finalize(stmt);
    return found;
}

bool ExtensionColorDao::saveExtensionColor(const QString& ext, const QColor& bg, const QColor& text, bool isCustom) {
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return false;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "INSERT INTO extension_colors (extension, bg_color, text_color, is_custom, updated_at) "
                      "VALUES (?, ?, ?, ?, ?) "
                      "ON CONFLICT(extension) DO UPDATE SET "
                      "bg_color=excluded.bg_color, text_color=excluded.text_color, "
                      "is_custom=excluded.is_custom, updated_at=excluded.updated_at;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    std::string extStd = ext.toLower().trimmed().toStdString();
    std::string bgStd = bg.name().toUpper().toStdString();
    std::string textStd = text.name().toUpper().toStdString();
    qint64 nowSecs = QDateTime::currentSecsSinceEpoch();

    sqlite3_bind_text(stmt, 1, extStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, bgStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, textStd.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, isCustom ? 1 : 0);
    sqlite3_bind_int64(stmt, 5, nowSecs);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    // 固化 PASSIVE 检查点到 inspiration.db 主文件
    sqlite3_wal_checkpoint_v2(db, nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    return success;
}

QMap<QString, QPair<QColor, QColor>> ExtensionColorDao::loadAllColors() {
    QMap<QString, QPair<QColor, QColor>> resultMap;
    sqlite3* db = DatabaseManager::instance().getGlobalDb();
    if (!db) return resultMap;

    std::lock_guard<std::mutex> lock(DatabaseManager::instance().getGlobalMutex());

    const char* sql = "SELECT extension, bg_color, text_color FROM extension_colors;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return resultMap;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* extStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* bgStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* textStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        if (extStr && bgStr && textStr) {
            resultMap[QString::fromUtf8(extStr)] = { QColor(QString::fromUtf8(bgStr)), QColor(QString::fromUtf8(textStr)) };
        }
    }
    sqlite3_finalize(stmt);
    return resultMap;
}

} // namespace QuarkMeta
