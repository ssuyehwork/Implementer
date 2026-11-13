#pragma once

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSlider>
#include <QHBoxLayout>
#include <QContextMenuEvent>
#include <QThreadPool>
#include <atomic>
#include <memory>
#include "CoreEngine.h"
#include "QuickLookGraphicsView.h"

namespace RapidNotes {

class FramelessWindowHelper;

class QuickLookWindow : public QWidget {
    Q_OBJECT
public:
    static QuickLookWindow& instance();

    void preview(const QString& filePath);
    QString currentPath() const { return m_currentPath; }
    void closePreview();
    void setIgnoreDeactivate(bool ignore) { m_ignoreDeactivate = ignore; }
    void handleSearchShortcut();

    static QImage loadOrExtractQuickLookImage(const QString& filePath, int targetSize, std::shared_ptr<CancellationToken> token);
    static QImage loadOrExtractQuickLookEps(const QString& filePath, int targetSize, std::shared_ptr<CancellationToken> token);

signals:
    void ratingRequested(int rating);
    void colorRequested(const QString& color);
    void prevRequested();
    void nextRequested();
    void deleteRequested(const QString& path);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    // [归一化] 8 向拖拽缩放的原生命中测试统一转发 FramelessWindowHelper（SSOT）
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;

private:
    QuickLookWindow();
    ~QuickLookWindow() override;

    void setupUi();
    void renderImage(const QString& path);
    void renderText(const QString& path);
    void showContextMenu(const QPoint& globalPos);
    
    QString detectEncoding(const QByteArray& data);
    bool isBinary(const QByteArray& data);

    void showSearchBar();
    void hideSearchBar();
    void findNext();
    void updateMatchCount();
    void updateSearchBarGeometry();
    void clearSearchHighlights();

    QuickLookGraphicsView* m_graphicsView = nullptr;
    QPlainTextEdit* m_textEdit = nullptr;
    QLabel* m_lblEmptyPrompt = nullptr;
    QLabel* m_titleLabel = nullptr;
    QLabel* m_infoLabel = nullptr;
    QWidget* m_container = nullptr;

    // 文本预览内置搜索悬浮控件
    QWidget* m_searchBar = nullptr;
    QLineEdit* m_searchEdit = nullptr;
    QPushButton* m_btnFindNext = nullptr;
    QPushButton* m_btnCount = nullptr;
    QLabel* m_lblStatus = nullptr;
    QString m_lastSearchQuery;
    int m_cachedTotalMatches = 0;
    
    QString m_currentPath;
    bool m_ignoreDeactivate = false;

    QThreadPool m_previewThreadPool; // 专属线程池，只服务预览加载，不与批量提取共享
    std::atomic<uint64_t> m_previewGeneration{1};
    std::shared_ptr<CancellationToken> m_previewToken; // 当前预览的取消令牌（仅 UI 线程读写）
    // [归一化] 8 向拖拽缩放统一走 FramelessWindowHelper（SSOT）
    FramelessWindowHelper* m_framelessHelper = nullptr;
};

} // namespace RapidNotes
