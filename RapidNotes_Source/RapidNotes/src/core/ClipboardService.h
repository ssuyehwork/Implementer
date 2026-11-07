#pragma once

#include <QObject>
#include <QStringList>
#include <QWidget>
#include <QAbstractItemView>

namespace QuarkMeta {

class ClipboardService : public QObject {
    Q_OBJECT

public:
    static ClipboardService& instance();

    void copyItems(const QStringList& paths);
    void cutItems(const QStringList& paths);
    bool canPaste(const QString& targetDir) const;
    void executePaste(const QString& targetDir, QWidget* parentWidget = nullptr);

    // 标签剪贴板方法
    void setCopiedTags(const QStringList& tags);
    QStringList copiedTags() const;
    bool hasCopiedTags() const;
    void clearCopiedTags();

    /**
     * @brief 粘贴标签至目标视图选中项的 SSOT 统一入口 (包含校验、Model更新与 ToolTip 提示)
     */
    bool executePasteTags(QAbstractItemView* view);

signals:
    void pasteCompleted(const QString& targetDir);

private:
    QStringList m_copiedTags;
    explicit ClipboardService(QObject* parent = nullptr);
    ~ClipboardService() override = default;
    ClipboardService(const ClipboardService&) = delete;
    ClipboardService& operator=(const ClipboardService&) = delete;
};

} // namespace QuarkMeta
