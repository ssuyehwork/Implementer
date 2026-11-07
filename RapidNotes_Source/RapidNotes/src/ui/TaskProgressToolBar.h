#pragma once

#include <QWidget>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

namespace QuarkMeta {

class TaskProgressToolBar : public QWidget {
    Q_OBJECT

public:
    explicit TaskProgressToolBar(QWidget* parent = nullptr);
    ~TaskProgressToolBar() override = default;

private:
    void initUi();
    void bindService();

    QProgressBar* m_progressBar = nullptr;
    QLabel* m_lblTitle = nullptr;
    QLabel* m_lblDetail = nullptr;
    QLabel* m_lblCount = nullptr;
    QPushButton* m_btnCancel = nullptr;
};

} // namespace QuarkMeta
