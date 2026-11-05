#pragma once

#include <QObject>
#include <QSplitter>
#include <QMainWindow>
#include <QMenu>
#include <QPoint>
#include <QPointer>

namespace RapidNotes {

class PanelLayoutManager : public QObject {
    Q_OBJECT

public:
    explicit PanelLayoutManager(QMainWindow* mainWindow,
                                QSplitter* mainSplitter,
                                QWidget* sidebarPanel,
                                QWidget* contentPanel,
                                QWidget* detailPanel = nullptr,
                                QObject* parent = nullptr);
    ~PanelLayoutManager() override = default;

    void initLayout();
    void resetSplitterLayout();
    void setPanelVisible(const QString& panelId, bool visible);
    bool isPanelVisible(const QString& panelId) const;
    void saveLayoutState();
    void restoreLayoutState();

signals:
    void panelVisibilityChanged(const QString& panelId, bool visible);

private:
    QPointer<QMainWindow> m_mainWindow;
    QPointer<QSplitter> m_mainSplitter;
    QPointer<QWidget> m_sidebarPanel;
    QPointer<QWidget> m_contentPanel;
    QPointer<QWidget> m_detailPanel;

    QByteArray m_savedSplitterState;
};

} // namespace RapidNotes
