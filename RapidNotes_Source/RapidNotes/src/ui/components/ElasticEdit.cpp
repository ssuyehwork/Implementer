#include "ElasticEdit.h"
#include <QTextDocument>
#include <QKeyEvent>
#include <QtMath>
#include <QEvent>

namespace QuarkMeta {

ElasticEdit::ElasticEdit(QWidget* parent) : QTextEdit(parent) {
    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Minimum);
    sp.setHeightForWidth(true);
    setSizePolicy(sp);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setLineWrapMode(QTextEdit::WidgetWidth);
    QTextOption opt = document()->defaultTextOption();
    opt.setWrapMode(QTextOption::WordWrap);
    document()->setDefaultTextOption(opt);
    document()->setDocumentMargin(2); // 留出 2px 文档内边距，保证字不贴顶且不切字
    connect(this, &QTextEdit::textChanged, this, &QWidget::updateGeometry);
}

bool ElasticEdit::hasHeightForWidth() const {
    return true;
}

int ElasticEdit::heightForWidth(int w) const {
    if (w <= 0) return 32;

    const int padTop = 4;
    const int padBottom = 4;
    const int padLeft = 8;
    const int padRight = 8;
    const int border = 1;
    const int chromeH = (padTop + padBottom) + border * 2;
    const int chromeW = (padLeft + padRight) + border * 2;

    QString text = toPlainText();
    if (text.trimmed().isEmpty()) {
        return 32;
    }

    int availableTextW = w - chromeW - static_cast<int>(document()->documentMargin() * 2);
    if (availableTextW <= 10) availableTextW = 10;

    QTextDocument doc;
    doc.setDefaultFont(font());
    doc.setDocumentMargin(document()->documentMargin());
    doc.setDefaultTextOption(document()->defaultTextOption());
    doc.setPlainText(text);
    doc.setTextWidth(availableTextW);

    int contentH = static_cast<int>(qCeil(doc.size().height()));
    int finalH = contentH + chromeH;

    return qMax(32, finalH);
}

QSize ElasticEdit::sizeHint() const {
    int w = width() > 0 ? width() : 200;
    return QSize(w, heightForWidth(w));
}

QSize ElasticEdit::minimumSizeHint() const {
    int w = width() > 0 ? width() : 200;
    return QSize(w, heightForWidth(w));
}

void ElasticEdit::changeEvent(QEvent* e) {
    QTextEdit::changeEvent(e);
    if (e && (e->type() == QEvent::StyleChange || e->type() == QEvent::FontChange)) {
        updateGeometry();
    }
}

void ElasticEdit::keyPressEvent(QKeyEvent* e) {
    if ((e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) && !(e->modifiers() & Qt::ShiftModifier)) {
        emit returnPressed();
        clearFocus();
        return;
    }
    QTextEdit::keyPressEvent(e);
}

} // namespace QuarkMeta