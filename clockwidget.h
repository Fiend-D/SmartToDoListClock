#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include <QWidget>
#include <QTime>
#include "configmanager.h"

// 前向声明，避免循环包含
class AIVisualEngine;
class QTimer;

class ClockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ClockWidget(QWidget *parent = nullptr);
    void setStyle(const ClockStyle &style);
    void setTime(const QTime &time);
    int heightForWidth(int w) const override { return w; }
    QSize sizeHint() const override { return QSize(300, 300); }
    QSize minimumSizeHint() const override { return QSize(100, 100); }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawAnalogClock(QPainter &painter, const QRect &rect);
    void drawDigitalClock(QPainter &painter, const QRect &rect);
    
    QTime m_time;
    ClockStyle m_style;
    AIVisualEngine *m_visualEngine;  // ✅ 添加
    QTimer *m_animTimer;              // ✅ 添加
    qreal m_timeMs = 0;               // ✅ 添加
};
#endif