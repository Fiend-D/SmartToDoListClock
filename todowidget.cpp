#include "todowidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QStyleOption>
#include <QTimer>
#include <QPainter>
#include <QDateTime>
#include "neumorphism.h"

TodoWidget::TodoWidget(QWidget *parent) : QWidget(parent), m_hourHandColor(Qt::blue), m_accentColor(Qt::blue)
{
    setupUI();
}

void TodoWidget::setupUI()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(6);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setStyleSheet("");

    // 进度标签
    m_progressLabel = new QLabel(this);
    m_progressLabel->setAlignment(Qt::AlignCenter);
    m_progressLabel->setStyleSheet("color: rgba(180, 180, 180, 0.7); font-size: 11px; padding: 2px;");
    m_progressLabel->setText("暂无待办事项");
    m_layout->addWidget(m_progressLabel);

    // 代办列表
    m_todoList = new QListWidget(this);
    m_todoList->setSelectionMode(QAbstractItemView::NoSelection);
    m_todoList->setFocusPolicy(Qt::NoFocus);
    m_todoList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_todoList->setMinimumHeight(0);
    m_todoList->setStyleSheet(
        "QListWidget { background: transparent; border: none; padding: 0; margin: 0; }"
        "QListWidget::item { background: transparent; border: none; padding: 0; margin: 0; }"
        "QListWidget::item:selected { background: transparent; border: none; }"
    );
    m_todoList->setAttribute(Qt::WA_TranslucentBackground, true);
    m_layout->addWidget(m_todoList);

    // 添加按钮 - 新拟态风格
    m_addButton = new QPushButton(this);
    m_addButton->setText("+ 添加代办");
    m_addButton->setStyleSheet(
        "QPushButton { "
        "background-color: transparent; "
        "border: 1px solid rgba(120, 120, 120, 0.4); "
        "border-radius: 8px; "
        "font-size: 13px; "
        "color: rgba(150, 150, 150, 0.9); "
        "text-align: center; "
        "padding: 6px; "
        "min-height: 28px; "
        "}"
        "QPushButton:hover { "
        "border-color: rgba(150, 150, 150, 0.7); "
        "color: rgba(180, 180, 180, 1.0); "
        "background-color: rgba(255, 255, 255, 0.05); "
        "}"
        "QPushButton:pressed { "
        "background-color: rgba(200, 200, 200, 0.1); "
        "}"
    );
    m_layout->addWidget(m_addButton);

    m_todoList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_addButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(m_addButton, &QPushButton::clicked, this, &TodoWidget::onAddButtonClicked);
    connect(m_todoList, &QListWidget::itemClicked, this, &TodoWidget::onItemClicked);
    connect(m_todoList, &QListWidget::itemChanged, this, &TodoWidget::onItemChanged);

    // 定时更新进度和逾期状态
    QTimer *refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, [this]() {
        for (int i = 0; i < m_todoList->count(); ++i) {
            QListWidgetItem *item = m_todoList->item(i);
            if (item) {
                int idx = item->data(Qt::UserRole).toInt();
                if (idx >= 0 && idx < m_todoItems.size()) {
                    updateItemDisplay(item, m_todoItems[idx]);
                }
            }
        }
        updateProgress();
    });
    refreshTimer->start(30000); // 30秒刷新一次

    QTimer::singleShot(0, this, [this]() {
        updateHeight();
    });
}

void TodoWidget::addTodoItem(const TodoItem &item)
{
    m_todoItems.append(item);

    QListWidgetItem *listItem = new QListWidgetItem(m_todoList);
    listItem->setFlags(listItem->flags() | Qt::ItemIsUserCheckable);
    listItem->setCheckState(item.completed() ? Qt::Checked : Qt::Unchecked);

    updateItemDisplay(listItem, item);
    listItem->setData(Qt::UserRole, m_todoItems.size() - 1);

    updateHeight();
    updateProgress();
}

void TodoWidget::removeTodoItem(int index)
{
    if (index >= 0 && index < m_todoItems.size()) {
        m_todoItems.removeAt(index);
        delete m_todoList->takeItem(index);

        for (int i = 0; i < m_todoList->count(); i++) {
            m_todoList->item(i)->setData(Qt::UserRole, i);
        }

        updateHeight();
        updateProgress();
    }
}

void TodoWidget::toggleTodoItemCompletion(int index)
{
    if (index >= 0 && index < m_todoItems.size()) {
        m_todoItems[index].setCompleted(!m_todoItems[index].completed());

        QListWidgetItem *listItem = m_todoList->item(index);
        if (listItem) {
            listItem->setCheckState(m_todoItems[index].completed() ? Qt::Checked : Qt::Unchecked);
            updateItemDisplay(listItem, m_todoItems[index]);
        }

        emit todoItemCompleted(index, m_todoItems[index].completed());
        updateProgress();
    }
}

void TodoWidget::updateTodoItem(int index, const TodoItem &item)
{
    if (index >= 0 && index < m_todoItems.size()) {
        m_todoItems[index] = item;
        QListWidgetItem *listItem = m_todoList->item(index);
        if (listItem) {
            listItem->setCheckState(item.completed() ? Qt::Checked : Qt::Unchecked);
            updateItemDisplay(listItem, item);
        }
        updateProgress();
    }
}

QList<TodoItem> TodoWidget::todoItems() const
{
    return m_todoItems;
}

void TodoWidget::setTodoItems(const QList<TodoItem> &items)
{
    m_todoList->clear();
    m_todoItems.clear();

    for (const TodoItem &item : items) {
        addTodoItem(item);
    }
}

void TodoWidget::clearAll()
{
    m_todoItems.clear();
    m_todoList->clear();
    updateHeight();
    updateProgress();
}

void TodoWidget::setHourHandColor(const QColor &color)
{
    if (m_hourHandColor != color) {
        m_hourHandColor = color;
        m_accentColor = color;
        updateTodoList();
    }
}

void TodoWidget::setAccentColor(const QColor &color)
{
    if (m_accentColor != color) {
        m_accentColor = color;
        updateTodoList();
    }
}

void TodoWidget::updateHeight()
{
    if (m_isUpdatingHeight) return;
    m_isUpdatingHeight = true;

    m_layout->activate();

    int btnHeight = m_addButton->height();
    if (btnHeight <= 0) btnHeight = m_addButton->sizeHint().height();
    if (btnHeight <= 0) btnHeight = 35;

    int progressHeight = m_progressLabel->isVisible() ? m_progressLabel->height() : 0;
    int itemHeight = btnHeight;
    int listHeight = m_todoList->count() * itemHeight;

    int currentHeight = progressHeight + listHeight + m_layout->spacing() * 2 + btnHeight;

    if (m_todoList->count() == 0) {
        m_todoList->setFixedHeight(0);
        currentHeight = progressHeight + btnHeight + m_layout->spacing() * 2;
    } else {
        m_todoList->setFixedHeight(listHeight);
    }

    setMinimumHeight(currentHeight);
    setMaximumHeight(currentHeight);
    setFixedHeight(currentHeight);

    m_isUpdatingHeight = false;
    emit heightChanged(currentHeight);
}

void TodoWidget::updateProgress()
{
    int total = m_todoItems.size();
    if (total == 0) {
        m_progressLabel->setText("暂无待办事项");
        m_progressLabel->setVisible(true);
        return;
    }

    int completed = 0;
    int overdue = 0;
    int urgent = 0;

    for (const auto &item : m_todoItems) {
        if (item.completed()) completed++;
        if (item.isOverdue()) overdue++;
        if (item.priority() == TodoItem::Urgent && !item.completed()) urgent++;
    }

    qreal percent = total > 0 ? (completed * 100.0 / total) : 0;

    QString text;
    if (urgent > 0) {
        text = QString("⏰ %1个紧急 | 完成 %2%").arg(urgent).arg(qRound(percent));
    } else if (overdue > 0) {
        text = QString("⚠️ %1个逾期 | 完成 %2%").arg(overdue).arg(qRound(percent));
    } else {
        text = QString("完成 %1/%2 (%3%)").arg(completed).arg(total).arg(qRound(percent));
    }

    m_progressLabel->setText(text);

    // 根据状态设置颜色
    if (urgent > 0) {
        m_progressLabel->setStyleSheet("color: rgba(255, 120, 100, 0.9); font-size: 11px; padding: 2px; font-weight: bold;");
    } else if (overdue > 0) {
        m_progressLabel->setStyleSheet("color: rgba(255, 180, 80, 0.9); font-size: 11px; padding: 2px;");
    } else if (percent >= 100) {
        m_progressLabel->setStyleSheet("color: rgba(100, 220, 120, 0.9); font-size: 11px; padding: 2px; font-weight: bold;");
    } else {
        m_progressLabel->setStyleSheet("color: rgba(180, 180, 180, 0.7); font-size: 11px; padding: 2px;");
    }

    m_progressLabel->setVisible(true);
}

void TodoWidget::updateTodoList()
{
    static bool isBatchUpdating = false;
    if (isBatchUpdating) return;
    isBatchUpdating = true;

    for (int i = m_todoList->count() - 1; i >= 0; --i) {
        QListWidgetItem *item = m_todoList->item(i);
        QWidget *widget = m_todoList->itemWidget(item);
        if (widget) {
            widget->disconnect();
            widget->blockSignals(true);
            m_todoList->setItemWidget(item, nullptr);
            widget->setParent(nullptr);
            widget->deleteLater();
        }
    }

    m_todoList->clear();
    m_todoItems.clear();

    for (int i = 0; i < m_todoItems.size(); i++) {
        QListWidgetItem *listItem = new QListWidgetItem(m_todoList);
        listItem->setFlags(listItem->flags() | Qt::ItemIsUserCheckable);
        listItem->setCheckState(m_todoItems[i].completed() ? Qt::Checked : Qt::Unchecked);
        listItem->setData(Qt::UserRole, i);
    }

    QTimer::singleShot(0, this, [this]() {
        for (int i = 0; i < m_todoItems.size(); i++) {
            QListWidgetItem *item = m_todoList->item(i);
            if (item) {
                updateItemDisplay(item, m_todoItems[i]);
            }
        }
        updateHeight();
        isBatchUpdating = false;
    });
}

void TodoWidget::onAddButtonClicked()
{
    emit addButtonClicked();
}

void TodoWidget::onItemClicked(QListWidgetItem *item)
{
    int index = item->data(Qt::UserRole).toInt();
    emit todoItemClicked(index);
}

void TodoWidget::onItemChanged(QListWidgetItem *item)
{
    int index = item->data(Qt::UserRole).toInt();
    bool completed = (item->checkState() == Qt::Checked);

    if (index >= 0 && index < m_todoItems.size()) {
        m_todoItems[index].setCompleted(completed);
        updateItemDisplay(item, m_todoItems[index]);
        emit todoItemCompleted(index, completed);
        updateProgress();
    }
}

void TodoWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if (m_addButton && m_todoList) {
        int buttonWidth = m_addButton->width();
        m_todoList->setFixedWidth(buttonWidth);

        int itemHeight = m_addButton->height();
        if (itemHeight <= 0) itemHeight = 40;
        for (int i = 0; i < m_todoList->count(); i++) {
            QListWidgetItem *item = m_todoList->item(i);
            if (item) {
                item->setSizeHint(QSize(buttonWidth, itemHeight));
            }
        }
    }
}

void TodoWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}

void TodoWidget::updateItemDisplay(QListWidgetItem *item, const TodoItem &todoItem)
{
    static thread_local bool isUpdating = false;
    if (isUpdating) return;
    isUpdating = true;

    int buttonWidth = m_addButton->width();
    int itemHeight = m_addButton->height();
    if (itemHeight <= 0) itemHeight = 40;
    item->setSizeHint(QSize(buttonWidth, itemHeight));

    QWidget *oldWidget = m_todoList->itemWidget(item);
    if (oldWidget) {
        oldWidget->disconnect();
        oldWidget->blockSignals(true);
        m_todoList->setItemWidget(item, nullptr);
        oldWidget->setParent(nullptr);
        oldWidget->deleteLater();
    }

    QWidget *widget = new QWidget();
    QHBoxLayout *widgetLayout = new QHBoxLayout(widget);
    widgetLayout->setContentsMargins(8, 4, 8, 4);
    widgetLayout->setSpacing(8);

    QString colorName = m_accentColor.isValid() ? m_accentColor.name() : m_hourHandColor.name();

    bool isDone = todoItem.completed();
    bool isOverdue = todoItem.isOverdue();

    // 根据状态确定文字颜色
    QString textColor = isDone ? "#777777" : (isOverdue ? "#ff8a65" : colorName);
    QString titleStyle = isDone ?
        "text-decoration: line-through; color: #666666;" :
        QString("color: %1;").arg(textColor);

    // 逾期闪烁效果
    bool shouldBlink = isOverdue && !isDone;
    if (shouldBlink) {
        int ms = QDateTime::currentDateTime().time().msec();
        if (ms < 500) {
            titleStyle += " font-weight: bold;";
        }
    }

    // 优先级颜色
    QColor prioColor = todoItem.priorityColor();
    QString prioStyle = QString("background-color: %1; border-radius: 3px;").arg(prioColor.name());

    // 设置widget样式
    widget->setStyleSheet("QWidget { border: none; background-color: transparent; }");

    // 优先级指示条
    QLabel *prioBar = new QLabel(widget);
    prioBar->setFixedSize(4, itemHeight - 12);
    prioBar->setStyleSheet(prioStyle);
    widgetLayout->addWidget(prioBar);

    // 圆形选择框
    QPushButton *checkBox = new QPushButton(widget);
    checkBox->setFixedSize(16, 16);
    if (isDone) {
        checkBox->setStyleSheet(QString(
            "QPushButton { background-color: %1; border: 1px solid %1; border-radius: 8px; }"
            "QPushButton:hover { opacity: 0.8; }"
        ).arg(colorName));
    } else {
        checkBox->setStyleSheet(QString(
            "QPushButton { background-color: transparent; border: 1.5px solid %1; border-radius: 8px; }"
            "QPushButton:hover { background-color: rgba(%2, %3, %4, 0.15); }"
        ).arg(colorName)
         .arg(m_accentColor.red()).arg(m_accentColor.green()).arg(m_accentColor.blue()));
    }
    widgetLayout->addWidget(checkBox);

    connect(checkBox, &QPushButton::clicked, this, [this, item]() {
        int index = item->data(Qt::UserRole).toInt();
        toggleTodoItemCompletion(index);
    });

    // 标题 + 优先级标签
    QWidget *titleContainer = new QWidget(widget);
    QHBoxLayout *titleLayout = new QHBoxLayout(titleContainer);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(4);

    QLabel *titleLabel = new QLabel(todoItem.title(), titleContainer);
    titleLabel->setStyleSheet(QString("%1 font-size: 13px;").arg(titleStyle));
    titleLabel->setWordWrap(false);
    titleLayout->addWidget(titleLabel, 1);

    // 优先级文字标签
    QLabel *prioLabel = new QLabel(todoItem.priorityText(), titleContainer);
    prioLabel->setStyleSheet(QString(
        "color: %1; font-size: 10px; background-color: rgba(%2, %3, %4, 0.15);"
        "border-radius: 4px; padding: 1px 5px;"
    ).arg(prioColor.name())
     .arg(prioColor.red()).arg(prioColor.green()).arg(prioColor.blue()));
    titleLayout->addWidget(prioLabel);

    widgetLayout->addWidget(titleContainer, 1);

    // 截止日期
    if (todoItem.dueDate().isValid()) {
        QString dateText = todoItem.dueDate().toString("MM-dd HH:mm");
        if (todoItem.daysUntilDue() == 0 && !isDone) {
            dateText = "今天 " + todoItem.dueDate().toString("HH:mm");
        } else if (todoItem.daysUntilDue() == 1 && !isDone) {
            dateText = "明天 " + todoItem.dueDate().toString("HH:mm");
        }

        QString dateColor = isOverdue && !isDone ? "#ff8a65" : colorName;
        QLabel *dateLabel = new QLabel(dateText, widget);
        dateLabel->setStyleSheet(QString("color: %1; font-size: 10px;").arg(dateColor));
        dateLabel->setFixedWidth(85);
        dateLabel->setAlignment(Qt::AlignCenter);
        widgetLayout->addWidget(dateLabel);
    }

    // 删除按钮
    QPushButton *deleteButton = new QPushButton(widget);
    deleteButton->setFixedSize(18, 18);
    deleteButton->setStyleSheet(QString(
        "QPushButton { background-color: transparent; border: 1px solid %1; border-radius: 9px;"
        "color: %1; font-size: 11px; font-weight: bold; }"
        "QPushButton:hover { background-color: rgba(255, 80, 80, 0.2); border-color: #ff5050; color: #ff5050; }"
        "QPushButton:pressed { background-color: rgba(255, 80, 80, 0.3); }"
    ).arg(colorName));
    deleteButton->setText("×");
    widgetLayout->addWidget(deleteButton);

    connect(deleteButton, &QPushButton::clicked, this, [this, item]() {
        int index = item->data(Qt::UserRole).toInt();
        removeTodoItem(index);
    });

    m_todoList->setItemWidget(item, widget);
    isUpdating = false;
}
