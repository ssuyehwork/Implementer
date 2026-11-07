#include "MetaDbRepository.h"
#include "DatabaseManager.h"

namespace QuarkMeta {

MetaDbRepository& MetaDbRepository::instance() {
    static MetaDbRepository inst;
    return inst;
}

void MetaDbRepository::initDatabase() {
    DatabaseManager::instance().init();
}

DriveMetaRecord MetaDbRepository::getDriveMeta(const std::wstring& drivePath) {
    return DriveMetaDao::getDriveMeta(drivePath);
}

void MetaDbRepository::saveDriveMeta(const DriveMetaRecord& record) {
    DriveMetaDao::saveDriveMeta(record);
}

} // namespace QuarkMeta
