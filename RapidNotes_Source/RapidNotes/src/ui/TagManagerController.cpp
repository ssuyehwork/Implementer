#include "TagManagerController.h"
#include <QtConcurrent>
#include "../core/TagLexiconService.h"

namespace QuarkMeta {

TagManagerController::TagManagerController(QObject* parent) : QObject(parent) {}

void TagManagerController::addTagToGroupAsync(const QString& tagName, int groupId) {
    (void)QtConcurrent::run([this, tagName, groupId]() {
        if (TagLexiconService::instance().addTagToGroup(tagName, groupId)) {
            emit tagGroupStateChanged(); // 成功后发射刷新信号
        }
    });
}

void TagManagerController::renameGroupAsync(int groupId, const QString& newName) {
    (void)QtConcurrent::run([this, groupId, newName]() {
        if (TagLexiconService::instance().renameGroup(groupId, newName)) {
            emit tagGroupStateChanged();
        }
    });
}

void TagManagerController::deleteGroupAsync(int groupId) {
    (void)QtConcurrent::run([this, groupId]() {
        if (TagLexiconService::instance().deleteGroup(groupId)) {
            emit tagGroupStateChanged();
        }
    });
}

void TagManagerController::removeTagFromGroupAsync(const QString& tagName, int groupId) {
    (void)QtConcurrent::run([this, tagName, groupId]() {
        if (TagLexiconService::instance().removeTagFromGroup(tagName, groupId)) {
            emit tagGroupStateChanged();
        }
    });
}

} // namespace QuarkMeta
