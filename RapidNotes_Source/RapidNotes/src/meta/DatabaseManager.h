#ifndef QuarkMeta_DATABASE_MANAGER_H
#define QuarkMeta_DATABASE_MANAGER_H

#include <QString>
#include <QObject>
#include <QRecursiveMutex>
#include <string>
#include <mutex>
#include "sqlite3.h"

namespace QuarkMeta {

/**
 * @brief 数据库事务 RAII 守卫
 */
class SqlTransaction {
public:
    explicit SqlTransaction(sqlite3* db);
    ~SqlTransaction();

    bool commit();
    void rollback();

private:
    sqlite3* m_db;
    bool m_committed = false;
    bool m_isNested = false;
};

/**
 * @brief 全局数据库管理器 (纯磁盘模式)
 */
class DatabaseManager : public QObject {
    Q_OBJECT
public:
    static DatabaseManager& instance();

    /**
     * @brief 初始化全局数据库（支持多次安全调用）
     */
    bool init();

    /**
     * @brief 显式关闭并释放数据库资源
     */
    void shutdown();

    /**
     * @brief 获取全局数据库连接（内置自愈式懒加载，永远不会返回未初始化的空指针）
     */
    sqlite3* getGlobalDb();

    /**
     * @brief 全局数据库并发读写互斥锁
     */
    static std::mutex& getGlobalMutex() { return instance().m_globalDbMutex; }
    QRecursiveMutex* dbMutex() { return &m_dbMutex; }

    static QString getAppDir();
    static QString getGlobalDbPath();
    static QString getBackupDir();

    static void logSqlError(const char* location, sqlite3* db, const QString& sql = QString());

    static void performDailyBackup();
    static bool tryRecoverFromBackup();

private:
    DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager() override;

    struct DbConnection {
        sqlite3* diskDb = nullptr;
        std::wstring diskPath;
    };

    bool loadDb(const std::wstring& diskPath, DbConnection& conn);
    void closeDb(DbConnection& conn);

    bool initLocked();

    DbConnection m_globalDb;
    std::mutex m_initMutex;
    std::mutex m_globalDbMutex;
    QRecursiveMutex m_dbMutex;
    std::once_flag m_driveMetaOnce;
    bool m_isInitialized = false;
};

} // namespace QuarkMeta

#endif // QuarkMeta_DATABASE_MANAGER_H