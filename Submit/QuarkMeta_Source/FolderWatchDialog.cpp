#include "dialogs/FolderWatchDialog.h"
#include "../UiHelper.h"
#include "../../core/AppConfig.h"
#include "../../core/DirectoryWatchService.h"
#include "../../core/TaskProgressService.h"
#include "../../meta/LibrarySyncService.h"
#include "../../meta/LibraryService.h"
#include "../../core/DwsTrace.h"
#include "../Logger.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QMimeData>
#include <QFileInfo>
#include <QDirIterator>
#include <QCoreApplication>
#include <QtConcurrent>

namespace QuarkMeta {

FolderWatchRow::FolderWatchRow(const QString& path, QWidget* parent)
    : QWidget(parent) {
    setAcceptDrops(true);
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 2, 0, 2);
    layout->setSpacing(6);

    m_pathEdit = new QLineEdit(path, this);
    m_pathEdit->setPlaceholderText("可填入或拖入文件夹路径...");
    m_pathEdit->setObjectName("FramelessInputEdit");
    m_pathEdit->setMinimumHeight(30);
    layout->addWidget(m_pathEdit, 1);

    m_btnRemove = new QPushButton("-", this);
    m_btnRemove->setObjectName("RuleDeleteBtn");
    m_btnRemove->setFixedSize(28, 28);
    m_btnRemove->setCursor(Qt::PointingHandCursor);
    m_btnRemove->setFocusPolicy(Qt::NoFocus);
    connect(m_btnRemove, &QPushButton::clicked, this, &FolderWatchRow::removeRequested);
    layout->addWidget(m_btnRemove);

    m_btnAdd = new QPushButton("+", this);
    m_btnAdd->setObjectName("RuleDeleteBtn");
    m_btnAdd->setFixedSize(28, 28);
    m_btnAdd->setCursor(Qt::PointingHandCursor);
    m_btnAdd->setFocusPolicy(Qt::NoFocus);
    connect(m_btnAdd, &QPushButton::clicked, this, &FolderWatchRow::addRequested);
    layout->addWidget(m_btnAdd);
}

QString FolderWatchRow::path() const {
    return m_pathEdit ? m_pathEdit->text().trimmed() : QString();
}

void FolderWatchRow::setPath(const QString& path) {
    if (m_pathEdit) m_pathEdit->setText(path);
}

void FolderWatchRow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void FolderWatchRow::dropEvent(QDropEvent* event) {
    const auto urls = event->mimeData()->urls();
    for (const auto& url : urls) {
        QString localPath = url.toLocalFile();
        if (QFileInfo(localPath).isDir()) {
            setPath(localPath);
            event->acceptProposedAction();
            break;
        }
    }
}

FolderWatchDialog::FolderWatchDialog(QWidget* parent)
    : FramelessDialog("目录监听设置", parent) {
    setVisibleButtons(Close);
    resize(560, 420);
    setMinimumSize(480, 360);
    setAcceptDrops(true);

    setupUi();
    loadWatchedFolders();
}

void FolderWatchDialog::setupUi() {
    auto* layout = new QVBoxLayout(m_contentArea);
    layout->setContentsMargins(20, 15, 20, 20);
    layout->setSpacing(12);

    auto* lblDesc = new QLabel("设置需要后台高并发监控的文件夹列表（支持拖入文件夹）：", this);
    lblDesc->setObjectName("FolderWatchDescLabel");
    layout->addWidget(lblDesc);

    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setObjectName("FolderWatchScrollArea");

    auto* container = new QWidget(scrollArea);
    container->setObjectName("FolderWatchContainer");
    m_rowsLayout = new QVBoxLayout(container);
    m_rowsLayout->setContentsMargins(0, 0, 0, 0);
    m_rowsLayout->setSpacing(6);
    m_rowsLayout->addStretch();

    scrollArea->setWidget(container);
    layout->addWidget(scrollArea, 1);

    auto* bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();

    m_btnCancel = new QPushButton("取消", this);
    m_btnCancel->setFixedSize(80, 32);
    m_btnCancel->setCursor(Qt::PointingHandCursor);
    m_btnCancel->setObjectName("FramelessBtnCancel");
    m_btnCancel->setFocusPolicy(Qt::NoFocus);
    connect(m_btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    bottomLayout->addWidget(m_btnCancel);

    m_btnComplete = new QPushButton("完成", this);
    m_btnComplete->setFixedSize(80, 32);
    m_btnComplete->setCursor(Qt::PointingHandCursor);
    m_btnComplete->setObjectName("FramelessBtnOk");
    m_btnComplete->setFocusPolicy(Qt::NoFocus);
    m_btnComplete->setDefault(true);
    connect(m_btnComplete, &QPushButton::clicked, this, &FolderWatchDialog::onComplete);
    bottomLayout->addWidget(m_btnComplete);

    layout->addLayout(bottomLayout);
}

void FolderWatchDialog::loadWatchedFolders() {
    QStringList folders = AppConfig::instance().getValue("DirectoryWatch/WatchedFolders", QStringList{}).toStringList();
    if (folders.isEmpty()) {
        onAddRow();
    } else {
        for (const QString& f : folders) {
            onAddRow(f);
        }
    }
}

void FolderWatchDialog::onAddRow(const QString& path) {
    auto* row = new FolderWatchRow(path, this);
    connect(row, &FolderWatchRow::addRequested, this, [this]() {
        onAddRow();
    });
    connect(row, &FolderWatchRow::removeRequested, this, [this, row]() {
        onRemoveRow(row);
    });

    m_rows.append(row);
    m_rowsLayout->insertWidget(m_rowsLayout->count() - 1, row);
}

void FolderWatchDialog::onRemoveRow(FolderWatchRow* row) {
    if (m_rows.size() <= 1) {
        row->setPath("");
        return;
    }
    m_rows.removeOne(row);
    m_rowsLayout->removeWidget(row);
    row->deleteLater();
}

void FolderWatchDialog::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void FolderWatchDialog::dropEvent(QDropEvent* event) {
    const auto urls = event->mimeData()->urls();
    for (const auto& url : urls) {
        QString localPath = url.toLocalFile();
        if (QFileInfo(localPath).isDir()) {
            bool foundEmpty = false;
            for (auto* row : m_rows) {
                if (row->path().isEmpty()) {
                    row->setPath(localPath);
                    foundEmpty = true;
                    break;
                }
            }
            if (!foundEmpty) {
                onAddRow(localPath);
            }
        }
    }
    event->acceptProposedAction();
}

void FolderWatchDialog::onComplete() {
    QStringList validFolders;
    for (auto* row : m_rows) {
        QString p = row->path();
        if (!p.isEmpty() && QFileInfo(p).isDir() && !validFolders.contains(p)) {
            validFolders.append(p);
        }
    }

    QStringList oldFolders = AppConfig::instance().getValue("DirectoryWatch/WatchedFolders", QStringList{}).toStringList();
    AppConfig::instance().setValue("DirectoryWatch/WatchedFolders", validFolders);
    AppConfig::instance().sync();

    // 1. 同步后台 IOCP 监控状态
    for (const QString& oldF : oldFolders) {
        if (!validFolders.contains(oldF)) {
            QUARKMETA_DWS_TRACE("FolderWatchDialog", "onComplete",
                QString("调用 DirectoryWatchService::unwatch; path=%1, includeSubtree=true").arg(oldF));
            DirectoryWatchService::instance().unwatch(oldF, true);
        }
    }
    for (const QString& newF : validFolders) {
        QUARKMETA_DWS_TRACE("FolderWatchDialog", "onComplete",
            QString("调用 DirectoryWatchService::watch; path=%1, includeSubtree=true").arg(newF));
        DirectoryWatchService::instance().watch(newF, true);
    }

    // 2. 调用进度条执行全量子文件/子文件夹元数据入库
    if (!validFolders.isEmpty()) {
        m_btnComplete->setEnabled(false);
        m_btnCancel->setEnabled(false);

        (void)QtConcurrent::run([validFolders]() {
            QStringList allPathsToSync;
            for (const QString& folder : validFolders) {
                allPathsToSync.append(folder);
                QDirIterator it(folder, QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden, QDirIterator::Subdirectories);
                while (it.hasNext()) {
                    it.next();
                    allPathsToSync.append(it.filePath());
                }
            }

            int total = allPathsToSync.size();
            int jobId = 0;
            QMetaObject::invokeMethod(QCoreApplication::instance(), [total, &jobId]() {
                jobId = TaskProgressService::instance().createJob("正在同步监听目录数据与元数据...", total);
            }, Qt::BlockingQueuedConnection);

            const int batchSize = 100;
            for (int i = 0; i < total; i += batchSize) {
                QStringList batch = allPathsToSync.mid(i, batchSize);
                LibrarySyncService::instance().syncPaths(batch);
                int processed = qMin(i + batchSize, total);
                QMetaObject::invokeMethod(QCoreApplication::instance(), [jobId, processed, total]() {
                    TaskProgressService::instance().updateProgress(jobId, processed, total, QString("已录入 %1 / %2 项").arg(processed).arg(total));
                }, Qt::QueuedConnection);
            }

            // 自动将监听目录结构同步至分类树并追加绑定
            for (const QString& folder : validFolders) {
                LibraryService::instance().syncWatchedFolderHierarchy(folder);
            }

            QMetaObject::invokeMethod(QCoreApplication::instance(), [jobId]() {
                TaskProgressService::instance().finishJob(jobId);
            }, Qt::QueuedConnection);
        });
    }

    accept();
}

} // namespace QuarkMeta
