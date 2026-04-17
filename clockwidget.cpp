#include "clockwidget.h"
#include <QPainter>
#include <QtMath>
#include <QTimer>  // ✅ 添加这行
#include "aivisualengine.h"  // ✅ 添加

// clockwidget.cpp
ClockWidget::ClockWidget(QWidget *parent)
    : QWidget(parent)
    , m_time(QTime::currentTime())
    , m_visualEngine(new AIVisualEngine(this)) 
    , m_animTimer(new QTimer(this))
    , m_timeMs(0)          // ✅ 初始化)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(100, 100);
    setAttribute(Qt::WA_TranslucentBackground, true);

    connect(m_animTimer, &QTimer::timeout, this, [this]() {
        m_timeMs += 16;
        update();
    });
    m_animTimer->start(16);
}

void ClockWidget::setStyle(const ClockStyle &style)
{
    qDebug() << "ClockWidget::setStyle 开始";
    m_style = style;
    qDebug() << "ClockWidget::setStyle 被调用";
    qDebug() << "aiVisualScript 长度:" << style.aiVisualScript.length();
    qDebug() << "aiVisualScript 内容:" << style.aiVisualScript.left(200);
    // ✅ 加载AI视觉脚本
    if (!style.aiVisualScript.isEmpty()) {
        qDebug() << "调用 m_visualEngine->loadScript";
        m_visualEngine->loadScript(style.aiVisualScript);
        qDebug() << "m_visualEngine->loadScript 调用完成";
    } else {
        qDebug() << "调用 m_visualEngine->reset";
        m_visualEngine->reset();
        qDebug() << "m_visualEngine->reset 调用完成";
    }
    
    qDebug() << "调用 update()";
    update();
    qDebug() << "ClockWidget::setStyle 结束";
}

void ClockWidget::setTime(const QTime &time)
{
    m_time = time;
    update();
}

void ClockWidget::paintEvent(QPaintEvent *)
{
    qDebug() << "ClockWidget::paintEvent 开始";
    // qDebug() << "paintEvent: aiVisualScript 为空?" << m_style.aiVisualScript.isEmpty();
    QPainter painter(this);
    qDebug() << "设置painter属性";
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    // painter.fillRect(rect(), Qt::transparent);
    
    qDebug() << "计算时钟直径";
    int diameter = qMin(width(), height()) - 20;
    if (diameter < 50) diameter = 50;
    // 移除最大直径限制，允许时钟无限放大
    
    qDebug() << "计算目标矩形";
    QRect targetRect((width() - diameter) / 2, (height() - diameter) / 2, diameter, diameter);
    
    // 先绘制AI粒子效果（在时钟下层）
    if (!m_style.aiVisualScript.isEmpty()) {
        qDebug() << "绘制AI粒子效果";
        // 扩大区域让粒子可以落在时钟周围4
        QRect effectRect = targetRect.adjusted(-50, -50, 50, 50);
        qDebug() << "特效区域:" << effectRect;

        qDebug() << "调用 m_visualEngine->render";
        m_visualEngine->render(&painter, effectRect, m_timeMs);
        qDebug() << "m_visualEngine->render 调用完成";
    } else {
        qDebug() << "aiVisualScript 为空，跳过粒子效果";
    }
    
    // 再绘制时钟
    qDebug() << "绘制时钟";
    if (m_style.isDigital) {
        qDebug() << "绘制数字时钟";
        drawDigitalClock(painter, targetRect);
    } else {
        qDebug() << "绘制模拟时钟";
        drawAnalogClock(painter, targetRect);
    }
    qDebug() << "ClockWidget::paintEvent 结束";
}


void ClockWidget::drawAnalogClock(QPainter &painter, const QRect &rect)
{
    int size = rect.width();
    QPoint center = rect.center();
    int radius = size / 2 - 5;
    
    // // 背景
    QColor bgColor = m_style.backgroundColor;
    bgColor.setAlpha(m_style.bgAlpha);  // 应用透明度
    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(center, radius, radius);
    
    // 发光边框
    if (m_style.glowEffect) {
        QPen glowPen(m_style.markColor, 3);
        glowPen.setStyle(Qt::SolidLine);
        painter.setPen(glowPen);
        painter.drawEllipse(center, radius + 2, radius + 2);
    }
    
    // 刻度...（保持不变）
    for (int i = 0; i < 60; ++i) {
        painter.save();
        painter.translate(center);
        painter.rotate(i * 6.0);
        if (i % 5 == 0) {
            painter.setPen(QPen(m_style.markColor, 3));
            painter.drawLine(0, -radius + 5, 0, -radius + 20);
        } else {
            painter.setPen(QPen(m_style.markColor, 1));
            painter.drawLine(0, -radius + 5, 0, -radius + 12);
        }
        painter.restore();
    }
    
    // 数字...（保持不变）
    painter.setPen(m_style.textColor);
    QFont font = painter.font();
    font.setPointSize(qMax(8, size / 15));
    font.setBold(true);
    painter.setFont(font);
    for (int i = 1; i <= 12; ++i) {
        double angle = (i - 3) * 30.0 * M_PI / 180.0;
        int r = radius - 30;
        int x = center.x() + r * cos(angle);
        int y = center.y() + r * sin(angle);
        QString num = QString::number(i);
        QRect textRect(x - 15, y - 10, 30, 20);
        painter.drawText(textRect, Qt::AlignCenter, num);
    }
    
    // 指针...（保持不变）
    double hourAngle = 30.0 * (m_time.hour() % 12) + m_time.minute() / 2.0;
    double minuteAngle = 6.0 * m_time.minute() + 0.1 * m_time.second();
    double secondAngle = 6.0 * m_time.second();
    
    painter.save();
    painter.translate(center);
    painter.rotate(hourAngle);
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_style.hourHandColor);
    painter.drawRoundedRect(-radius/15, -radius*0.5, radius/7.5, radius*0.5, 4, 4);
    painter.restore();
    
    painter.save();
    painter.translate(center);
    painter.rotate(minuteAngle);
    painter.setBrush(m_style.minuteHandColor);
    painter.drawRoundedRect(-radius/20, -radius*0.7, radius/10, radius*0.7, 3, 3);
    painter.restore();
    
    painter.save();
    painter.translate(center);
    painter.rotate(secondAngle);
    QLinearGradient grad(0, 0, 0, -radius*0.8);
    grad.setColorAt(0, m_style.secondHandColor);
    grad.setColorAt(1, Qt::transparent);
    painter.setBrush(grad);
    painter.drawRoundedRect(-radius/40, -radius*0.8, radius/20, radius*0.8, 1, 1);
    painter.restore();
    
    painter.setBrush(m_style.secondHandColor);
    painter.drawEllipse(center, radius/12, radius/12);
}

void ClockWidget::drawDigitalClock(QPainter &painter, const QRect &rect)
{
    // ✅ 修复布局：水平边距加大，垂直分区更合理
    int hMargin = rect.width() / 15;   // 水平边距（防止左右截断）
    int vMargin = rect.height() / 10;  // 垂直边距
    int spacing = 8;                    // 时间与日期间距
    
    int contentWidth = rect.width() - 2 * hMargin;
    int contentHeight = rect.height() - 2 * vMargin;
    
    // 日期在上（占25%），时间在下（占75%）—— 时间更重要
    int dateHeight = static_cast<int>(contentHeight * 0.25);
    int timeHeight = static_cast<int>(contentHeight * 0.70); // 留5%余量
    
    QRect dateRect(
        rect.left() + hMargin,
        rect.top() + vMargin,
        contentWidth,
        dateHeight
    );
    
    QRect timeRect(
        rect.left() + hMargin,
        dateRect.bottom() + spacing,
        contentWidth,
        timeHeight
    );
    
    // ===== 先画日期（简化格式避免过长） =====
    QString dateStr;
    if (contentWidth < 300) {
        dateStr = QDate::currentDate().toString("MM-dd");
    } else if (contentWidth < 450) {
        dateStr = QDate::currentDate().toString("MM月dd日");
    } else {
        dateStr = QDate::currentDate().toString("yyyy年MM月dd日 ddd");
    }
    
    int dateFontSize = qMin(static_cast<int>(dateHeight * 0.7), contentWidth / 12);
    if (dateFontSize < 9) dateFontSize = 9;
    // 移除最大字体大小限制，允许文字随时钟大小变化
    
    QFont dateFont = painter.font();
    dateFont.setPointSize(dateFontSize);
    dateFont.setBold(false);
    painter.setFont(dateFont);
    painter.setPen(m_style.markColor);
    painter.drawText(dateRect, Qt::AlignCenter | Qt::TextSingleLine, dateStr);
    
    // ===== 再画时间 =====
    QString timeStr = m_time.toString("HH:mm:ss");
    
    int timeFontSize = static_cast<int>(timeHeight * 0.85);
    if (timeFontSize < 20) timeFontSize = 20;
    
    QFont timeFont = painter.font();
    timeFont.setPointSize(timeFontSize);
    timeFont.setBold(true);
    timeFont.setFamily(m_style.fontFamily.isEmpty() ? "Arial" : m_style.fontFamily);
    painter.setFont(timeFont);
    
    QFontMetrics fm(painter.font());
    int textWidth = fm.horizontalAdvance(timeStr);
    
    if (textWidth > contentWidth - 10) {
        qreal scale = (contentWidth - 10.0) / textWidth;
        timeFontSize = static_cast<int>(timeFontSize * scale * 0.95);
        timeFont.setPointSize(timeFontSize);
        painter.setFont(timeFont);
    }
    
    if (m_style.glowEffect) {
        QColor glowColor = m_style.hourHandColor;
        glowColor.setAlpha(60);
        painter.setPen(glowColor);
        painter.drawText(timeRect.translated(2, 2), Qt::AlignCenter | Qt::TextSingleLine, timeStr);
    }
    
    painter.setPen(m_style.textColor);
    painter.drawText(timeRect, Qt::AlignCenter | Qt::TextSingleLine, timeStr);
}