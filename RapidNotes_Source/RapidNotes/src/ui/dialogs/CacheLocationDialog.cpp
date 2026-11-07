#include "dialogs/CacheLocationDialog.h"
#include "UiHelper.h"
#include "../core/AppConfig.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDirIterator>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

namespace QuarkMeta {

CacheLocationDialog::CacheLocationDialog(QWidget* parent)
    : FramelessDialog("缩略图与缓存位置设置", parent) {
    resize(520, 240);
    setupUi();
    updatePathDisplay();
}

void CacheLocationDialog::setupUi() {
    auto* mainLayout = new QVBoxLayout(getContentArea());
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    auto* pathLayout = new QHBoxLayout();
    m_pathEdit = new QLineEdit(this);
    m_pathEdit->setReadOnly(true);
    UiHelper::setupLineEditContextMenu(m_pathEdit);

    m_btnBrowse = new QPushButton("浏览...", this);
    connect(m_btnBrowse, &QPushButton::clicked, this, &CacheLocationDialog::onBrowsePath);

    pathLayout->addWidget(m_pathEdit);
    pathLayout->addWidget(m_btnBrowse);
    mainLayout->addLayout(pathLayout);

    m_sizeLabel = new QLabel("当前缓存占用: 计算中...", this);
    mainLayout->addWidget(m_sizeLabel);

    m_chkMigrateData = new QCheckBox("迁移现有缓存数据到新目录", this);
    m_chkMigrateData->setChecked(true);
    mainLayout->addWidget(m_chkMigrateData);

    mainLayout->addStretch();

    auto* btnLayout = new QHBoxLayout();
    m_btnRestore = new QPushButton("恢复默认", this);
    connect(m_btnRestore, &QPushButton::clicked, this, &CacheLocationDialog::onRestoreDefault);

    m_btnCancel = new QPushButton("取消", this);
    connect(m_btnCancel, &QPushButton::clicked, this, &CacheLocationDialog::reject);

    m_btnSave = new QPushButton("保存", this);
    connect(m_btnSave, &QPushButton::clicked, this, &CacheLocationDialog::onSaveAndAccept);

    btnLayout->addWidget(m_btnRestore);
    btnLayout->addStretch();
    btnLayout->addWidget(m_btnCancel);
    btnLayout->addWidget(m_btnSave);
    mainLayout->addLayout(btnLayout);
}

void CacheLocationDialog::updatePathDisplay() {
    QString currentCacheDir = AppConfig::instance().getGlobalCacheDir();
    m_pathEdit->setText(currentCacheDir);

    qint64 totalBytes = calculateDirectorySize(currentCacheDir);
    double mbSize = totalBytes / (1024.0 * 1024.0);
    m_sizeLabel->setText(QString("当前缓存占用: %1 MB").arg(mbSize, 0, 'f', 2));
}

qint64 CacheLocationDialog::calculateDirectorySize(const QString& path) {
    qint64 size = 0;
    QDirIterator it(path, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        size += it.fileInfo().size();
    }
    return size;
}

void CacheLocationDialog::onBrowsePath() {
    QString selectedDir = QFileDialog::getExistingDirectory(this, "选择缓存存储根目录", m_pathEdit->text());
    if (!selectedDir.isEmpty()) {
        m_pathEdit->setText(selectedDir);
    }
}

void CacheLocationDialog::onRestoreDefault() {
    AppConfig::instance().remove("Cache/CustomLocation");
    AppConfig::instance().sync();
    updatePathDisplay();
}

void CacheLocationDialog::onSaveAndAccept() {
    QString oldDir = AppConfig::instance().getGlobalCacheDir();
    QString newDir = m_pathEdit->text().trimmed();
    QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    if (m_chkMigrateData && m_chkMigrateData->isChecked() && !oldDir.isEmpty() && !newDir.isEmpty() && QDir(oldDir).canonicalPath() != QDir(newDir).canonicalPath()) {
        QDir().mkpath(newDir);
        QDirIterator it(oldDir, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            QString relativePath = QDir(oldDir).relativeFilePath(it.filePath());
            QString destPath = QDir(newDir).filePath(relativePath);
            QDir().mkpath(QFileInfo(destPath).absolutePath());
            if (!QFile::exists(destPath)) {
                QFile::copy(it.filePath(), destPath);
            }
        }
    }

    if (newDir != defaultDir && !newDir.isEmpty()) {
        AppConfig::instance().setValue("Cache/CustomLocation", newDir);
    } else {
        AppConfig::instance().remove("Cache/CustomLocation");
    }
    AppConfig::instance().sync();
    accept();
}

} // namespace QuarkMeta
