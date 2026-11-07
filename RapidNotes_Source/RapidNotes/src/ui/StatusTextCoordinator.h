#pragma once

#include <QObject>
#include <QString>
#include <QTimer>
#include "../core/TaskProgressService.h"

namespace QuarkMeta {

class StatusTextCoordinator : public QObject {
    Q_OBJECT

public:
    explicit StatusTextCoordinator(QObject* parent = nullptr);
    ~StatusTextCoordinator() override = default;

    void setNormalText(const QString& text);

signals:
    void finalTextChanged(const QString& text, bool isRichText);

private:
    void onSnapshotChanged(const TaskProgressSnapshot& snapshot);
    void reevaluate();
    QString formatSegmentsToHtml(const QList<TextSegment>& segments);
    QString formatNormalTextToHtml(const QString& text);

    QString m_normalText;
    QList<TextSegment> m_activeSegments;
    bool m_hasActiveTask = false;

    QList<TextSegment> m_finishSegments;
    bool m_showingFinishMessage = false;

    QTimer* m_finishTimer = nullptr;
};

} // namespace QuarkMeta
