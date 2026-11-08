#pragma once

#include <QStyledItemDelegate>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include "LibraryNodeKind.h"
#include "LibraryTreeLayout.h"

namespace QuarkMeta {

/**
 * @brief 分组标头行内部子区域悬停/点击枚举
 */
enum class SectionHoverArea {
    None,       // 无悬停
    Row,        // 整行空白处悬停
    Header,     // 折叠组件块区域（文字+箭头）
    AddButton   // 右侧"+"按钮区域
};

/**
 * @brief 库树视图项目渲染委托
 * 物理独立拆分，严格遵循单一职责原则：只负责视觉绘制与交互点击检测，不含业务逻辑。
 */
class LibraryItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit LibraryItemDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
    ~LibraryItemDelegate() override = default;

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

    void setFilterKeyword(const QString& keyword) { m_filterKeyword = keyword; }

    // 静态布局与字体文字提取（单一真理源）
    static QString sectionTitleText(LibraryNodeKind kind, int count) {
        return sectionTitle(kind, count);
    }

    static QFont sectionFont(const QFont& baseFont) {
        QFont font = baseFont;
        font.setPointSize(10);
        font.setBold(true);
        return font;
    }

    static SectionHoverArea hitTestSectionRow(const QRect& rowRect, LibraryNodeKind kind, int count, const QFont& baseFont, const QPoint& pos);

    // 静态布局转发（兼容旧调用或布局查询）
    static QRect addBtnRect(const QRect& optionRect) {
        return LibraryTreeLayout::addBtnRect(optionRect);
    }
    static QRect sectionHeaderRect(const QRect& optionRect, const QString& text, const QFont& font) {
        return LibraryTreeLayout::sectionHeaderRect(optionRect, text, font);
    }

signals:
    void requestAddCategory(LibraryNodeKind kind);
    void requestToggleSection(LibraryNodeKind kind);

private:
    void paintHighlightedText(QPainter* painter, const QRect& rect, const QString& text, const QString& keyword, const QFont& font, const QColor& normalColor, const QColor& matchColor) const;

    QString m_filterKeyword;
};

} // namespace QuarkMeta
