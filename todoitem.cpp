#include "todoitem.h"

TodoItem::TodoItem()
    : m_completed(false)
{
}

TodoItem::TodoItem(const QString &title, const QDateTime &dueDate, const QColor &color, const QString &note, bool completed)
    : m_title(title)
    , m_dueDate(dueDate)
    , m_color(color)
    , m_note(note)
    , m_completed(completed)
{
}

QString TodoItem::title() const
{
    return m_title;
}

void TodoItem::setTitle(const QString &title)
{
    m_title = title;
}

QDateTime TodoItem::dueDate() const
{
    return m_dueDate;
}

void TodoItem::setDueDate(const QDateTime &dueDate)
{
    m_dueDate = dueDate;
}

QColor TodoItem::color() const
{
    return m_color;
}

void TodoItem::setColor(const QColor &color)
{
    m_color = color;
}

QString TodoItem::note() const
{
    return m_note;
}

void TodoItem::setNote(const QString &note)
{
    m_note = note;
}

bool TodoItem::completed() const
{
    return m_completed;
}

void TodoItem::setCompleted(bool completed)
{
    m_completed = completed;
}

QJsonObject TodoItem::toJson() const
{
    QJsonObject json;
    json["title"] = m_title;
    json["dueDate"] = m_dueDate.toString(Qt::ISODate);
    json["color"] = m_color.name();
    json["note"] = m_note;
    json["completed"] = m_completed;
    return json;
}

TodoItem TodoItem::fromJson(const QJsonObject &json)
{
    TodoItem item;
    item.setTitle(json["title"].toString());
    item.setDueDate(QDateTime::fromString(json["dueDate"].toString(), Qt::ISODate));
    item.setColor(QColor(json["color"].toString()));
    item.setNote(json["note"].toString());
    item.setCompleted(json["completed"].toBool());
    return item;
}
