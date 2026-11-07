#include "FileCollisionDialog.h"
#include "UiHelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileInfo>
#include <QDir>

namespace QuarkMeta {

FileCollisionDialog::FileCollisionDialog(const QString& firstFileName,
                                           const QString& targetDirName,
                                           int totalCount,
                                           QWidget* parent)
    : FramelessDialog("QuarkMeta", parent) {
    setFixedSize(520, 180);
    setupUi(firstFileName, targetDirName, totalCount);
}

CollisionResolveAction FileCollisionDialog::selectedAction() const {
    return m_selectedAction;
}

bool FileCollisionDialog::applyToAll() const {
    return m_chkApplyToAll ? m_chkApplyToAll->isChecked() : false;
}

void FileCollisionDialog::setupUi(const QString& firstFileName, const QString& targetDirName, int totalCount) {
    QWidget* area = getContentArea();
    QVBoxLayout* mainLayout = new QVBoxLayout(area);
    mainLayout->setContentsMargins(20, 15, 20, 15);
    mainLayout->setSpacing(15);

    // 顶部消息区域：左侧蓝圈问号 Icon，右侧说明文字
    QHBoxLayout* msgLayout = new QHBoxLayout();
    msgLayout->setSpacing(15);

    QLabel* iconLabel = new QLabel(area);
    iconLabel->setFixedSize(40, 40);
    iconLabel->setPixmap(UiHelper::getIcon("info", QColor("#378ADD"), 36).pixmap(36, 36));
    msgLayout->addWidget(iconLabel, 0, Qt::AlignTop);

    QString cleanFileName = QFileInfo(firstFileName).fileName();
    if (cleanFileName.isEmpty()) cleanFileName = firstFileName;
    QString cleanDirName = QFileInfo(targetDirName).fileName();
    if (cleanDirName.isEmpty()) cleanDirName = targetDirName;

    QLabel* textLabel = new QLabel(QString("一个名为“%1”的项目已在“%2”中存在。")
                                      .arg(cleanFileName.toHtmlEscaped())
                                      .arg(cleanDirName.toHtmlEscaped()), area);
    textLabel->setWordWrap(true);
    textLabel->setObjectName("CollisionTextLabel");
    msgLayout->addWidget(textLabel, 1, Qt::AlignVCenter);

    mainLayout->addLayout(msgLayout, 1);

    // 底部控制区域：左侧复选框，右侧 4 个椭圆胶囊按钮（自动解析、替换、跳过、取消）
    QHBoxLayout* btmLayout = new QHBoxLayout();
    btmLayout->setSpacing(10);

    m_chkApplyToAll = new QCheckBox("是否应用于全部文件？", area);
    m_chkApplyToAll->setObjectName("CollisionApplyAllChk");
    if (totalCount <= 1) {
        m_chkApplyToAll->setVisible(false);
    }
    btmLayout->addWidget(m_chkApplyToAll, 0, Qt::AlignVCenter);

    btmLayout->addStretch(1);

    auto createCapsuleBtn = [this, area](const QString& text, CollisionResolveAction action) {
        QPushButton* btn = new QPushButton(text, area);
        btn->setFixedHeight(28);
        btn->setMinimumWidth(76);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setObjectName("CollisionOptionBtn");
        connect(btn, &QPushButton::clicked, this, [this, action]() {
            m_selectedAction = action;
            accept();
        });
        return btn;
    };

    btmLayout->addWidget(createCapsuleBtn("同存", CollisionResolveAction::AutoResolve));
    btmLayout->addWidget(createCapsuleBtn("替换", CollisionResolveAction::Replace));

    QPushButton* btnSkip = createCapsuleBtn("跳过", CollisionResolveAction::Skip);
    if (totalCount <= 1) {
        btnSkip->setVisible(false);
    }
    btmLayout->addWidget(btnSkip);

    btmLayout->addWidget(createCapsuleBtn("取消", CollisionResolveAction::Cancel));

    mainLayout->addLayout(btmLayout);
}

} // namespace QuarkMeta
