#ifndef ADDTODODIALOG_H
#define ADDTODODIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QColorDialog>
#include <QButtonGroup>
#include <QRadioButton>
#include <QComboBox>
#include <QLabel>
#include "todoitem.h"

class AddTodoDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddTodoDialog(QWidget *parent = nullptr);

    TodoItem getTodoItem() const;
    void setTodoItem(const TodoItem &item);

private slots:
    void onColorButtonClicked(QAbstractButton *button);
    void onCustomColorClicked();
    void onOkClicked();
    void onCancelClicked();

private:
    void setupUI();
    void setupColorButtons();

    QVBoxLayout *m_layout;

    QLineEdit *m_titleEdit;
    QDateTimeEdit *m_dueDateEdit;
    QTextEdit *m_noteEdit;
    QComboBox *m_priorityCombo;
    QLineEdit *m_categoryEdit;

    QButtonGroup *m_colorButtonGroup;
    QRadioButton *m_customColorButton;
    QColor m_selectedColor;

    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
};

#endif // ADDTODODIALOG_H
