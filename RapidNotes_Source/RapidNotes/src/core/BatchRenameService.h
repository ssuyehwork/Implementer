#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <vector>
#include <string>
#include <functional>
#include <QWidget>

namespace QuarkMeta {

enum class RenameComponentType {
    Text,           // 固定文本
    Sequence,       // 序列数字
    Date,           // 日期
    OriginalName,   // 原始文件名
    Metadata,       // 元数据标记
    ParentFolderName // 父文件夹名称
};

enum class DiskOperationMode {
    Rename,
    Move,
    Copy
};

struct RenameRule {
    RenameComponentType type = RenameComponentType::Text;
    QString value;      // 文本值、日期格式等
    int start = 1;      // 序列起始
    int step = 1;       // 序列步长
    int padding = 3;    // 补零位数
};

class BatchRenameService : public QObject {
    Q_OBJECT

public:
    static BatchRenameService& instance();

    // 1. Fast memory-based preview calculation
    std::vector<std::wstring> computePreview(const std::vector<std::wstring>& originalPaths,
                                            const std::vector<RenameRule>& rules);

    // 2. Async execution pipeline (UUID two-phase rename, metadata/thumbnail roaming, atomic undo)
    void executeAsync(const std::vector<std::wstring>& originalPaths,
                      const std::vector<std::wstring>& newNames,
                      DiskOperationMode mode,
                      const QString& targetDir,
                      QWidget* parentWidget = nullptr,
                      std::function<void(int successCount)> callback = nullptr);

private:
    explicit BatchRenameService(QObject* parent = nullptr);
    ~BatchRenameService() override = default;
    BatchRenameService(const BatchRenameService&) = delete;
    BatchRenameService& operator=(const BatchRenameService&) = delete;

    QString processOne(const QString& originalPath, int index, const std::vector<RenameRule>& rules);
};

} // namespace QuarkMeta
