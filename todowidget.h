#ifndef TODOWIDGET_H
#define TODOWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QListWidgetItem>
#include "todoitem.h"

class TodoWidget : public QWidget {
    Q_OBJECT

public:
    explicit TodoWidget(QWidget *parent = nullptr);

    void addTodoItem(const TodoItem &item);
    void removeTodoItem(int index);
    void toggleTodoItemCompletion(int index);
    void updateTodoItem(int index, const TodoItem &item);

    QList<TodoItem> todoItems() const;
    void setTodoItems(const QList<TodoItem> &items);

    void clearAll();

    void setHourHandColor(const QColor &color);

    void updateHeight();
    
signals:
    void addButtonClicked();
    void todoItemClicked(int index);
    void todoItemCompleted(int index, bool completed);
    void heightChanged(int newHeight);
    
private slots:
    void onAddButtonClicked();
    void onItemClicked(QListWidgetItem *item);
    void onItemChanged(QListWidgetItem *item);
    
private:
    void setupUI();
    void updateItemDisplay(QListWidgetItem *item, const TodoItem &todoItem);
    void updateTodoList();
    void resizeEvent(QResizeEvent *event) override;
    
    QVBoxLayout *m_layout;
    QListWidget *m_todoList;
    QPushButton *m_addButton;
    QList<TodoItem> m_todoItems;
    QColor m_hourHandColor;
    bool m_isUpdatingHeight = false;
};

#endif // TODOWIDGET_H