#include "StatusTextCoordinator.h"
#include <QTextDocument>
#include <QRegularExpression>

namespace QuarkMeta {

constexpr const char* kHighlightColorHex = "#ff551c";

StatusTextCoordinator::StatusTextCoordinator(QObject* parent)
    : QObject(parent) {
    m_finishTimer = new QTimer(this);
    m_finishTimer->setSingleShot(true);
    m_finishTimer->setInterval(3000);

    connect(m_finishTimer, &QTimer::timeout, this, [this]() {
        m_showingFinishMessage = false;
        reevaluate();
    });

    connect(&TaskProgressService::instance(), &TaskProgressService::snapshotChanged,
            this, &StatusTextCoordinator::onSnapshotChanged);
}

void StatusTextCoordinator::setNormalText(const QString& text) {
    m_normalText = text;
    reevaluate();
}

void StatusTextCoordinator::onSnapshotChanged(const TaskProgressSnapshot& snapshot) {
    if (snapshot.finished && !snapshot.finishSegments.isEmpty()) {
        m_finishSegments = snapshot.finishSegments;
        m_showingFinishMessage = true;
        m_finishTimer->start();
    }

    if (snapshot.active) {
        m_activeSegments = snapshot.statusSegments;
        m_hasActiveTask = true;
    } else {
        m_activeSegments.clear();
        m_hasActiveTask = false;
    }

    reevaluate();
}

QString StatusTextCoordinator::formatSegmentsToHtml(const QList<TextSegment>& segments) {
    QString html;
    for (const auto& seg : segments) {
        QString escaped = seg.text.toHtmlEscaped();
        if (seg.highlight) {
            html += QString("<span style=\"color:%1; font-weight:bold;\">%2</span>").arg(kHighlightColorHex, escaped);
        } else {
            html += escaped;
        }
    }
    return html;
}

QString StatusTextCoordinator::formatNormalTextToHtml(const QString& text) {
    if (text.isEmpty()) return QString();

    // 若原字符串本身已包含 html 标签，先剥离标签取纯文本
    QString plainText = text;
    if (plainText.contains('<') && plainText.contains('>')) {
        QTextDocument doc;
        doc.setHtml(plainText);
        plainText = doc.toPlainText();
    }

    QString html;
    static const QRegularExpression numReg("([0-9]+(?:[/:.][0-9]+)*%?)");
    int lastPos = 0;
    auto it = numReg.globalMatch(plainText);
    while (it.hasNext()) {
        auto match = it.next();
        int start = match.capturedStart();
        int len = match.capturedLength();
        if (start > lastPos) {
            html += plainText.mid(lastPos, start - lastPos).toHtmlEscaped();
        }
        html += QString("<span style=\"color:%1; font-weight:bold;\">%2</span>")
                    .arg(kHighlightColorHex, match.captured(1).toHtmlEscaped());
        lastPos = start + len;
    }
    if (lastPos < plainText.length()) {
        html += plainText.mid(lastPos).toHtmlEscaped();
    }
    return html;
}

void StatusTextCoordinator::reevaluate() {
    // 优先级 1：完成提示（显示 3 秒）
    if (m_showingFinishMessage) {
        emit finalTextChanged(formatSegmentsToHtml(m_finishSegments), true);
        return;
    }

    // 优先级 2：任务进行中状态
    if (m_hasActiveTask) {
        emit finalTextChanged(formatSegmentsToHtml(m_activeSegments), true);
        return;
    }

    // 优先级 3：常规文字（自动转为 HTML 高亮数字，避免显示原始代码）
    emit finalTextChanged(formatNormalTextToHtml(m_normalText), true);
}

} // namespace QuarkMeta
