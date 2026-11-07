#include "OperationSnapshotEngine.h"
#include "../meta/MetadataManager.h"
#include "../ui/UndoToastOverlay.h"
#include "UndoManager.h"
#include "ActionCommand.h"
#include <QFileInfo>

namespace QuarkMeta {

OperationSnapshotEngine& OperationSnapshotEngine::instance() {
    static OperationSnapshotEngine inst;
    return inst;
}

AssetItemSnapshot OperationSnapshotEngine::captureSingle(const QString& path) {
    AssetItemSnapshot snap;
    snap.path = path;
    snap.fileName = QFileInfo(path).fileName();

    std::wstring wpath = path.toStdWString();
    // 读取收藏/置顶与元数据属性
    auto meta = MetadataManager::instance().getMeta(wpath);
    snap.isPinned = meta.pinned;
    snap.rating = meta.rating;
    snap.color = QString::fromStdWString(meta.manualColor);
    snap.tags = meta.tags;
    snap.note = QString::fromStdWString(meta.note);
    return snap;
}

QVector<AssetItemSnapshot> OperationSnapshotEngine::captureBatch(const QStringList& paths) {
    QVector<AssetItemSnapshot> list;
    list.reserve(paths.size());
    for (const auto& p : paths) {
        list.append(captureSingle(p));
    }
    return list;
}

bool OperationSnapshotEngine::executeWithSnapshot(
    QWidget* parentWidget,
    SnapshotOperationType opType,
    const QStringList& targetPaths,
    const QString& successToastMsg,
    std::function<bool()> doAction,
    std::function<bool(const QVector<AssetItemSnapshot>& beforeState)> undoAction)
{
    Q_UNUSED(opType);
    if (!doAction) return false;

    // 1. 操作前：自动捕获受影响资产的状态快照 (Before State)
    QVector<AssetItemSnapshot> beforeState = captureBatch(targetPaths);

    // 2. 执行主体写操作
    bool ok = doAction();
    if (!ok) return false;

    // 3. 操作成功：如果外部传入了专用的 undoAction，构建双向对称 Command
    if (undoAction) {
        class GeneralSnapshotUndoCommand : public ActionCommand {
        public:
            GeneralSnapshotUndoCommand(QVector<AssetItemSnapshot> before,
                                       std::function<bool()> doFunc,
                                       std::function<bool(const QVector<AssetItemSnapshot>& beforeState)> undoFunc)
                : m_before(std::move(before)),
                  m_doFunc(std::move(doFunc)),
                  m_undoFunc(std::move(undoFunc)) {}

            void execute() override {
                // 🚀【Redo 核心闭环】：重新执行正向操作
                if (m_doFunc) {
                    m_doFunc();
                }
            }

            void undo() override {
                // 🚀【Undo 核心闭环】：基于快照执行逆向回滚
                if (m_undoFunc) {
                    m_undoFunc(m_before);
                }
            }

            void redo() override {
                execute();
            }

            QString description() const override {
                return "快照事务撤销/重做";
            }

            bool affectsPath(const QString& path) const override {
                for (const auto& snap : m_before) {
                    if (snap.path == path) return true;
                }
                return false;
            }

        private:
            QVector<AssetItemSnapshot> m_before;
            std::function<bool()> m_doFunc;
            std::function<bool(const QVector<AssetItemSnapshot>& beforeState)> m_undoFunc;
        };

        // 压入全局撤销栈，实现 Ctrl+Z 与 Ctrl+Y 的 100% 严格时序双向可逆
        UndoManager::instance().pushCommand(
            std::make_unique<GeneralSnapshotUndoCommand>(beforeState, doAction, undoAction)
        );

        // 🚀【规范 7000ms 与点击显式绑定】：弹出 7 秒气泡，点击右侧“撤销”按钮直接调用 UndoManager::undo()
        UndoToastOverlay::instance()->showToast(
            parentWidget,
            successToastMsg,
            []() {
                UndoManager::instance().undo();
            },
            7000 // 👈 统一 7 秒停留时长红线
        );
    }

    return true;
}

} // namespace QuarkMeta
