#include "addtododialog.h"
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QMessageBox>

AddTodoDialog::AddTodoDialog(QWidget *parent) : QDialog(parent), m_selectedColor(Qt::white)
{
    setWindowTitle("添加待办事项");
    setMinimumWidth(320);
    setupUI();
}

void AddTodoDialog::setupUI()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(12);
    m_layout->setContentsMargins(16, 16, 16, 16);

    // 表单区域
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(10);
    formLayout->setLabelAlignment(Qt::AlignRight);

    // 标题
    m_titleEdit = new QLineEdit(this);
    m_titleEdit->setPlaceholderText("输入待办事项标题...");
    m_titleEdit->setStyleSheet(
        "QLineEdit { padding: 6px; border: 1px solid rgba(120,120,120,0.4); border-radius: 6px; "
        "background-color: rgba(255,255,255,0.05); color: #0c0a0a; }"
        "QLineEdit:focus { border-color: rgba(2, 2, 2, 0.7); }"
    );
    formLayout->addRow("标题:", m_titleEdit);

    // 截止日期
    m_dueDateEdit = new QDateTimeEdit(this);
    m_dueDateEdit->setCalendarPopup(true);
    m_dueDateEdit->setDateTime(QDateTime::currentDateTime().addSecs(3600));
    m_dueDateEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    m_dueDateEdit->setStyleSheet(
        "QDateTimeEdit { padding: 5px; border: 1px solid rgba(5, 5, 5, 0.4); border-radius: 6px; "
        "background-color: rgba(255,255,255,0.05); color: #080707; }"
    );
    formLayout->addRow("截止时间:", m_dueDateEdit);

    // 优先级
    m_priorityCombo = new QComboBox(this);
    m_priorityCombo->addItem("低", static_cast<int>(TodoItem::Low));
    m_priorityCombo->addItem("中", static_cast<int>(TodoItem::Medium));
    m_priorityCombo->addItem("高", static_cast<int>(TodoItem::High));
    m_priorityCombo->addItem("紧急", static_cast<int>(TodoItem::Urgent));
    m_priorityCombo->setCurrentIndex(1);
    m_priorityCombo->setStyleSheet(
        "QComboBox { padding: 5px; border: 1px solid rgba(120,120,120,0.4); border-radius: 6px; "
        "background-color: rgba(245, 245, 245, 0.05); color: #0a0909; }"
        "QComboBox::drop-down { border: none; }"
        "QComboBox QAbstractItemView { background-color: #c5c4c4; color: #b9b4b4; selection-background-color: #404040; }"
    );
    formLayout->addRow("优先级:", m_priorityCombo);

    // 分类
    m_categoryEdit = new QLineEdit(this);
    m_categoryEdit->setPlaceholderText("工作 / 学习 / 生活...");
    m_categoryEdit->setStyleSheet(
        "QLineEdit { padding: 6px; border: 1px solid rgba(120,120,120,0.4); border-radius: 6px; "
        "background-color: rgba(255,255,255,0.05); color: #050505; }"
        "QLineEdit:focus { border-color: rgba(14, 13, 13, 0.7); }"
    );
    formLayout->addRow("分类:", m_categoryEdit);

    m_layout->addLayout(formLayout);

    // 备注
    QLabel *noteLabel = new QLabel("备注:", this);
    noteLabel->setStyleSheet("color: #070707;");
    m_layout->addWidget(noteLabel);

    m_noteEdit = new QTextEdit(this);
    m_noteEdit->setPlaceholderText("添加详细描述...");
    m_noteEdit->setMaximumHeight(80);
    m_noteEdit->setStyleSheet(
        "QTextEdit { padding: 6px; border: 1px solid rgba(120,120,120,0.4); border-radius: 6px; "
        "background-color: rgba(255,255,255,0.05); color: #070606; }"
        "QTextEdit:focus { border-color: rgba(19, 17, 17, 0.7); }"
    );
    m_layout->addWidget(m_noteEdit);

    // 颜色选择
    setupColorButtons();

    // 按钮
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    m_okButton = new QPushButton("确定", this);
    m_okButton->setStyleSheet(
        "QPushButton { padding: 8px 24px; border: none; border-radius: 6px; "
        "background-color: rgba(100, 180, 150, 0.7); color: white; font-weight: bold; }"
        "QPushButton:hover { background-color: rgba(100, 180, 150, 0.9); }"
        "QPushButton:pressed { background-color: rgba(80, 160, 130, 0.9); }"
    );

    m_cancelButton = new QPushButton("取消", this);
    m_cancelButton->setStyleSheet(
        "QPushButton { padding: 8px 24px; border: 1px solid rgba(120,120,120,0.4); border-radius: 6px; "
        "background-color: transparent; color: #b0b0b0; }"
        "QPushButton:hover { border-color: rgba(150,150,150,0.7); color: #e0e0e0; }"
    );

    btnLayout->addWidget(m_cancelButton);
    btnLayout->addWidget(m_okButton);
    m_layout->addLayout(btnLayout);

    connect(m_okButton, &QPushButton::clicked, this, &AddTodoDialog::onOkClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &AddTodoDialog::onCancelClicked);
}

void AddTodoDialog::setupColorButtons()
{
    QGroupBox *colorGroup = new QGroupBox("标记颜色", this);
    colorGroup->setStyleSheet(
        "QGroupBox { color: #b0b0b0; border: 1px solid rgba(120,120,120,0.3); border-radius: 6px; margin-top: 8px; padding-top: 8px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 8px; }"
    );

    QHBoxLayout *colorLayout = new QHBoxLayout(colorGroup);
    colorLayout->setSpacing(8);

    m_colorButtonGroup = new QButtonGroup(this);

    QList<QColor> colors = {
        QColor(255, 120, 120), QColor(255, 180, 100), QColor(255, 220, 100),
        QColor(120, 220, 120), QColor(100, 200, 255), QColor(180, 160, 255),
        QColor(255, 150, 200), QColor(200, 200, 200)
    };

    for (int i = 0; i < colors.size(); ++i) {
        QRadioButton *btn = new QRadioButton(colorGroup);
        btn->setFixedSize(22, 22);
        QString style = QString(
            "QRadioButton::indicator { width: 20px; height: 20px; border-radius: 10px; "
            "background-color: %1; border: 2px solid transparent; }"
            "QRadioButton::indicator:checked { border: 2px solid white; }"
            "QRadioButton::indicator:hover { border: 2px solid rgba(255,255,255,0.5); }"
        ).arg(colors[i].name());
        btn->setStyleSheet(style);
        colorLayout->addWidget(btn);
        m_colorButtonGroup->addButton(btn, i);
    }

    // 自定义颜色
    m_customColorButton = new QRadioButton("自定义", colorGroup);
    m_customColorButton->setStyleSheet("color: #b0b0b0;");
    colorLayout->addWidget(m_customColorButton);
    m_colorButtonGroup->addButton(m_customColorButton, colors.size());

    colorLayout->addStretch();
    m_layout->addWidget(colorGroup);

    connect(m_colorButtonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
            this, &AddTodoDialog::onColorButtonClicked);

    // 默认选中第一个
    if (m_colorButtonGroup->button(0)) {
        m_colorButtonGroup->button(0)->setChecked(true);
        m_selectedColor = colors[0];
    }
}

void AddTodoDialog::onColorButtonClicked(QAbstractButton *button)
{
    int id = m_colorButtonGroup->id(button);
    QList<QColor> colors = {
        QColor(255, 120, 120), QColor(255, 180, 100), QColor(255, 220, 100),
        QColor(120, 220, 120), QColor(100, 200, 255), QColor(180, 160, 255),
        QColor(255, 150, 200), QColor(200, 200, 200)
    };

    if (id >= 0 && id < colors.size()) {
        m_selectedColor = colors[id];
    } else if (button == m_customColorButton) {
        onCustomColorClicked();
    }
}

void AddTodoDialog::onCustomColorClicked()
{
    QColor color = QColorDialog::getColor(m_selectedColor, this, "选择颜色");
    if (color.isValid()) {
        m_selectedColor = color;
        // 更新自定义按钮的颜色
        QString style = QString(
            "QRadioButton::indicator { width: 20px; height: 20px; border-radius: 10px; "
            "background-color: %1; border: 2px solid white; }"
        ).arg(color.name());
        m_customColorButton->setStyleSheet(style + " color: #b0b0b0;");
    }
}

void AddTodoDialog::onOkClicked()
{
    if (m_titleEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入待办事项标题");
        return;
    }
    accept();
}

void AddTodoDialog::onCancelClicked()
{
    reject();
}

TodoItem AddTodoDialog::getTodoItem() const
{
    TodoItem item;
    item.setTitle(m_titleEdit->text().trimmed());
    item.setDueDate(m_dueDateEdit->dateTime());
    item.setNote(m_noteEdit->toPlainText().trimmed());
    item.setColor(m_selectedColor);
    item.setPriority(static_cast<TodoItem::Priority>(m_priorityCombo->currentData().toInt()));
    item.setCategory(m_categoryEdit->text().trimmed());
    return item;
}

void AddTodoDialog::setTodoItem(const TodoItem &item)
{
    m_titleEdit->setText(item.title());
    m_dueDateEdit->setDateTime(item.dueDate().isValid() ? item.dueDate() : QDateTime::currentDateTime().addSecs(3600));
    m_noteEdit->setPlainText(item.note());
    m_priorityCombo->setCurrentIndex(static_cast<int>(item.priority()));
    m_categoryEdit->setText(item.category());
    m_selectedColor = item.color();

    // 尝试匹配颜色按钮
    QList<QColor> colors = {
        QColor(255, 120, 120), QColor(255, 180, 100), QColor(255, 220, 100),
        QColor(120, 220, 120), QColor(100, 200, 255), QColor(180, 160, 255),
        QColor(255, 150, 200), QColor(200, 200, 200)
    };

    bool matched = false;
    for (int i = 0; i < colors.size(); ++i) {
        if (colors[i] == item.color()) {
            if (m_colorButtonGroup->button(i)) {
                m_colorButtonGroup->button(i)->setChecked(true);
                matched = true;
            }
            break;
        }
    }

    if (!matched) {
        m_customColorButton->setChecked(true);
        QString style = QString(
            "QRadioButton::indicator { width: 20px; height: 20px; border-radius: 10px; "
            "background-color: %1; border: 2px solid white; }"
        ).arg(item.color().name());
        m_customColorButton->setStyleSheet(style + " color: #b0b0b0;");
    }
}
