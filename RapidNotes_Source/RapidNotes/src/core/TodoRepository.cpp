#include "TodoRepository.h"
#include "DatabaseManager.h"

namespace RapidNotes {

TodoRepository& TodoRepository::instance() {
    static TodoRepository s_instance;
    return s_instance;
}

TodoRepository::TodoRepository(QObject* parent) : QObject(parent) {
    connect(&DatabaseManager::instance(), &DatabaseManager::todoChanged, this, &TodoRepository::todoChanged);
}

QList<TodoItem> TodoRepository::getTodosByDate(const QDate& date) {
    return DatabaseManager::instance().getTodosByDate(date);
}

QList<TodoItem> TodoRepository::getAllTodos() {
    return DatabaseManager::instance().getAllTodos();
}

int TodoRepository::addTodo(const TodoItem& todo) {
    // 严格对齐实际返回值契约：返回底层真实生成的新待办 ID
    return DatabaseManager::instance().addTodo(todo);
}

bool TodoRepository::updateTodo(const TodoItem& todo) {
    return DatabaseManager::instance().updateTodo(todo);
}

bool TodoRepository::deleteTodo(int id) {
    return DatabaseManager::instance().deleteTodo(id);
}

bool TodoRepository::deleteTodosBatch(const QList<int>& ids) {
    // 严格调用底层单事务原子批量删除接口，杜绝任何 for 循环串行调用
    return DatabaseManager::instance().deleteTodosBatch(ids);
}

} // namespace RapidNotes
