#include "dialogs/TextExtensionDialog.h"
#include "../UiHelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>

namespace QuarkMeta {

TextExtensionDialog::TextExtensionDialog(QWidget* parent)
    : FramelessDialog("文本扩展名设置", parent) {
    setVisibleButtons(Close);
    resize(560, 480);
    setMinimumSize(480, 400);

    setupUi();
    loadCustomExtensions();
}

void TextExtensionDialog::setupUi() {
    auto* layout = new QVBoxLayout(m_contentArea);
    layout->setContentsMargins(20, 15, 20, 20);
    layout->setSpacing(12);

    // 1. 内置格式区 (只读标签胶囊)
    auto* lblBuiltInTitle = new QLabel("内置支持的常见文件格式：");
    lblBuiltInTitle->setObjectName("TextExtBuiltInTitle");
    layout->addWidget(lblBuiltInTitle);

    auto* scrollArea = new QScrollArea();
    scrollArea->setObjectName("TextExtScrollArea");
    scrollArea->setFixedHeight(85);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto* scrollContent = new QWidget();
    scrollContent->setObjectName("TextExtScrollContent");
    auto* flowLayout = new QHBoxLayout(scrollContent);
    flowLayout->setContentsMargins(8, 8, 8, 8);
    flowLayout->setSpacing(6);

    // 显示常见内置后缀列表
    QStringList builtInList = UiHelper::getBuiltInTextExtensions();
    auto* builtInContainer = new QWidget();
    builtInContainer->setObjectName("TextExtBuiltInContainer");
    auto* builtInLayout = new QVBoxLayout(builtInContainer);
    builtInLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* lblBuiltInCapsules = new QLabel(builtInList.join("  •  "));
    lblBuiltInCapsules->setObjectName("TextExtBuiltInCapsules");
    lblBuiltInCapsules->setWordWrap(true);
    builtInLayout->addWidget(lblBuiltInCapsules);

    scrollArea->setWidget(builtInContainer);
    layout->addWidget(scrollArea);

    // 2. 自定义格式输入区
    auto* lblCustomTitle = new QLabel("自定义纯文本扩展名：");
    lblCustomTitle->setObjectName("TextExtCustomTitle");
    layout->addWidget(lblCustomTitle);

    auto* inputLayout = new QHBoxLayout();
    m_inputEdit = new QLineEdit();
    m_inputEdit->setPlaceholderText("输入扩展名（如 mydata 或 .frag）");
    m_inputEdit->setMinimumHeight(34);
    m_inputEdit->setObjectName("FramelessInputEdit");
    connect(m_inputEdit, &QLineEdit::returnPressed, this, &TextExtensionDialog::onAddExtension);
    inputLayout->addWidget(m_inputEdit);

    m_btnAdd = new QPushButton("添加");
    m_btnAdd->setFixedSize(70, 34);
    m_btnAdd->setCursor(Qt::PointingHandCursor);
    m_btnAdd->setObjectName("FramelessBtnOk");
    connect(m_btnAdd, &QPushButton::clicked, this, &TextExtensionDialog::onAddExtension);
    inputLayout->addWidget(m_btnAdd);

    layout->addLayout(inputLayout);

    // 3. 自定义格式列表与删除
    auto* listLayout = new QHBoxLayout();
    m_customListWidget = new QListWidget();
    m_customListWidget->setObjectName("TextExtensionListWidget");
    listLayout->addWidget(m_customListWidget);

    auto* listBtnLayout = new QVBoxLayout();
    m_btnDelete = new QPushButton("删除");
    m_btnDelete->setFixedSize(75, 32);
    m_btnDelete->setCursor(Qt::PointingHandCursor);
    m_btnDelete->setObjectName("FramelessBtnCancel");
    connect(m_btnDelete, &QPushButton::clicked, this, &TextExtensionDialog::onDeleteExtension);
    listBtnLayout->addWidget(m_btnDelete);

    m_btnRestore = new QPushButton("恢复默认");
    m_btnRestore->setFixedSize(75, 32);
    m_btnRestore->setCursor(Qt::PointingHandCursor);
    m_btnRestore->setObjectName("FramelessBtnCancel");
    connect(m_btnRestore, &QPushButton::clicked, this, &TextExtensionDialog::onRestoreDefault);
    listBtnLayout->addWidget(m_btnRestore);

    listBtnLayout->addStretch();
    listLayout->addLayout(listBtnLayout);

    layout->addLayout(listLayout);

    // 4. 底部按钮组
    auto* bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();

    m_btnCancel = new QPushButton("取消");
    m_btnCancel->setFixedSize(80, 32);
    m_btnCancel->setCursor(Qt::PointingHandCursor);
    m_btnCancel->setObjectName("FramelessBtnCancel");
    connect(m_btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    bottomLayout->addWidget(m_btnCancel);

    m_btnSave = new QPushButton("保存");
    m_btnSave->setFixedSize(80, 32);
    m_btnSave->setCursor(Qt::PointingHandCursor);
    m_btnSave->setObjectName("FramelessBtnOk");
    connect(m_btnSave, &QPushButton::clicked, this, &TextExtensionDialog::onSaveAndAccept);
    bottomLayout->addWidget(m_btnSave);

    layout->addLayout(bottomLayout);
}

void TextExtensionDialog::loadCustomExtensions() {
    m_customListWidget->clear();
    QStringList customList = UiHelper::getCustomTextExtensions();
    for (const QString& ext : customList) {
        m_customListWidget->addItem("." + ext);
    }
}

void TextExtensionDialog::onAddExtension() {
    QString ext = m_inputEdit->text().trimmed().toLower();
    if (ext.startsWith('.')) ext = ext.mid(1);
    if (ext.isEmpty()) return;

    QString formatted = "." + ext;
    // 查重
    for (int i = 0; i < m_customListWidget->count(); ++i) {
        if (m_customListWidget->item(i)->text().toLower() == formatted) {
            m_inputEdit->clear();
            return;
        }
    }

    m_customListWidget->addItem(formatted);
    m_inputEdit->clear();
}

void TextExtensionDialog::onDeleteExtension() {
    auto items = m_customListWidget->selectedItems();
    for (auto* item : items) {
        delete m_customListWidget->takeItem(m_customListWidget->row(item));
    }
}

void TextExtensionDialog::onRestoreDefault() {
    m_customListWidget->clear();
}

void TextExtensionDialog::onSaveAndAccept() {
    QStringList customExts;
    for (int i = 0; i < m_customListWidget->count(); ++i) {
        QString itemText = m_customListWidget->item(i)->text().trimmed();
        if (itemText.startsWith('.')) itemText = itemText.mid(1);
        if (!itemText.isEmpty()) {
            customExts << itemText;
        }
    }
    UiHelper::setCustomTextExtensions(customExts);
    accept();
}

} // namespace QuarkMeta
