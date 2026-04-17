#include "addtododialog.h"
#include <QLabel>

AddTodoDialog::AddTodoDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("添加代办");
    setFixedSize(400, 350);
    setupUI();
    setupColorButtons();
    
    // 默认选择第一个颜色
    m_selectedColor = QColor("#4CAF50");
    m_colorButtonGroup->button(0)->setChecked(true);
}

void AddTodoDialog::setupUI()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(20, 20, 20, 20);
    m_layout->setSpacing(15);
    
    // 标题输入
    QLabel *titleLabel = new QLabel("标题:");
    m_titleEdit = new QLineEdit(this);
    m_titleEdit->setPlaceholderText("输入代办标题");
    
    // 截止时间
    QLabel *dueDateLabel = new QLabel("截止时间:");
    m_dueDateEdit = new QDateTimeEdit(QDateTime::currentDateTime().addDays(1), this);
    m_dueDateEdit->setCalendarPopup(true);
    m_dueDateEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    
    // 颜色选择
    QLabel *colorLabel = new QLabel("标题颜色:");
    
    // 备注
    QLabel *noteLabel = new QLabel("备注:");
    m_noteEdit = new QTextEdit(this);
    m_noteEdit->setPlaceholderText("输入备注信息");
    m_noteEdit->setFixedHeight(100);
    
    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    
    m_okButton = new QPushButton("确定", this);
    m_cancelButton = new QPushButton("取消", this);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    
    // 添加到布局
    m_layout->addWidget(titleLabel);
    m_layout->addWidget(m_titleEdit);
    m_layout->addWidget(dueDateLabel);
    m_layout->addWidget(m_dueDateEdit);
    m_layout->addWidget(colorLabel);
    
    // 颜色按钮组将在setupColorButtons中添加
    
    m_layout->addWidget(noteLabel);
    m_layout->addWidget(m_noteEdit);
    m_layout->addLayout(buttonLayout);
    
    // 连接信号槽
    connect(m_okButton, &QPushButton::clicked, this, &AddTodoDialog::onOkClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &AddTodoDialog::onCancelClicked);
}

void AddTodoDialog::setupColorButtons()
{
    m_colorButtonGroup = new QButtonGroup(this);
    QHBoxLayout *colorLayout = new QHBoxLayout();
    colorLayout->setSpacing(10);
    
    // 常用颜色
    QList<QColor> colors = {
        QColor("#4CAF50"),  // 绿色
        QColor("#2196F3"),  // 蓝色
        QColor("#FF9800"),  // 橙色
        QColor("#9C27B0"),  // 紫色
        QColor("#F44336"),  // 红色
        QColor("#607D8B"),  // 蓝灰色
        QColor("#FFC107"),  // 黄色
        QColor("#795548")   // 棕色
    };
    
    for (int i = 0; i < colors.size(); i++) {
        QRadioButton *button = new QRadioButton(this);
        button->setFixedSize(24, 24);
        button->setStyleSheet(QString(
            "QRadioButton::indicator { "
            "width: 20px; "
            "height: 20px; "
            "border-radius: 10px; "
            "background-color: %1; "
            "border: 2px solid white; "
            "}"
            "QRadioButton::indicator:checked { "
            "border: 2px solid #333333; "
            "}"
        ).arg(colors[i].name()));
        
        m_colorButtonGroup->addButton(button, i);
        colorLayout->addWidget(button);
    }
    
    // 自定义颜色按钮
    m_customColorButton = new QRadioButton("自定义", this);
    colorLayout->addWidget(m_customColorButton);
    m_colorButtonGroup->addButton(m_customColorButton, colors.size());
    
    m_layout->addLayout(colorLayout);
    
    // 连接信号槽
    connect(m_colorButtonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            this, &AddTodoDialog::onColorButtonClicked);
}

TodoItem AddTodoDialog::getTodoItem() const
{
    return TodoItem(
        m_titleEdit->text(),
        m_dueDateEdit->dateTime(),
        m_selectedColor,
        m_noteEdit->toPlainText(),
        false
    );
}

void AddTodoDialog::setTodoItem(const TodoItem &item)
{
    m_titleEdit->setText(item.title());
    m_dueDateEdit->setDateTime(item.dueDate());
    m_noteEdit->setPlainText(item.note());
    m_selectedColor = item.color();
    
    // 查找并选择对应的颜色按钮
    bool found = false;
    QList<QColor> colors = {
        QColor("#4CAF50"),  // 绿色
        QColor("#2196F3"),  // 蓝色
        QColor("#FF9800"),  // 橙色
        QColor("#9C27B0"),  // 紫色
        QColor("#F44336"),  // 红色
        QColor("#607D8B"),  // 蓝灰色
        QColor("#FFC107"),  // 黄色
        QColor("#795548")   // 棕色
    };
    
    for (int i = 0; i < colors.size(); i++) {
        if (colors[i] == item.color()) {
            m_colorButtonGroup->button(i)->setChecked(true);
            found = true;
            break;
        }
    }
    
    if (!found) {
        m_customColorButton->setChecked(true);
    }
}

void AddTodoDialog::onColorButtonClicked(QAbstractButton *button)
{
    if (button == m_customColorButton) {
        onCustomColorClicked();
    } else {
        // 提取颜色
        QList<QColor> colors = {
            QColor("#4CAF50"),  // 绿色
            QColor("#2196F3"),  // 蓝色
            QColor("#FF9800"),  // 橙色
            QColor("#9C27B0"),  // 紫色
            QColor("#F44336"),  // 红色
            QColor("#607D8B"),  // 蓝灰色
            QColor("#FFC107"),  // 黄色
            QColor("#795548")   // 棕色
        };
        
        int index = m_colorButtonGroup->id(button);
        if (index >= 0 && index < colors.size()) {
            m_selectedColor = colors[index];
        }
    }
}

void AddTodoDialog::onCustomColorClicked()
{
    QColor color = QColorDialog::getColor(m_selectedColor, this, "选择颜色");
    if (color.isValid()) {
        m_selectedColor = color;
    } else {
        // 如果用户取消选择，保持原颜色
        m_colorButtonGroup->button(0)->setChecked(true);
        m_selectedColor = QColor("#4CAF50");
    }
}

void AddTodoDialog::onOkClicked()
{
    if (m_titleEdit->text().isEmpty()) {
        // 标题不能为空
        return;
    }
    
    accept();
}

void AddTodoDialog::onCancelClicked()
{
    reject();
}