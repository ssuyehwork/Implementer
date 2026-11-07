#pragma once

#include "../FramelessDialog.h"
#include <QStringList>
#include <QPointer>
#include <QAbstractTableModel>
#include <QStyledItemDelegate>
#include <atomic>
#include <memory>
#include <vector>

class QTableView;
class QLabel;
class QPushButton;
class QSortFilterProxyModel;

namespace QuarkMeta {

class FramelessWindowHelper;

struct FileCompareItem {
    QString path;
    QString folderPath;
    QString name;
    QString hash;       // SHA-256（64位小写）或状态："计算中…"、"文件不存在"、"无法读取"
    qint64 size = -1;   // 原始字节数，-1 表示未读取或读取失败
    long long ctime = 0; // 毫秒
    long long mtime = 0; // 毫秒
    bool isLoaded = false;
    bool hasError = false;
};

class FileCompareModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Column {
        ColIndex = 0,
        ColName,
        ColHash,
        ColSize,
        ColCreatedDate,
        ColModifiedDate,
        ColumnCount
    };

    explicit FileCompareModel(const QStringList& filePaths, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void updateItemResult(int row, qint64 size, long long ctime, long long mtime, const QString& hash, bool hasError);
    const FileCompareItem& itemAt(int row) const { return m_items[row]; }

private:
    std::vector<FileCompareItem> m_items;
};

class HashCompareDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit HashCompareDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

class RowIndexDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit RowIndexDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        opt.text = QString::number(index.row() + 1);
        opt.displayAlignment = Qt::AlignCenter;
        QStyledItemDelegate::paint(painter, opt, index);
    }
};

class FileCompareDialog : public FramelessDialog {
    Q_OBJECT
public:
    explicit FileCompareDialog(const QStringList& filePaths, QWidget* parent = nullptr);
    ~FileCompareDialog() override;

protected:
    void closeEvent(QCloseEvent* event) override;
    void reject() override;
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void initUi();
    void startBackgroundHashCalculation();
    void cancelCalculation();

    QStringList m_filePaths;
    std::shared_ptr<std::atomic<bool>> m_cancelToken;
    int m_jobId = 0;
    int m_completedCount = 0;

    QTableView* m_tableView = nullptr;
    FileCompareModel* m_model = nullptr;
    QSortFilterProxyModel* m_proxyModel = nullptr;
    HashCompareDelegate* m_hashDelegate = nullptr;
    RowIndexDelegate* m_indexDelegate = nullptr;
    QLabel* m_progressLabel = nullptr;
    QPushButton* m_closeButton = nullptr;
    FramelessWindowHelper* m_framelessHelper = nullptr;
};

} // namespace QuarkMeta
