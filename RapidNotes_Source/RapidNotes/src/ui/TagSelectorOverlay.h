#pragma once

#include <QFrame>
#include <QLineEdit>
#include <QShowEvent>
#include <QListWidget>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QMap>
#include <QStringList>
#include "components/FlowLayout.h"
#include "../core/TagLexiconService.h"
#include "FramelessWindowHelper.h"

namespace QuarkMeta {

/**
 * @brief 触发来源契约枚举
 * 明确区分浮层的业务发起方，决定落盘生命周期策略
 */
enum class TagTriggerSource {
    MetaPanel,         // 来自元数据属性面板：失焦/关闭时自动执行单次原子落盘
    PresetTagsDialog,  // 来自预设标签弹窗：仅响应内存UI，落盘由外部对话框的“保存”按钮接管
    SmartFolderDialog  // 来自智能文件夹规则配置：仅响应内存UI，关闭时不写盘
};

class TagSelectorOverlay : public QFrame {
    Q_OBJECT
public:
    TagSelectorOverlay(const QStringList& initialSelected, QWidget* parent = nullptr, TagTriggerSource source = TagTriggerSource::MetaPanel);
    TagSelectorOverlay(const QStringList& initialSelected, const QStringList& candidatePaths, QWidget* parent = nullptr, TagTriggerSource source = TagTriggerSource::MetaPanel);
    ~TagSelectorOverlay() override;

signals:
    /**
     * @brief 前端内存实时响应信号（仅更新UI胶囊展示，绝对不碰数据库和磁盘）
     */
    void selectionChanged(const QStringList& selectedTags);

    /**
     * @brief 离场持久化提交信号（仅在 MetaPanel 模式下失焦关闭时发射一次）
     */
    void tagsCommitted(const QStringList& finalTags);

    void overlayClosed();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void changeEvent(QEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;

private:
    void initUi();
    void loadRecentAndRecommendedTags(const QStringList& candidatePaths);
    void filterAndPopulate();
    void toggleTagSelection(const QString& tag);
    void updateTagButtonsSelection();
    void recordRecentTag(const QString& tag);

public:
    void closeOverlay();

private:
    TagTriggerSource m_triggerSource = TagTriggerSource::MetaPanel;
    QStringList m_selectedTags;
    QStringList m_initialTags;
    QStringList m_recentTags;
    QStringList m_recommendedTags;

    QLineEdit* m_searchEdit = nullptr;
    QScrollArea* m_scrollArea = nullptr;
    QWidget* m_bodyContainer = nullptr;
    QVBoxLayout* m_bodyLayout = nullptr;

    QWidget* m_recSectionWidget = nullptr;
    FlowLayout* m_recFlowLayout = nullptr;
    QPushButton* m_recHeaderBtn = nullptr;
    bool m_recExpanded = true;

    QWidget* m_recentSectionWidget = nullptr;
    FlowLayout* m_recentFlowLayout = nullptr;
    QPushButton* m_recentHeaderBtn = nullptr;
    bool m_recentExpanded = true;

    bool isInternalWidget(QObject* obj) const;

    bool m_isClosing = false;
    bool m_hasCommitted = false;
    bool m_isDragging = false;
    bool m_swallowingClick = false;
    QPoint m_dragPos;

    FramelessWindowHelper* m_framelessHelper = nullptr;
};

} // namespace QuarkMeta