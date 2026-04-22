#include "clockwidget.h"
#include <QPainter>
#include <QtMath>
#include <QTimer>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QDate>
#include "aivisualengine.h"
#include "neumorphism.h"

ClockWidget::ClockWidget(QWidget *parent)
    : QWidget(parent)
    , m_time(QTime::currentTime())
    , m_visualEngine(new AIVisualEngine(this))
    , m_animTimer(new QTimer(this))
    , m_pomodoroTimer(new QTimer(this))
    , m_timeMs(0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(120, 120);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setCursor(Qt::PointingHandCursor);

    connect(m_animTimer, &QTimer::timeout, this, [this]() {
        m_timeMs += 16;
        if (m_showingFinishAnimation) {
            m_finishAnimationPhase += 0.05;
            if (m_finishAnimationPhase > 1.0) {
                m_showingFinishAnimation = false;
                m_finishAnimationPhase = 0.0;
            }
        }
        update();
    });
    m_animTimer->start(16);

    connect(m_pomodoroTimer, &QTimer::timeout, this, &ClockWidget::updatePomodoro);
}

void ClockWidget::setStyle(const ClockStyle &style)
{
    m_style = style;
    if (!style.aiVisualScript.isEmpty()) {
        m_visualEngine->loadScript(style.aiVisualScript);
    } else {
        m_visualEngine->reset();
    }
    update();
}

void ClockWidget::setTime(const QTime &time)
{
    m_time = time;
    update();
}

void ClockWidget::switchMode(Mode mode)
{
    if (m_mode == mode) return;
    m_mode = mode;
    if (mode == ClockMode) {
        stopPomodoro();
    }
    emit modeChanged(mode);
    update();
}

void ClockWidget::togglePomodoro()
{
    if (m_mode != PomodoroMode) {
        switchMode(PomodoroMode);
        startPomodoro();
    } else if (m_pomodoroRunning) {
        pausePomodoro();
    } else {
        startPomodoro();
    }
}

void ClockWidget::startPomodoro()
{
    if (!m_pomodoroRunning) {
        m_pomodoroRunning = true;
        m_pomodoroPaused = false;
        if (m_pomodoroSecondsRemaining <= 0) {
            m_pomodoroSecondsRemaining = m_pomodoroTotalSeconds;
        }
        m_pomodoroTimer->start(1000);
        emit pomodoroStarted();
        update();
    }
}

void ClockWidget::pausePomodoro()
{
    if (m_pomodoroRunning) {
        m_pomodoroRunning = false;
        m_pomodoroPaused = true;
        m_pomodoroTimer->stop();
        emit pomodoroPaused();
        update();
    }
}

void ClockWidget::stopPomodoro()
{
    m_pomodoroRunning = false;
    m_pomodoroPaused = false;
    m_pomodoroSecondsRemaining = 0;
    m_pomodoroProgress = 0.0;
    m_pomodoroTimer->stop();
    emit pomodoroStopped();
    update();
}

void ClockWidget::updatePomodoro()
{
    if (m_pomodoroSecondsRemaining > 0) {
        m_pomodoroSecondsRemaining--;
        m_pomodoroProgress = 1.0 - (m_pomodoroSecondsRemaining / (qreal)m_pomodoroTotalSeconds);
        update();
    } else {
        m_pomodoroRunning = false;
        m_pomodoroTimer->stop();
        m_showingFinishAnimation = true;
        m_finishAnimationPhase = 0.0;
        emit pomodoroFinished();
        update();
    }
}

void ClockWidget::setPomodoroProgress(qreal progress)
{
    m_pomodoroProgress = qBound(0.0, progress, 1.0);
    update();
}

void ClockWidget::setGlowIntensity(qreal intensity)
{
    m_glowIntensity = intensity;
    update();
}

void ClockWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        togglePomodoro();
    } else if (event->button() == Qt::RightButton) {
        if (m_mode == PomodoroMode) {
            stopPomodoro();
            switchMode(ClockMode);
        }
    }
    QWidget::mousePressEvent(event);
}

void ClockWidget::enterEvent(QEnterEvent *event)
{
    m_hovered = true;
    if (!m_glowAnim) {
        m_glowAnim = new QPropertyAnimation(this, "glowIntensity", this);
        m_glowAnim->setDuration(300);
    }
    m_glowAnim->stop();
    m_glowAnim->setStartValue(m_glowIntensity);
    m_glowAnim->setEndValue(1.0);
    m_glowAnim->start();
    QWidget::enterEvent(event);
}

void ClockWidget::leaveEvent(QEvent *event)
{
    m_hovered = false;
    if (m_glowAnim) {
        m_glowAnim->stop();
        m_glowAnim->setStartValue(m_glowIntensity);
        m_glowAnim->setEndValue(0.0);
        m_glowAnim->start();
    }
    QWidget::leaveEvent(event);
}

void ClockWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setCompositionMode(QPainter::CompositionMode_Source);

    int diameter = qMin(width(), height()) - 20;
    if (diameter < 50) diameter = 50;

    QRect targetRect((width() - diameter) / 2, (height() - diameter) / 2, diameter, diameter);

    // AI粒子效果（在时钟下层）
    if (!m_style.aiVisualScript.isEmpty()) {
        QRect effectRect = targetRect.adjusted(-50, -50, 50, 50);
        m_visualEngine->render(&painter, effectRect, m_timeMs);
    }

    // 根据模式绘制
    if (m_mode == PomodoroMode) {
        drawPomodoroMode(painter, targetRect);
    } else if (m_style.isDigital) {
        drawNeumorphismDigitalClock(painter, targetRect);
    } else {
        drawNeumorphismAnalogClock(painter, targetRect);
    }

    // 完成动画覆盖层
    if (m_showingFinishAnimation) {
        painter.save();
        qreal alpha = 1.0 - m_finishAnimationPhase;
        QColor flashColor = m_style.hourHandColor;
        flashColor.setAlphaF(alpha * 0.4);
        painter.setBrush(flashColor);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(targetRect);

        // 扩散圆环
        int expand = qRound(m_finishAnimationPhase * diameter * 0.5);
        QColor ringColor = m_style.secondHandColor;
        ringColor.setAlphaF(alpha * 0.6);
        painter.setPen(QPen(ringColor, 3));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(targetRect.adjusted(-expand, -expand, expand, expand));
        painter.restore();
    }
}

void ClockWidget::drawNeumorphismAnalogClock(QPainter &painter, const QRect &rect)
{
    int size = rect.width();
    QPoint center = rect.center();
    int radius = size / 2 - 8;

    QColor bgColor = m_style.backgroundColor;
    int baseAlpha = m_style.bgAlpha;
    bgColor.setAlpha(baseAlpha);

    // 悬停发光效果
    if (m_hovered && m_glowIntensity > 0.01) {
        QColor glowColor = m_style.hourHandColor;
        glowColor.setAlphaF(m_glowIntensity * 0.15);
        painter.setBrush(glowColor);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(rect.adjusted(-8, -8, 8, 8));
    }

    // 绘制凹陷表盘
    Neumorphism::drawInsetEllipse(&painter, rect.adjusted(4, 4, -4, -4),
                                   bgColor, 0.35);

    // 内圈装饰环
    int innerRadius = radius - 12;
    QRect innerRect(center.x() - innerRadius, center.y() - innerRadius,
                    innerRadius * 2, innerRadius * 2);
    QColor ringColor = Neumorphism::mix(bgColor, m_style.markColor, 0.15);
    ringColor.setAlpha(60);
    painter.setPen(QPen(ringColor, 1.5));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(innerRect);

    // 刻度
    for (int i = 0; i < 60; ++i) {
        painter.save();
        painter.translate(center);
        painter.rotate(i * 6.0);

        bool isHour = (i % 5 == 0);
        int tickStart = -radius + (isHour ? 18 : 12);
        int tickEnd = -radius + (isHour ? 28 : 18);
        int tickWidth = isHour ? 3 : 1;

        QColor tickColor = isHour ? m_style.markColor : Neumorphism::mix(m_style.markColor, bgColor, 0.5);
        tickColor.setAlpha(isHour ? 200 : 100);

        // 新拟态刻度：小时刻度有微妙的凸起感
        if (isHour) {
            painter.setPen(Qt::NoPen);
            QColor shadowColor = Neumorphism::darker(bgColor, 30);
            shadowColor.setAlpha(80);
            painter.setBrush(shadowColor);
            painter.drawRoundedRect(-2, tickStart + 1, 4, tickEnd - tickStart, 2, 2);

            QColor highlightColor = Neumorphism::lighter(bgColor, 20);
            highlightColor.setAlpha(60);
            painter.setBrush(highlightColor);
            painter.drawRoundedRect(-2, tickStart - 1, 4, tickEnd - tickStart, 2, 2);
        }

        painter.setPen(QPen(tickColor, tickWidth, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(0, tickStart, 0, tickEnd);
        painter.restore();
    }

    // 数字
    painter.setPen(m_style.textColor);
    QFont font = painter.font();
    int numSize = qMax(8, size / 14);
    font.setPointSize(numSize);
    font.setBold(true);
    font.setFamily(m_style.fontFamily.isEmpty() ? "Arial" : m_style.fontFamily);
    painter.setFont(font);

    for (int i = 1; i <= 12; ++i) {
        double angle = (i - 3) * 30.0 * M_PI / 180.0;
        int r = radius - 38;
        int x = center.x() + r * cos(angle);
        int y = center.y() + r * sin(angle);
        QString num = QString::number(i);

        QFontMetrics fm(font);
        QRect textRect(x - fm.horizontalAdvance(num) / 2,
                       y - fm.height() / 2,
                       fm.horizontalAdvance(num), fm.height());

        // 数字阴影（凸起效果）
        painter.setPen(QPen(Neumorphism::darker(bgColor, 40), 1));
        painter.drawText(textRect.translated(1, 1), Qt::AlignCenter, num);

        painter.setPen(m_style.textColor);
        painter.drawText(textRect, Qt::AlignCenter, num);
    }

    // 指针
    drawClockHands(painter, center, radius - 20);

    // 中心凸起圆点
    int centerRadius = qMax(6, radius / 12);
    QRect centerRect(center.x() - centerRadius, center.y() - centerRadius,
                     centerRadius * 2, centerRadius * 2);
    Neumorphism::drawRaisedEllipse(&painter, centerRect,
                                    m_style.secondHandColor, 0.5);

    // 中心小点
    painter.setBrush(m_style.markColor);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(center.x() - 3, center.y() - 3, 6, 6);
}

void ClockWidget::drawClockHands(QPainter &painter, const QPoint &center, int radius)
{
    double hourAngle = 30.0 * (m_time.hour() % 12) + m_time.minute() / 2.0;
    double minuteAngle = 6.0 * m_time.minute() + 0.1 * m_time.second();
    double secondAngle = 6.0 * m_time.second();

    // 时针
    drawNeumorphismHand(painter, center, radius * 0.5, radius * 0.06,
                        hourAngle - 90, m_style.hourHandColor);

    // 分针
    drawNeumorphismHand(painter, center, radius * 0.72, radius * 0.04,
                        minuteAngle - 90, m_style.minuteHandColor);

    // 秒针（更纤细，带尾端平衡）
    painter.save();
    painter.translate(center);
    painter.rotate(secondAngle);

    // 秒针阴影
    painter.setPen(QPen(Neumorphism::darker(m_style.backgroundColor, 30), 2));
    painter.drawLine(1, 1, 1, -radius * 0.78 + 1);
    painter.drawLine(1, 1, 1, radius * 0.2 + 1);

    // 秒针主体
    QLinearGradient secGrad(0, radius * 0.2, 0, -radius * 0.78);
    secGrad.setColorAt(0, m_style.secondHandColor.darker(120));
    secGrad.setColorAt(0.5, m_style.secondHandColor);
    secGrad.setColorAt(1, m_style.secondHandColor.lighter(120));
    painter.setPen(QPen(secGrad, 2, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(0, radius * 0.2, 0, -radius * 0.78);

    painter.restore();
}

void ClockWidget::drawNeumorphismHand(QPainter &painter, const QPoint &center,
                                       qreal length, qreal width, qreal angle,
                                       const QColor &color, bool castShadow)
{
    painter.save();
    painter.translate(center);
    painter.rotate(angle);

    // 阴影
    if (castShadow) {
        QColor shadowColor = Neumorphism::darker(m_style.backgroundColor, 30);
        shadowColor.setAlpha(100);
        painter.setPen(Qt::NoPen);
        painter.setBrush(shadowColor);
        painter.drawRoundedRect(-width / 2 + 2, -length + 2, width, length, width / 2, width / 2);
    }

    // 主体 - 使用渐变制造立体感
    QLinearGradient grad(0, 0, 0, -length);
    grad.setColorAt(0, color.darker(110));
    grad.setColorAt(0.5, color);
    grad.setColorAt(1, color.lighter(115));

    painter.setPen(Qt::NoPen);
    painter.setBrush(grad);
    painter.drawRoundedRect(-width / 2, -length, width, length, width / 2, width / 2);

    // 高光边缘
    painter.setPen(QPen(color.lighter(130), 0.8));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(-width / 2 + 0.5, -length + 0.5, width - 1, length - 1, width / 2, width / 2);

    painter.restore();
}

void ClockWidget::drawNeumorphismDigitalClock(QPainter &painter, const QRect &rect)
{
    int hMargin = rect.width() / 15;
    int vMargin = rect.height() / 10;
    int spacing = 8;

    int contentWidth = rect.width() - 2 * hMargin;
    int contentHeight = rect.height() - 2 * vMargin;

    int dateHeight = static_cast<int>(contentHeight * 0.25);
    int timeHeight = static_cast<int>(contentHeight * 0.70);

    QRect dateRect(rect.left() + hMargin, rect.top() + vMargin, contentWidth, dateHeight);
    QRect timeRect(rect.left() + hMargin, dateRect.bottom() + spacing, contentWidth, timeHeight);

    // 凹陷显示屏背景
    QColor displayBg = Neumorphism::darker(m_style.backgroundColor, 15);
    displayBg.setAlpha(m_style.bgAlpha);
    Neumorphism::drawInsetRect(&painter, rect.adjusted(8, 8, -8, -8), 16,
                                displayBg, 0.4, 8, 3);

    // 日期
    QString dateStr;
    if (contentWidth < 300) {
        dateStr = QDate::currentDate().toString("MM-dd");
    } else if (contentWidth < 450) {
        dateStr = QDate::currentDate().toString("MM月dd日");
    } else {
        dateStr = QDate::currentDate().toString("yyyy年MM月dd日 ddd");
    }

    int dateFontSize = qMin(static_cast<int>(dateHeight * 0.65), contentWidth / 12);
    if (dateFontSize < 9) dateFontSize = 9;

    QFont dateFont = painter.font();
    dateFont.setPointSize(dateFontSize);
    dateFont.setBold(false);
    painter.setFont(dateFont);
    painter.setPen(m_style.markColor);
    painter.drawText(dateRect, Qt::AlignCenter | Qt::TextSingleLine, dateStr);

    // 时间
    QString timeStr = m_time.toString("HH:mm:ss");

    int timeFontSize = static_cast<int>(timeHeight * 0.8);
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

    // 时间凹陷文字效果
    if (m_style.glowEffect) {
        QColor shadowColor = Neumorphism::darker(displayBg, 40);
        painter.setPen(shadowColor);
        painter.drawText(timeRect.translated(1, 1), Qt::AlignCenter | Qt::TextSingleLine, timeStr);

        QColor highlightColor = Neumorphism::lighter(displayBg, 30);
        highlightColor.setAlpha(80);
        painter.setPen(highlightColor);
        painter.drawText(timeRect.translated(-1, -1), Qt::AlignCenter | Qt::TextSingleLine, timeStr);
    }

    painter.setPen(m_style.textColor);
    painter.drawText(timeRect, Qt::AlignCenter | Qt::TextSingleLine, timeStr);
}

void ClockWidget::drawPomodoroMode(QPainter &painter, const QRect &rect)
{
    int size = rect.width();
    QPoint center = rect.center();
    int radius = size / 2 - 12;

    QColor bgColor = m_style.backgroundColor;
    bgColor.setAlpha(m_style.bgAlpha);

    // 悬停发光
    if (m_hovered && m_glowIntensity > 0.01) {
        QColor glowColor = m_style.hourHandColor;
        glowColor.setAlphaF(m_glowIntensity * 0.12);
        painter.setBrush(glowColor);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(rect.adjusted(-6, -6, 6, 6));
    }

    // 外圈凸起
    Neumorphism::drawRaisedEllipse(&painter, rect.adjusted(2, 2, -2, -2),
                                    bgColor, 0.35);

    // 内圈凹陷
    int innerPad = qMax(12, radius / 8);
    QRect innerRect(rect.left() + innerPad, rect.top() + innerPad,
                    rect.width() - innerPad * 2, rect.height() - innerPad * 2);
    Neumorphism::drawInsetEllipse(&painter, innerRect,
                                   Neumorphism::darker(bgColor, 8), 0.4);

    // 绘制进度环
    drawPomodoroProgress(painter, innerRect.adjusted(8, 8, -8, -8));

    // 时间文字
    int minutes = m_pomodoroSecondsRemaining / 60;
    int seconds = m_pomodoroSecondsRemaining % 60;
    QString timeText = QString("%1:%2")
                           .arg(minutes, 2, 10, QChar('0'))
                           .arg(seconds, 2, 10, QChar('0'));

    int fontSize = qMax(16, radius / 3);
    QFont font = painter.font();
    font.setPointSize(fontSize);
    font.setBold(true);
    font.setFamily(m_style.fontFamily.isEmpty() ? "Arial" : m_style.fontFamily);
    painter.setFont(font);

    QFontMetrics fm(font);
    QRect textRect(center.x() - fm.horizontalAdvance(timeText) / 2 - 10,
                   center.y() - fm.height() / 2 - 5,
                   fm.horizontalAdvance(timeText) + 20, fm.height() + 10);

    // 文字凹陷效果
    QColor textShadow = Neumorphism::darker(bgColor, 30);
    painter.setPen(textShadow);
    painter.drawText(textRect.translated(1, 1), Qt::AlignCenter, timeText);

    QColor textHighlight = Neumorphism::lighter(bgColor, 25);
    textHighlight.setAlpha(60);
    painter.setPen(textHighlight);
    painter.drawText(textRect.translated(-1, -1), Qt::AlignCenter, timeText);

    // 主文字
    QColor mainColor = m_pomodoroRunning ? m_style.hourHandColor : m_style.markColor;
    if (m_pomodoroSecondsRemaining < 60 && m_pomodoroRunning) {
        // 最后一分钟闪烁
        if (QTime::currentTime().msec() < 500) {
            mainColor = m_style.secondHandColor;
        }
    }
    painter.setPen(mainColor);
    painter.drawText(textRect, Qt::AlignCenter, timeText);

    // 状态标签
    QString statusText;
    if (m_pomodoroRunning) {
        statusText = "专注中";
    } else if (m_pomodoroPaused) {
        statusText = "已暂停";
    } else {
        statusText = "点击开始";
    }

    QFont smallFont = painter.font();
    smallFont.setPointSize(qMax(8, fontSize / 3));
    smallFont.setBold(false);
    painter.setFont(smallFont);

    QRect statusRect(center.x() - 50, textRect.bottom() + 2, 100, 20);
    painter.setPen(Neumorphism::mix(m_style.markColor, bgColor, 0.4));
    painter.drawText(statusRect, Qt::AlignCenter, statusText);

    // 模式指示器
    drawModeIndicator(painter, rect);
}

void ClockWidget::drawPomodoroProgress(QPainter &painter, const QRect &rect)
{
    QPoint center = rect.center();
    int radius = rect.width() / 2 - 4;

    // 背景轨道（凹陷感）
    QColor trackColor = Neumorphism::darker(m_style.backgroundColor, 15);
    trackColor.setAlpha(100);
    painter.setPen(QPen(trackColor, 6, Qt::SolidLine, Qt::RoundCap));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(center.x() - radius, center.y() - radius, radius * 2, radius * 2);

    if (m_pomodoroProgress > 0) {
        // 进度渐变
        QConicalGradient progressGrad(center, 90);
        QColor startColor = m_style.hourHandColor;
        QColor endColor = m_style.minuteHandColor;
        progressGrad.setColorAt(0.0, startColor);
        progressGrad.setColorAt(m_pomodoroProgress, endColor);
        progressGrad.setColorAt(m_pomodoroProgress + 0.001, trackColor);
        progressGrad.setColorAt(1.0, trackColor);

        // 进度环阴影
        painter.setPen(QPen(Neumorphism::darker(m_style.backgroundColor, 20), 8));
        painter.drawArc(center.x() - radius, center.y() - radius,
                        radius * 2, radius * 2, 90 * 16,
                        -qRound(m_pomodoroProgress * 360 * 16));

        // 进度环主体
        painter.setPen(QPen(progressGrad, 6, Qt::SolidLine, Qt::RoundCap));
        painter.drawArc(center.x() - radius, center.y() - radius,
                        radius * 2, radius * 2, 90 * 16,
                        -qRound(m_pomodoroProgress * 360 * 16));
    }
}

void ClockWidget::drawModeIndicator(QPainter &painter, const QRect &rect)
{
    // 在右上角显示一个小圆点指示当前模式
    int dotSize = 8;
    QRect dotRect(rect.right() - dotSize * 3, rect.top() + dotSize * 2, dotSize, dotSize);

    QColor dotColor;
    if (m_mode == PomodoroMode) {
        dotColor = m_pomodoroRunning ? QColor(100, 255, 100) : QColor(255, 200, 50);
    } else {
        dotColor = m_style.markColor;
        dotColor.setAlpha(80);
    }

    Neumorphism::drawRaisedEllipse(&painter, dotRect, dotColor, 0.4);
}
