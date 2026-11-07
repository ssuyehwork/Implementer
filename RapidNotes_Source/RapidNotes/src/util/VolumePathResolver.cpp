#include "VolumePathResolver.h"
#include <cwctype>
#include <unordered_map>
#include <mutex>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace QuarkMeta {

std::wstring VolumePathResolver::getVolumeSerialNumber(const std::wstring& path) {
    if (path.length() < 2 || path[1] != L':') return L"UNKNOWN";

    wchar_t driveLetter = static_cast<wchar_t>(towupper(path[0]));

    static std::unordered_map<wchar_t, std::wstring> s_cache;
    static std::mutex s_mutex;
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        auto it = s_cache.find(driveLetter);
        if (it != s_cache.end()) {
            return it->second;
        }
    }

#ifdef Q_OS_WIN
    wchar_t root[4] = { driveLetter, L':', L'\\', L'\0' };
    wchar_t volumeName[MAX_PATH + 1] = { 0 };
    DWORD serialNumber = 0;
    if (GetVolumeInformationW(root, volumeName, MAX_PATH, &serialNumber, nullptr, nullptr, nullptr, 0)) {
        wchar_t buf[64];
        swprintf_s(buf, 64, L"%08X", serialNumber);
        std::wstring res(buf);
        std::lock_guard<std::mutex> lock(s_mutex);
        s_cache[driveLetter] = res;
        return res;
    }
#endif

    std::wstring unknown = L"UNKNOWN";
    std::lock_guard<std::mutex> lock(s_mutex);
    s_cache[driveLetter] = unknown;
    return unknown;
}

} // namespace QuarkMeta
