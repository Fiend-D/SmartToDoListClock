#include "todoitem.h"
#include <QDateTime>

TodoItem::TodoItem()
    : m_completed(false)
    , m_priority(Medium)
{
}

TodoItem::TodoItem(const QString &title, const QDateTime &dueDate, const QColor &color,
                   const QString &note, bool completed, Priority priority, const QString &category)
    : m_title(title)
    , m_dueDate(dueDate)
    , m_color(color)
    , m_note(note)
    , m_completed(completed)
    , m_priority(priority)
    , m_category(category)
{
}

QString TodoItem::title() const { return m_title; }
void TodoItem::setTitle(const QString &title) { m_title = title; }

QDateTime TodoItem::dueDate() const { return m_dueDate; }
void TodoItem::setDueDate(const QDateTime &dueDate) { m_dueDate = dueDate; }

QColor TodoItem::color() const { return m_color; }
void TodoItem::setColor(const QColor &color) { m_color = color; }

QString TodoItem::note() const { return m_note; }
void TodoItem::setNote(const QString &note) { m_note = note; }

bool TodoItem::completed() const { return m_completed; }
void TodoItem::setCompleted(bool completed) { m_completed = completed; }

TodoItem::Priority TodoItem::priority() const { return m_priority; }
void TodoItem::setPriority(Priority priority) { m_priority = priority; }

QString TodoItem::priorityText() const
{
    switch (m_priority) {
    case Low: return "低";
    case Medium: return "中";
    case High: return "高";
    case Urgent: return "紧急";
    }
    return "中";
}

QColor TodoItem::priorityColor() const
{
    switch (m_priority) {
    case Low: return QColor(100, 200, 100);
    case Medium: return QColor(255, 200, 80);
    case High: return QColor(255, 140, 80);
    case Urgent: return QColor(255, 80, 80);
    }
    return QColor(255, 200, 80);
}

QString TodoItem::category() const { return m_category; }
void TodoItem::setCategory(const QString &category) { m_category = category; }

bool TodoItem::isOverdue() const
{
    return m_dueDate.isValid() && !m_completed && m_dueDate < QDateTime::currentDateTime();
}

int TodoItem::daysUntilDue() const
{
    if (!m_dueDate.isValid()) return INT_MAX;
    return QDateTime::currentDateTime().daysTo(m_dueDate);
}

QJsonObject TodoItem::toJson() const
{
    QJsonObject obj;
    obj["title"] = m_title;
    obj["dueDate"] = m_dueDate.toString(Qt::ISODate);
    obj["color"] = m_color.name();
    obj["note"] = m_note;
    obj["completed"] = m_completed;
    obj["priority"] = static_cast<int>(m_priority);
    obj["category"] = m_category;
    return obj;
}

TodoItem TodoItem::fromJson(const QJsonObject &json)
{
    TodoItem item;
    item.m_title = json["title"].toString();
    item.m_dueDate = QDateTime::fromString(json["dueDate"].toString(), Qt::ISODate);
    item.m_color = QColor(json["color"].toString("#ffffff"));
    item.m_note = json["note"].toString();
    item.m_completed = json["completed"].toBool(false);
    item.m_priority = static_cast<Priority>(json["priority"].toInt(1));
    item.m_category = json["category"].toString();
    return item;
}
