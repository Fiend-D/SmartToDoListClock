#include "todowidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QStyleOption>
#include <QTimer>  // ✅ 添加这行
#include <QPainter>

TodoWidget::TodoWidget(QWidget *parent) : QWidget(parent), m_hourHandColor(Qt::blue)
{
    setupUI();
}

void TodoWidget::setupUI()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(1);  // 更紧凑的间距
    
    setAttribute(Qt::WA_TranslucentBackground, true);
    setStyleSheet("");
    
    // 代办列表在上面
    m_todoList = new QListWidget(this);
    m_todoList->setSelectionMode(QAbstractItemView::NoSelection);
    m_todoList->setFocusPolicy(Qt::NoFocus);
    m_todoList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_todoList->setMinimumHeight(0);  // 确保没有代办项时也能正确显示
    m_todoList->setStyleSheet(
        "QListWidget { background: transparent; border: none; padding: 0; margin: 0; }"
        "QListWidget::item { background: transparent; border: none; padding: 0; margin: 0; }"
        "QListWidget::item:selected { background: transparent; border: none; }"
    );
    m_todoList->setAttribute(Qt::WA_TranslucentBackground, true);
    m_layout->addWidget(m_todoList);
    
    // 添加按钮在最下面
    m_addButton = new QPushButton(this);
    m_addButton->setText("添加代办");
    m_addButton->setStyleSheet(
        "QPushButton { "
        "background-color: transparent; "
        "border: 2px solid rgba(100, 100, 100, 0.5); "
        "border-radius: 5px; "
        "font-size: 14px; "
        "color: rgba(100, 100, 100, 0.8); "
        "text-align: center; "
        "padding-left: 5px; "
        "min-height: 30px; "
        "}"
        "QPushButton:hover { "
        "border-color: rgba(100, 100, 100, 0.8); "
        "color: rgba(100, 100, 100, 1.0); "
        "}"
        "QPushButton:pressed { "
        "background-color: rgba(200, 200, 200, 0.2); "
        "}"
    );
    // 添加加号图标
    m_addButton->setIcon(QIcon::fromTheme("list-add"));
    m_addButton->setIconSize(QSize(16, 16));
    m_addButton->setStyleSheet(m_addButton->styleSheet() + 
        "QPushButton::icon { "
        "margin-right: 10px; "
        "}"
    );
    m_layout->addWidget(m_addButton);
    
    // 确保 QListWidget 与添加按钮宽度相同
    m_todoList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_addButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    connect(m_addButton, &QPushButton::clicked, this, &TodoWidget::onAddButtonClicked);
    connect(m_todoList, &QListWidget::itemClicked, this, &TodoWidget::onItemClicked);
    connect(m_todoList, &QListWidget::itemChanged, this, &TodoWidget::onItemChanged);
    
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
}

void TodoWidget::removeTodoItem(int index)
{
    if (index >= 0 && index < m_todoItems.size()) {
        m_todoItems.removeAt(index);
        delete m_todoList->takeItem(index);
        
        // 更新剩余项的索引
        for (int i = 0; i < m_todoList->count(); i++) {
            m_todoList->item(i)->setData(Qt::UserRole, i);
        }
        
        updateHeight();
    }
}

void TodoWidget::toggleTodoItemCompletion(int index)
{
    if (index >= 0 && index < m_todoItems.size()) {
        m_todoItems[index].setCompleted(!m_todoItems[index].completed());
        
        // 更新当前项的显示
        QListWidgetItem *listItem = m_todoList->item(index);
        if (listItem) {
            updateItemDisplay(listItem, m_todoItems[index]);
        }
        
        // 发出完成状态变化信号
        emit todoItemCompleted(index, m_todoItems[index].completed());
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
}

void TodoWidget::setHourHandColor(const QColor &color)
{
    // 只有当颜色真正改变时才更新
    if (m_hourHandColor != color) {
        m_hourHandColor = color;
        updateTodoList();
    }
}

void TodoWidget::updateHeight()
{
    // 防止递归：如果正在更新，直接返回
    if (m_isUpdatingHeight) return;
    m_isUpdatingHeight = true;

    // 强制布局更新，确保获取正确高度
    m_layout->activate();

    // 取按钮实际高度（已随主窗口缩放）
    int btnHeight = m_addButton->height();
    if (btnHeight <= 0) {
        btnHeight = m_addButton->sizeHint().height();
    }
    if (btnHeight <= 0) btnHeight = 35;  // 保底

    // 列表高度：基于按钮高度的比例（保持一致）
    int itemHeight = btnHeight; // 1:1
    int listHeight = m_todoList->count() * itemHeight;

    int currentHeight = listHeight + m_layout->spacing() + btnHeight;

    // 保险：无代办项时至少显示按钮
    if (m_todoList->count() == 0) {
        m_todoList->setFixedHeight(0);  // 强制列表高度为0
        currentHeight = btnHeight + m_layout->spacing();  // 按钮 + 间距
    } else {
        // 有代办项时，设置列表高度
        m_todoList->setFixedHeight(listHeight);
    }

    setMinimumHeight(currentHeight);
    setMaximumHeight(currentHeight);
    setFixedHeight(currentHeight);

    m_isUpdatingHeight = false;
    emit heightChanged(currentHeight);
}

void TodoWidget::updateTodoList()
{
    // ✅ 批量更新标志，防止 heightChanged 信号递归
    static bool isBatchUpdating = false;
    if (isBatchUpdating) return;
    isBatchUpdating = true;
    
    // ✅ 先安全清理所有 widget，避免重复 delete
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
    
    // 清除所有 QListWidgetItem
    m_todoList->clear();
    m_todoItems.clear();  // 注意：这会清空数据，如果需要保留数据，不要 clear
    
    // 重新添加（从 m_todoItems 重新加载）
    for (int i = 0; i < m_todoItems.size(); i++) {
        QListWidgetItem *listItem = new QListWidgetItem(m_todoList);
        listItem->setFlags(listItem->flags() | Qt::ItemIsUserCheckable);
        listItem->setCheckState(m_todoItems[i].completed() ? Qt::Checked : Qt::Unchecked);
        listItem->setData(Qt::UserRole, i);
        // 注意：此时不调用 updateItemDisplay，避免递归
    }
    
    // 延迟更新显示（确保 deleteLater 已执行）
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
    }
}

void TodoWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 确保 QListWidget 与添加按钮宽度相同
    if (m_addButton && m_todoList) {
        int buttonWidth = m_addButton->width();
        m_todoList->setFixedWidth(buttonWidth);

        // 更新所有代办项的宽度和高度（基于按钮高度，保持1:1比例）
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

void TodoWidget::updateItemDisplay(QListWidgetItem *item, const TodoItem &todoItem)
{

    
    // ✅ 防递归保护
    static thread_local bool isUpdating = false;
    if (isUpdating) {

        return;
    }
    isUpdating = true;
    
    // 设置与添加按钮相同的宽度和高度
    int buttonWidth = m_addButton->width();
    int itemHeight = m_addButton->height();
    if (itemHeight <= 0) itemHeight = 40;
    item->setSizeHint(QSize(buttonWidth, itemHeight));
    
    // ✅ 安全移除旧的 widget
    QWidget *oldWidget = m_todoList->itemWidget(item);
    if (oldWidget) {

        
        // 1. 断开所有信号（防止按钮 clicked 信号还在处理）
        oldWidget->disconnect();
        oldWidget->blockSignals(true);
        
        // 2. 先解除 QListWidgetItem 的关联（关键！）
        m_todoList->setItemWidget(item, nullptr);
        
        // 3. 使用 deleteLater 延迟删除（放入事件队列尾部）
        oldWidget->setParent(nullptr);  // 断开 parent，防止布局重算
        oldWidget->deleteLater();
    }
    
    QWidget *widget = new QWidget();
    QHBoxLayout *widgetLayout = new QHBoxLayout(widget);
    widgetLayout->setContentsMargins(0, 0, 0, 0);
    widgetLayout->setSpacing(10);
    
    QString colorName = m_hourHandColor.name();
    QString completedStyle = todoItem.completed() ? 
        QString("text-decoration: line-through; color: #999999;") : 
        QString("color: %1;").arg(colorName);
    
    // 去掉外层边框
    widget->setStyleSheet(QString(
        "QWidget { "
        "border: none; "
        "background-color: transparent; "
        "}"
    ).arg(colorName));
    
    // 圆形选择框（可点击）
    QPushButton *checkBox = new QPushButton(widget);
    checkBox->setFixedSize(16, 16);
    if (todoItem.completed()) {
        checkBox->setStyleSheet(QString(
            "QPushButton { "
            "background-color: %1; "
            "border: 1px solid %1; "
            "border-radius: 8px; "
            "}"
            "QPushButton:hover { "
            "background-color: %1; "
            "opacity: 0.8; "
            "}"
        ).arg(colorName));
    } else {
        checkBox->setStyleSheet(QString(
            "QPushButton { "
            "background-color: transparent; "
            "border: 1px solid %1; "
            "border-radius: 8px; "
            "}"
            "QPushButton:hover { "
            "background-color: rgba(%2, %3, %4, 0.1); "
            "}"
        ).arg(colorName)
         .arg(m_hourHandColor.red())
         .arg(m_hourHandColor.green())
         .arg(m_hourHandColor.blue()));
    }
    // 往左移动一点，添加左边距
    widgetLayout->addWidget(checkBox);
    widgetLayout->setContentsMargins(-5, 0, 0, 0);
    
    // 连接选择框点击信号
    connect(checkBox, &QPushButton::clicked, this, [this, item]() {
        int index = item->data(Qt::UserRole).toInt();
        toggleTodoItemCompletion(index);
    });
    
    // 标题（主题）
    QLabel *titleLabel = new QLabel(todoItem.title(), widget);
    titleLabel->setStyleSheet(QString("%1 font-size: 13px; font-weight: bold;").arg(completedStyle));
    titleLabel->setWordWrap(false);
    widgetLayout->addWidget(titleLabel, 1);
    
    // 截止日期
    if (todoItem.dueDate().isValid()) {
        QLabel *dateLabel = new QLabel(todoItem.dueDate().toString("MM-dd HH:mm"), widget);
        dateLabel->setStyleSheet(QString("color: %1; font-size: 11px;").arg(colorName));
        dateLabel->setFixedWidth(80);
        dateLabel->setAlignment(Qt::AlignCenter);
        widgetLayout->addWidget(dateLabel);
    }
    
    // 删除按钮（图形按钮）
    QPushButton *deleteButton = new QPushButton(widget);
    deleteButton->setFixedSize(16, 16);
    deleteButton->setStyleSheet(QString(
        "QPushButton { "
        "background-color: transparent; "
        "border: 1px solid %1; "
        "border-radius: 8px; "
        "color: %1; "
        "font-size: 10px; "
        "font-weight: bold; "
        "}"
        "QPushButton:hover { "
        "background-color: rgba(255, 0, 0, 0.1); "
        "}"
        "QPushButton:pressed { "
        "background-color: rgba(255, 0, 0, 0.2); "
        "}"
    ).arg(colorName));
    deleteButton->setText("×");
    widgetLayout->addWidget(deleteButton);
    
    // 连接删除按钮信号
    connect(deleteButton, &QPushButton::clicked, this, [this, item]() {
        int index = item->data(Qt::UserRole).toInt();
        removeTodoItem(index);
    });
    
    m_todoList->setItemWidget(item, widget);
    isUpdating = false;
}
