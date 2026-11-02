#pragma once

#include <QObject>
#include <QStringList>
#include <vector>
#include "../../core/ItemRecord.h"

namespace QuarkMeta {

class ContentPanel;

class ContentDataLoader : public QObject {
    Q_OBJECT
public:
    explicit ContentDataLoader(ContentPanel* panel);
    ~ContentDataLoader() override = default;

    void loadDirectory(const QString& path, bool recursive);
    void loadCategory(const QString& categoryType);
    void loadPaths(const QStringList& paths, int reqId = 0);
    void loadLibraryPaths(const QStringList& paths, int reqId = 0);
    void appendPaths(const QStringList& paths, int reqId = 0);
    void loadSmartFolder(const struct SmartFolderRecord& record, const QString& displayName);

    int currentRequestId() const { return m_loadRequestId; }

private:
    ContentPanel* m_panel = nullptr;
    int m_loadRequestId = 0;
};

} // namespace QuarkMeta
