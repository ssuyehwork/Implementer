#pragma once
#include <QString>
#include <QStringList>
#include <string>

namespace RapidNotes {

struct ItemMeta {
    int rating = 0;
    std::wstring manualColor;
    QStringList tags;
    std::wstring note;
    std::wstring url;
};

class MetadataManager {
public:
    enum class RefreshLevel { FullRebuild };
    static MetadataManager& instance() {
        static MetadataManager s_inst;
        return s_inst;
    }
    ItemMeta getMeta(const std::wstring& path) { Q_UNUSED(path); return ItemMeta(); }
    void notifyUI(RefreshLevel level) { Q_UNUSED(level); }
    static std::wstring getVolumeSerialNumber(const std::wstring& path) { Q_UNUSED(path); return L""; }
    void renameItem(const std::wstring& oldPath, const std::wstring& newPath) { Q_UNUSED(oldPath); Q_UNUSED(newPath); }
};
}
