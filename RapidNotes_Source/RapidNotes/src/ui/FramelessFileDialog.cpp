#include "FramelessFileDialog.h"
#include "UiHelper.h"
#include <QStandardPaths>
#include <QStorageInfo>
#include <QDir>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QItemSelectionModel>
#include <QFile>

namespace QuarkMeta {

FramelessFileDialog::FramelessFileDialog(const QString& title, const QString& dir,
                                         FileMode mode, const QString& filter, QWidget* parent)
    : FramelessDialog(title, parent), m_mode(mode)
{
    resize(900, 600);

    auto* mainLayout = new QVBoxLayout(getContentArea());
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 1. 地址栏
    auto* topRow = new QWidget();
    topRow->setFixedHeight(40);
    topRow->setObjectName("FileDialogTopRow");
    auto* topLayout = new QHBoxLayout(topRow);
    topLayout->setContentsMargins(10, 0, 10, 0);

    m_pathEdit = new QLineEdit();
    m_pathEdit->setObjectName("FileDialogPathEdit");
    connect(m_pathEdit, &QLineEdit::returnPressed, this, &FramelessFileDialog::onPathEntered);
    topLayout->addWidget(m_pathEdit);

    mainLayout->addWidget(topRow);

    // 2. 中间主体 (侧边栏 + 文件列表)
    auto* middleWidget = new QWidget();
    auto* middleLayout = new QHBoxLayout(middleWidget);
    middleLayout->setContentsMargins(0, 0, 0, 0);
    middleLayout->setSpacing(0);

    m_sidebar = new QListWidget();
    m_sidebar->setFixedWidth(180);
    m_sidebar->setObjectName("FileDialogSidebar");
    setupSidebar();
    connect(m_sidebar, &QListWidget::itemClicked, this, &FramelessFileDialog::onSidebarClicked);
    middleLayout->addWidget(m_sidebar);

    m_listView = new QListView();
    m_listView->setViewMode(QListView::ListMode);
    m_listView->setObjectName("FileDialogListView");

    m_model = new QFileSystemModel(this);
    m_model->setReadOnly(true);
    m_model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    if (m_mode == Directory) m_model->setFilter(QDir::Dirs | QDir::Drives | QDir::NoDotAndDotDot);

    m_listView->setModel(m_model);
    m_model->setNameFilterDisables(false);
    connect(m_listView, &QListView::doubleClicked, this, &FramelessFileDialog::onFileViewDoubleClicked);
    connect(m_listView->selectionModel(), &QItemSelectionModel::currentChanged, this, &FramelessFileDialog::onSelectionChanged);

    middleLayout->addWidget(m_listView);
    mainLayout->addWidget(middleWidget, 1);

    // 3. 底部操作栏
    auto* bottomRow = new QWidget();
    bottomRow->setFixedHeight(80);
    bottomRow->setObjectName("FileDialogBottomRow");
    auto* bottomLayout = new QVBoxLayout(bottomRow);
    bottomLayout->setContentsMargins(15, 10, 15, 10);

    auto* fileRow = new QHBoxLayout();
    fileRow->addWidget(new QLabel("文件名:"));
    m_fileEdit = new QLineEdit();
    m_fileEdit->setObjectName("FileDialogFileEdit");
    fileRow->addWidget(m_fileEdit, 1);

    m_filterCombo = new QComboBox();
    m_filterCombo->setFixedWidth(200);
    m_filterCombo->setObjectName("FileDialogFilterCombo");

    QStringList filters;
    if (!filter.isEmpty()) {
        filters = filter.split(";;", Qt::SkipEmptyParts);
        m_filterCombo->addItems(filters);
    } else {
        m_filterCombo->addItem("所有文件 (*.*)");
    }

    auto updateModelFilter = [this](const QString& filterText) {
        if (m_mode == Directory) return;

        QRegularExpression re("\\((.*)\\)");
        QRegularExpressionMatch match = re.match(filterText);
        if (match.hasMatch()) {
            QString patterns = match.captured(1);
            m_model->setNameFilters(patterns.split(" ", Qt::SkipEmptyParts));
        } else {
            m_model->setNameFilters(QStringList("*"));
        }
    };

    connect(m_filterCombo, &QComboBox::currentTextChanged, this, updateModelFilter);
    updateModelFilter(m_filterCombo->currentText());

    fileRow->addWidget(m_filterCombo);
    bottomLayout->addLayout(fileRow);

    auto* btnRow = new QHBoxLayout();
    btnRow->addStretch();
    auto* cancelBtn = new QPushButton("取消");
    cancelBtn->setFixedSize(85, 28);
    cancelBtn->setObjectName("FileDialogCancelBtn");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnRow->addWidget(cancelBtn);

    m_okBtn = new QPushButton(m_mode == Directory ? "选择文件夹" : "打开");
    m_okBtn->setFixedSize(100, 28);
    m_okBtn->setObjectName("FileDialogOkBtn");
    connect(m_okBtn, &QPushButton::clicked, this, &FramelessFileDialog::onAccept);
    btnRow->addWidget(m_okBtn);
    bottomLayout->addLayout(btnRow);

    mainLayout->addWidget(bottomRow);

    // 初始路径
    QString initialDir = dir.isEmpty() ? QDir::currentPath() : dir;
    updatePath(initialDir);
}

void FramelessFileDialog::setupSidebar() {
    auto addSidebarItem = [this](const QString& text, const QString& iconName, const QString& path) {
        auto* item = new QListWidgetItem(UiHelper::getIcon(iconName, QColor("#BBB")), text);
        item->setData(Qt::UserRole, path);
        m_sidebar->addItem(item);
    };

    addSidebarItem("此电脑", "computer", "");
    addSidebarItem("桌面", "desktop", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    addSidebarItem("下载", "download", QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    addSidebarItem("文档", "document", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    addSidebarItem("图片", "image", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

    for (const QStorageInfo& storage : QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady()) {
            addSidebarItem(storage.displayName(), "drive", storage.rootPath());
        }
    }
}

void FramelessFileDialog::updatePath(const QString& path) {
    QDir d(path);
    if (d.exists()) {
        m_listView->setRootIndex(m_model->setRootPath(path));
        m_pathEdit->setText(QDir::toNativeSeparators(path));
    }
}

void FramelessFileDialog::onSidebarClicked(QListWidgetItem* item) {
    QString path = item->data(Qt::UserRole).toString();
    if (!path.isEmpty()) updatePath(path);
}

void FramelessFileDialog::onFileViewDoubleClicked(const QModelIndex& index) {
    if (m_model->isDir(index)) {
        updatePath(m_model->filePath(index));
    } else if (m_mode != Directory) {
        onAccept();
    }
}

void FramelessFileDialog::onSelectionChanged(const QModelIndex& current, const QModelIndex& /*previous*/) {
    if (current.isValid()) {
        m_fileEdit->setText(m_model->fileName(current));
    }
}

void FramelessFileDialog::onPathEntered() {
    updatePath(m_pathEdit->text());
}

void FramelessFileDialog::onAccept() {
    QString path = m_model->filePath(m_listView->currentIndex());
    if (path.isEmpty()) {
        path = QDir(m_model->rootPath()).absoluteFilePath(m_fileEdit->text());
    }

    if (m_mode == Directory) {
        if (QDir(path).exists()) {
            m_selectedPath = path;
            accept();
        }
    } else {
        if (QFile::exists(path)) {
            m_selectedPath = path;
            accept();
        }
    }
}

QString FramelessFileDialog::getExistingDirectory(QWidget* parent, const QString& caption, const QString& dir) {
    FramelessFileDialog dlg(caption, dir, Directory, "", parent);
    if (dlg.exec() == QDialog::Accepted) return dlg.selectedPath();
    return "";
}

QString FramelessFileDialog::getOpenFileName(QWidget* parent, const QString& caption, const QString& dir, const QString& filter) {
    FramelessFileDialog dlg(caption, dir, ExistingFile, filter, parent);
    if (dlg.exec() == QDialog::Accepted) return dlg.selectedPath();
    return "";
}

QString FramelessFileDialog::getSaveFileName(QWidget* parent, const QString& caption, const QString& dir, const QString& filter) {
    FramelessFileDialog dlg(caption, dir, AnyFile, filter, parent);
    if (dlg.exec() == QDialog::Accepted) return dlg.selectedPath();
    return "";
}

} // namespace QuarkMeta
