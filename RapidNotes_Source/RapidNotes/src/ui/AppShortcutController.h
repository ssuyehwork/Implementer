#pragma once

#include <QObject>
#include <QPointer>
#include <QWidget>
#include <QShortcut>

namespace QuarkMeta {

class SearchController;

/**
 * @brief 局内（应用内）快捷键控制器
 * 基于 Qt 标准 QShortcut 实现，严格限定为 Qt::WindowShortcut 作用域，绝不侵入操作系统全局
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
     * @brief Alt+Q 局内快捷键触发置顶状态翻转
     */
    void togglePinRequested();

    /**
     * @brief Tab 局内快捷键（非编辑状态）触发沉浸单栏模式切换
     */
    void toggleImmersiveRequested();

    /**
     * @brief Ctrl+Shift+N 触发新建文件夹
     */
    void createNewFolderRequested();

    /**
     * @brief Ctrl+N 触发新建 Markdown 文件
     */
    void createNewMdRequested();

    /**
     * @brief Ctrl+Shift+E 触发提取文本内容至剪贴板
     */
    void extractContentRequested();

    /**
     * @brief Ctrl+R 触发切换递归显示子文件夹中的项目
     */
    void toggleRecursiveRequested();

    /**
     * @brief Ctrl+D 触发重置所有筛选条件
     */
    void resetFilterRequested();

    /**
     * @brief Ctrl+Space 触发移入回收站
     */
    void moveToTrashRequested();

    /**
     * @brief Alt+B 触发标记/取消标记当前选中的文件夹
     */
    void toggleMarkedRequested();

    /**
     * @brief Alt+V 触发迁移选中项目到已标记的目的地文件夹
     */
    void transferToMarkedRequested();

    /**
     * @brief Alt+C 触发复制所选项目名称（顿号分割）
     */
    void copyNamesEnumeratedRequested();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void initShortcuts();

    QPointer<QWidget> m_window;
    QPointer<SearchController> m_searchController;
};

} // namespace QuarkMeta
