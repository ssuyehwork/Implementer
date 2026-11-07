#pragma once

#include <QObject>
#include <QString>
#include <QList>

namespace QuarkMeta {

class NavigationService : public QObject {
    Q_OBJECT

public:
    static NavigationService& instance();

    // 核心导航调度接口
    void navigateTo(const QString& rawUrl, bool recordHistory = true, bool recursive = false);
    void openInNewTab(const QString& rawPath);
    void goBack();
    void goForward();
    void goUp();
    void refresh();

    QString takePendingGoUpChildPath();

    // 状态查询接口
    QString currentUrl() const { return m_currentUrl; }
    QString currentDisplayPath() const;
    bool isVirtualProtocol() const;
    bool canGoBack() const { return m_currentIndex > 0; }
    bool canGoForward() const { return m_currentIndex < m_history.size() - 1; }
    bool canGoUp() const;

signals:
    /**
     * @brief 全局请求在新标签页打开路径信号
     */
    void requestOpenInNewTab(const QString& url);

    /**
     * @brief 全局统一路径变更信号 (驱动各子面板单向加载数据)
     * @param url 标准协议 URL (如 file://C:/Users 或 computer://)
     * @param displayPath 适合 UI 面包屑展示的文本 (如 C:\Users 或 此电脑)
     */
    void currentUrlChanged(const QString& url, const QString& displayPath, bool recursive);

    /**
     * @brief 导航可用性状态变动信号 (驱动前进/后退/上级按钮状态)
     */
    void navStateChanged(bool canBack, bool canForward, bool canUp);

private:
    explicit NavigationService(QObject* parent = nullptr);
    ~NavigationService() override = default;
    NavigationService(const NavigationService&) = delete;
    NavigationService& operator=(const NavigationService&) = delete;

    QString normalizeUrl(const QString& rawUrl) const;
    void emitNavState();

    QString m_currentUrl;
    QList<QString> m_history;
    int m_currentIndex = -1;
    QString m_pendingGoUpChildPath;
    static constexpr int kMaxHistoryDepth = 100;
};

} // namespace QuarkMeta
