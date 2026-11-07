#pragma once

#include "../core/ItemRecord.h"
#include "../core/CoreEngine.h"
#include <QObject>
#include <QStringList>
#include <QThreadPool>
#include <memory>
#include <mutex>
#include <vector>

namespace QuarkMeta {

class ContentPanel;

class LibraryValidationService : public QObject {
    Q_OBJECT

public:
    static LibraryValidationService& instance();

    // 启动对当前库显示记录的后台异步校验
    void validateRecordsAsync(const std::vector<ItemRecord>& records, ContentPanel* panel, int reqId);

    // 立即取消当前正在执行的校验任务
    void cancelCurrentValidation();

private:
    explicit LibraryValidationService(QObject* parent = nullptr);
    ~LibraryValidationService() override;

    QThreadPool m_valPool;
    std::shared_ptr<CancellationToken> m_currentToken;
    std::mutex m_tokenMutex;
};

} // namespace QuarkMeta
