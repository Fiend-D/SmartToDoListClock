#ifndef TODOITEM_H
#define TODOITEM_H

#include <QString>
#include <QDateTime>
#include <QColor>
#include <QJsonObject>

class TodoItem {
public:
    TodoItem();
    TodoItem(const QString &title, const QDateTime &dueDate, const QColor &color, const QString &note, bool completed = false);
    
    QString title() const;
    void setTitle(const QString &title);
    
    QDateTime dueDate() const;
    void setDueDate(const QDateTime &dueDate);
    
    QColor color() const;
    void setColor(const QColor &color);
    
    QString note() const;
    void setNote(const QString &note);
    
    bool completed() const;
    void setCompleted(bool completed);
    
    QJsonObject toJson() const;
    static TodoItem fromJson(const QJsonObject &json);
    
private:
    QString m_title;
    QDateTime m_dueDate;
    QColor m_color;
    QString m_note;
    bool m_completed;
};

#endif // TODOITEM_H