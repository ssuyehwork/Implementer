#pragma once

#include <QObject>
#include <QIcon>
#include <QString>
#include <QImage>
#include <QMutex>
#include <QSet>
#include <QMap>

namespace QuarkMeta {

/**
 * @brief 系统图标加载完成通知器（必须归属主线程）
 *
 * iconLoaded()          : 兼容旧接口，无参数。
 * iconLoadedForKey(key) : 带缓存键，模型据此只刷新使用该图标的行。
 */
class IconLoadNotifier : public QObject {
    Q_OBJECT
signals:
    void iconLoaded();
    void iconLoadedForKey(const QString& key);
public:
    static IconLoadNotifier& instance() {
        static IconLoadNotifier inst;
        return inst;
    }
private:
    IconLoadNotifier(QObject* parent = nullptr) : QObject(parent) {}
};

class WindowsShellThumbnailProvider : public QObject {
    Q_OBJECT
public:
    static WindowsShellThumbnailProvider& instance();

    /**
     * @brief 系统图标缓存键的唯一计算入口（文件夹="folder"，盘符=盘符路径，文件=小写扩展名）
     * 任何需要判断"某条记录用哪个系统图标"的地方都必须调用它，禁止自行拼键。
     */
    static QString iconKey(const QString& filePath, bool isDir, const QString& suffix);

    static QIcon getFileIcon(const QString& filePath, int size = 18);
    static QIcon getFileIconFast(const QString& filePath, bool isDir, const QString& suffix);
    static bool isIconCached(const QString& filePath, bool isDir, const QString& suffix);
    static QImage getShellThumbnail(const QString& path, int size);

signals:
    void requestIconLoad(const QString& filePath, const QString& key, bool isDir, bool isRoot);

private slots:
    void handleIconLoad(const QString& filePath, const QString& key, bool isDir, bool isRoot);

private:
    WindowsShellThumbnailProvider();
    ~WindowsShellThumbnailProvider() override = default;

    static bool isDriveRootPath(const QString& filePath);

    static QMutex& fileIconMutex();
    static QMap<QString, QIcon>& fileIconCache();
    static QMutex& loadingMutex();
    static QSet<QString>& loadingKeys();

    static bool checkCircuitBreaker();
    static void recordShellFailure();
    static void recordShellSuccess();
};

} // namespace QuarkMeta