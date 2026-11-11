#ifndef RAPIDNOTES_TODOREPOSITORY_H
#define RAPIDNOTES_TODOREPOSITORY_H

#include <QObject>
#include <QList>
#include <QDate>
#include <QDateTime>
#include "DatabaseManager.h"

namespace RapidNotes {

// 遵循 SSOT 实体契约：过度期兼容引用并确立独立 TodoItem 契约
using TodoItem = DatabaseManager::Todo;

class TodoRepository : public QObject {
    Q_OBJECT
public:
    static TodoRepository& instance();

    QList<TodoItem> getTodosByDate(const QDate& date);
    QList<TodoItem> getAllTodos();
    int addTodo(const TodoItem& todo);
    bool updateTodo(const TodoItem& todo);
    bool deleteTodo(int id);
    bool deleteTodosBatch(const QList<int>& ids);

signals:
    void todoChanged();

private:
    explicit TodoRepository(QObject* parent = nullptr);
    ~TodoRepository() override = default;
};

} // namespace RapidNotes

#endif // RAPIDNOTES_TODOREPOSITORY_H
