#include "TaskProgressToolBar.h"
#include "../core/TaskProgressService.h"
#include "UiHelper.h"

namespace QuarkMeta {

TaskProgressToolBar::TaskProgressToolBar(QWidget* parent)
    : QWidget(parent) {
    initUi();
    bindService();
    hide();
}

void TaskProgressToolBar::initUi() {
    setFixedHeight(36);
// TaskProgressToolBar style in style.qss

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(15, 0, 15, 0);
    layout->setSpacing(10);

    m_lblTitle = new QLabel("正在处理任务...", this);
    m_lblTitle->setObjectName("TaskProgressTitleLabel");

    m_progressBar = new QProgressBar(this);
    m_progressBar->setFixedHeight(6);
    m_progressBar->setRange(0, 100);
    m_progressBar->setTextVisible(false);
    m_progressBar->setObjectName("TaskProgressBar");

    m_lblDetail = new QLabel("", this);
    m_lblDetail->setObjectName("TaskProgressDetailLabel");

    m_lblCount = new QLabel("", this);
    m_lblCount->setObjectName("TaskProgressCountLabel");

    layout->addWidget(m_lblTitle);
    layout->addWidget(m_progressBar, 1);
    layout->addWidget(m_lblDetail);
    layout->addWidget(m_lblCount);
}

void TaskProgressToolBar::bindService() {
    connect(&TaskProgressService::instance(), &TaskProgressService::jobStarted, this, [this](int, const QString& title) {
        m_lblTitle->setText(title);
        show();
    });

    connect(&TaskProgressService::instance(), &TaskProgressService::progressUpdated, this,
            [this](int percent, const QString& title, const QString& detail, int activeCount) {
        m_progressBar->setValue(percent);
        if (!title.isEmpty()) m_lblTitle->setText(title);
        m_lblDetail->setText(detail);
        m_lblCount->setText(activeCount > 1 ? QString("(%1 项并发任务)").arg(activeCount) : "");
        if (!isVisible()) show();
    });

    connect(&TaskProgressService::instance(), &TaskProgressService::allJobsFinished, this, &QWidget::hide);
}

} // namespace QuarkMeta
