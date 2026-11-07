#pragma once

#include "FramelessDialog.h"
#include <QCheckBox>
#include <QPushButton>

namespace QuarkMeta {

enum class CollisionResolveAction {
    AutoResolve, // 自动解析
    Replace,     // 替换
    Skip,        // 跳过
    Cancel       // 取消
};

class FileCollisionDialog : public FramelessDialog {
    Q_OBJECT
public:
    explicit FileCollisionDialog(const QString& firstFileName,
                                 const QString& targetDirName,
                                 int totalCount,
                                 QWidget* parent = nullptr);

    CollisionResolveAction selectedAction() const;
    bool applyToAll() const;

private:
    void setupUi(const QString& firstFileName, const QString& targetDirName, int totalCount);

    CollisionResolveAction m_selectedAction = CollisionResolveAction::Cancel;
    QCheckBox* m_chkApplyToAll = nullptr;
};

} // namespace QuarkMeta
