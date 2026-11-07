#pragma once
#include <string>

namespace QuarkMeta {

class VolumePathResolver {
public:
    static std::wstring getVolumeSerialNumber(const std::wstring& path);
};

} // namespace QuarkMeta
