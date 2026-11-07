#pragma once

#include <QObject>
#include <QString>
#include <string>

namespace QuarkMeta {

class TrashRepository : public QObject {
    Q_OBJECT
public:
    static TrashRepository& instance();

    // 根据原始路径查询磁盘回收站记录 (id 和 trash_path)
    bool getDiskTrashRecordByPath(const std::wstring& originalPath, int& outId, QString& outTrashPath) const;

private:
    explicit TrashRepository(QObject* parent = nullptr);
};

} // namespace QuarkMeta
