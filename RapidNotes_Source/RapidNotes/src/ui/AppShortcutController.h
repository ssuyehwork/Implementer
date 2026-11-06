#pragma once

#include <QObject>
#include <QPointer>
#include <QWidget>
#include <QShortcut>

namespace RapidNotes {

class SearchController;

/**
 * @brief 局内（应用内）快捷键控制器
 * 基于 Qt 标准 QShortcut / 防抢占低级钩子实现，严格限定为局内作用域，
 * 严格剔除一切物理文件操作，聚焦纯笔记领域语义
 */
class AppShortcutController : public QObject {
    Q_OBJECT

public:
    explicit AppShortcutController(QWidget* targetWindow,
                                  SearchController* searchController,
                                  QObject* parent = nullptr);
    ~AppShortcutController() override;

    static bool isEditingFocus();

signals:
    /**
     * @brief Ctrl+N: 新建灵感笔记
     */
    void createNoteRequested();

    /**
     * @brief Ctrl+Shift+N: 新建笔记分类
     */
    void createCategoryRequested();

    /**
     * @brief Ctrl+Space: 移入回收站
     */
    void moveToTrashRequested();

    /**
     * @brief Ctrl+D: 清空筛选
     */
    void resetFilterRequested();

    /**
     * @brief Alt+B: 收藏标星 (切换选中笔记 is_favorite)
     */
    void toggleFavoriteRequested();

    /**
     * @brief Alt+C: 提取标题 (多条选中笔记标题以顿号拼接复制到剪贴板)
     */
    void copyTitlesRequested();

    /**
     * @brief Alt+Q: 切换置顶
     */
    void togglePinRequested();

    /**
     * @brief Tab: 沉浸模式 (一键折叠/展开侧面板)
     */
    void toggleImmersiveRequested();

    // 兼容旧接口别名
    void createNewFolderRequested();
    void createNewMdRequested();
    void copyNamesEnumeratedRequested();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void initShortcuts();

    QPointer<QWidget> m_window;
    QPointer<SearchController> m_searchController;
};

} // namespace RapidNotes
