#include "CentralEventHub.h"

namespace QuarkMeta {

CentralEventHub& CentralEventHub::instance() {
    static CentralEventHub s_instance;
    return s_instance;
}

CentralEventHub::CentralEventHub(QObject* parent)
    : QObject(parent) {
    qRegisterMetaType<QuarkMeta::AppEvent>("QuarkMeta::AppEvent");
}

void CentralEventHub::publishEvent(const AppEvent& event) {
    emit eventOccurred(event);
}

} // namespace QuarkMeta
