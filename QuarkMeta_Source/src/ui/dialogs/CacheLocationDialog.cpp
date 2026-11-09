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

    // QuickLook 预览图缓存源选择（严格互斥二选一单选源）
    m_chkDiskThumbs = new QCheckBox("优先使用现成缩略图缓存 (disk_thumbs)", this);
    m_chkQuickLookPreviews = new QCheckBox("优先使用高质量大图预览缓存 (quicklook_previews)", this);

    QString currentSource = AppConfig::instance().getValue("QuickLook/ThumbnailSource", "quicklook_previews").toString();
    if (currentSource == "disk_thumbs") {
        m_chkDiskThumbs->setChecked(true);
        m_chkQuickLookPreviews->setChecked(false);
    } else {
        m_chkDiskThumbs->setChecked(false);
        m_chkQuickLookPreviews->setChecked(true);
    }

    // 刚性互斥二选一逻辑：勾选一个自动取消另一个，且禁止同时取消
    connect(m_chkDiskThumbs, &QCheckBox::clicked, this, [this](bool checked) {
        if (!checked) {
            m_chkDiskThumbs->setChecked(true); // 保证二选一必选其一
        } else {
            m_chkQuickLookPreviews->setChecked(false);
        }
    });

    connect(m_chkQuickLookPreviews, &QCheckBox::clicked, this, [this](bool checked) {
        if (!checked) {
            m_chkQuickLookPreviews->setChecked(true); // 保证二选一必选其一
        } else {
            m_chkDiskThumbs->setChecked(false);
        }
    });

    mainLayout->addWidget(m_chkDiskThumbs);
    mainLayout->addWidget(m_chkQuickLookPreviews);

    // 复选框下方清晰说明注释，供用户参考思量
    m_sourceDescLabel = new QLabel(
        "提示：\n"
        "• disk_thumbs：日常浏览现成生成，载入速度极快、0ms秒开无需二次解码，但清晰度较低；\n"
        "• quicklook_previews：生成最高 2048px 超高清画质，细节极其锐利，但首次载入需要深度解码耗时。",
        this
    );
    m_sourceDescLabel->setWordWrap(true);
    m_sourceDescLabel->setStyleSheet("color: #888888; font-size: 11px; line-height: 1.4;");
    mainLayout->addWidget(m_sourceDescLabel);

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
        QString cleanSelected = QDir::toNativeSeparators(QDir::cleanPath(selectedDir));
        if (!cleanSelected.endsWith("cache", Qt::CaseInsensitive)) {
            cleanSelected = QDir(cleanSelected).filePath("cache");
        }
        m_pathEdit->setText(QDir::toNativeSeparators(cleanSelected));
    }
}

void CacheLocationDialog::onRestoreDefault() {
    AppConfig::instance().remove("Cache/CustomLocation");
    AppConfig::instance().remove("QuickLook/ThumbnailSource");
    AppConfig::instance().sync();
    updatePathDisplay();
    if (m_chkDiskThumbs && m_chkQuickLookPreviews) {
        m_chkDiskThumbs->setChecked(false);
        m_chkQuickLookPreviews->setChecked(true); // 默认高清大图模式
    }
}

void CacheLocationDialog::onSaveAndAccept() {
    // 1. 持久化 QuickLook 缓存源选择 (二选一互斥)
    if (m_chkDiskThumbs && m_chkDiskThumbs->isChecked()) {
        AppConfig::instance().setValue("QuickLook/ThumbnailSource", "disk_thumbs");
    } else {
        AppConfig::instance().setValue("QuickLook/ThumbnailSource", "quicklook_previews");
    }

    // 2. 缓存根目录整体真搬迁（迁移后彻底清除原路径数据）
    QString oldDir = AppConfig::instance().getGlobalCacheDir();
    QString targetDisplayDir = m_pathEdit->text().trimmed();
    QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    // 标准化去除末尾 /cache，以获取 AppConfig 期望的父级根目录
    QString customRootToStore = targetDisplayDir;
    if (customRootToStore.endsWith("/cache", Qt::CaseInsensitive) || customRootToStore.endsWith("\\cache", Qt::CaseInsensitive)) {
        customRootToStore.chop(6);
    }

    QString newDir = targetDisplayDir;
    if (!newDir.endsWith("cache", Qt::CaseInsensitive)) {
        newDir = QDir(newDir).filePath("cache");
    }

    if (m_chkMigrateData && m_chkMigrateData->isChecked() && !oldDir.isEmpty() && !newDir.isEmpty() && QDir(oldDir).canonicalPath() != QDir(newDir).canonicalPath()) {
        QDir().mkpath(newDir);
        // A. 逐项复制并校验
        QDirIterator it(oldDir, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
        QStringList copiedOldFiles;
        while (it.hasNext()) {
            it.next();
            QString srcFilePath = it.filePath();
            QString relativePath = QDir(oldDir).relativeFilePath(srcFilePath);
            QString destPath = QDir(newDir).filePath(relativePath);
            QDir().mkpath(QFileInfo(destPath).absolutePath());

            bool copyOk = false;
            if (QFile::exists(destPath)) {
                QFile::remove(destPath);
            }
            if (QFile::copy(srcFilePath, destPath)) {
                copyOk = true;
            }
            if (copyOk) {
                copiedOldFiles.append(srcFilePath);
            }
        }

        // B. 搬家清理：迁移成功的文件从原路径彻底删除，杜绝伪迁移空间残留
        for (const QString& oldFile : copiedOldFiles) {
            QFile::remove(oldFile);
        }

        // C. 递归清理旧 cache 目录内的空子目录
        QDir(oldDir).removeRecursively();
    }

    if (newDir != defaultDir && !customRootToStore.isEmpty()) {
        AppConfig::instance().setValue("Cache/CustomLocation", customRootToStore);
    } else {
        AppConfig::instance().remove("Cache/CustomLocation");
    }
    AppConfig::instance().sync();
    accept();
}

} // namespace QuarkMeta
