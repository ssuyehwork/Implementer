#pragma once

#include <QObject>
#include <QHash>
#include <QString>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

namespace QuarkMeta {

class ContentPanel;

class AlphabetIndexBar : public QObject {
public:
    static constexpr int kSpacingPx = 10;

    explicit AlphabetIndexBar(ContentPanel* panel);

    void initialize();
    void update();
    void updateGeometry();
    void setRightMargin(int margin) { m_rightMargin = qMax(0, margin); }

private:
    void clearViewRightReserve();
    static QString classifyIndexKey(const QString& itemName);
    void jumpToAlphabetKey(const QString& key);

    ContentPanel* m_panel = nullptr;
    QWidget* m_bar = nullptr;
    QVBoxLayout* m_layout = nullptr;
    QHash<QString, QPushButton*> m_buttons;
    QHash<QString, int> m_cycleCursor;
    QString m_activeKey;
    int m_rightMargin = kSpacingPx;
};

} // namespace QuarkMeta