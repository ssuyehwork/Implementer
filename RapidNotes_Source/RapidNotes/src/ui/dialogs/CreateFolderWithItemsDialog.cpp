#include "dialogs/CreateFolderWithItemsDialog.h"
#include "UiHelper.h"
#include "../ToolTipOverlay.h"
#include "../SvgIconRenderer.h"
#include "../../util/DiskIoService.h"
#include "../../core/LastOperationManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QPointer>

namespace QuarkMeta {

CreateFolderWithItemsDialog::CreateFolderWithItemsDialog(const QString& currentDir, const QStringList& selectedPaths, QWidget* parent)
    : FramelessDialog("添加文件夹", parent), m_currentDir(currentDir), m_selectedPaths(selectedPaths) {
    resize(460, 220);
    setMinimumSize(420, 200);
    setupUi();
}

void CreateFolderWithItemsDialog::setupUi() {
    auto* mainLayout = new QVBoxLayout(getContentArea());
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(16);

    // 1. 顶部 Header 区域：左侧蓝色加号图标，右侧主副标题
    auto* headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(14);

    QLabel* iconLabel = new QLabel(this);
    iconLabel->setFixedSize(48, 48);

    // 渲染蓝色圆形背景加号图标
    QPixmap circlePixmap(48, 48);
    circlePixmap.fill(Qt::transparent);
    {
        QPainter p(&circlePixmap);
        p.setRenderHint(QPainter::Antialiasing);
        p.setBrush(QColor("#3498db"));
        p.setPen(Qt::NoPen);
        p.drawEllipse(0, 0, 48, 48);

        QIcon addIcon = UiHelper::getIcon("add", Qt::white, 24);
        addIcon.paint(&p, QRect(12, 12, 24, 24));
    }
    iconLabel->setPixmap(circlePixmap);
    headerLayout->addWidget(iconLabel);

    auto* titleTextLayout = new QVBoxLayout();
    titleTextLayout->setSpacing(4);

    QLabel* titleLabel = new QLabel("添加文件夹", this);
    titleLabel->setObjectName("CreateFolderTitleLabel");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    QLabel* subtitleLabel = new QLabel("输入新的文件夹名称", this);
    subtitleLabel->setObjectName("CreateFolderSubtitleLabel");
    QFont subFont = subtitleLabel->font();
    subFont.setPointSize(9);
    subtitleLabel->setFont(subFont);

    titleTextLayout->addWidget(titleLabel);
    titleTextLayout->addWidget(subtitleLabel);
    headerLayout->addLayout(titleTextLayout);
    headerLayout->addStretch();

    mainLayout->addLayout(headerLayout);

    // 2. 单行文件夹名称输入框
    m_nameEdit = new QLineEdit("新建文件夹", this);
    m_nameEdit->setFixedHeight(38);
    m_nameEdit->setObjectName("CreateFolderNameEdit");
    UiHelper::setupLineEditContextMenu(m_nameEdit);
    connect(m_nameEdit, &QLineEdit::returnPressed, this, &CreateFolderWithItemsDialog::onCreateAndMigrate);
    mainLayout->addWidget(m_nameEdit);

    mainLayout->addStretch();

    // 3. 底部按钮区域：左侧取消，右侧蓝底主按钮
    auto* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);

    m_btnCancel = new QPushButton("取消", this);
    m_btnCancel->setFixedSize(120, 36);
    m_btnCancel->setCursor(Qt::PointingHandCursor);
    m_btnCancel->setObjectName("CreateFolderCancelBtn");
    connect(m_btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    m_btnCreate = new QPushButton("新的文件夹名称", this);
    m_btnCreate->setFixedHeight(36);
    m_btnCreate->setMinimumWidth(150);
    m_btnCreate->setCursor(Qt::PointingHandCursor);
    m_btnCreate->setObjectName("CreateFolderCreateBtn");
    connect(m_btnCreate, &QPushButton::clicked, this, &CreateFolderWithItemsDialog::onCreateAndMigrate);

    btnLayout->addWidget(m_btnCancel);
    btnLayout->addWidget(m_btnCreate);
    mainLayout->addLayout(btnLayout);

    m_nameEdit->setFocus();
    m_nameEdit->selectAll();
}

void CreateFolderWithItemsDialog::onCreateAndMigrate() {
    QString folderName = m_nameEdit->text().trimmed();
    if (folderName.isEmpty()) {
        return;
    }

    QDir baseDir(m_currentDir);
    QString targetFolderPath = baseDir.filePath(folderName);

    // 自动避重：若文件夹已存在，增加递增编号 (例如 新建文件夹 (2))
    int counter = 2;
    while (QDir(targetFolderPath).exists()) {
        targetFolderPath = baseDir.filePath(QString("%1 (%2)").arg(folderName).arg(counter++));
    }

    if (!baseDir.mkpath(targetFolderPath)) {
        return;
    }

    // 执行所选项目移动（Cut/Move）入新创建的文件夹
    if (!m_selectedPaths.isEmpty()) {
        LastOperationManager::instance().recordMoveToFolder(targetFolderPath);

        DiskIoContext ioCtx;
        ioCtx.sources = m_selectedPaths;
        ioCtx.destination = targetFolderPath;
        ioCtx.isMove = true;

        QPointer<QWidget> pWidget(this->parentWidget());
        DiskIoService::instance().executeAsync(ioCtx, [pWidget, count = m_selectedPaths.size()](bool success) {
            QMetaObject::invokeMethod(QCoreApplication::instance(), [success, count]() {
                if (success) {
                }
            });
        });
    }

    accept();
}

} // namespace QuarkMeta
