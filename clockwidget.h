#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include <QWidget>
#include <QTime>
#include "configmanager.h"

class AIVisualEngine;
class QTimer;
class QPropertyAnimation;

class ClockWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal pomodoroProgress READ pomodoroProgress WRITE setPomodoroProgress)
    Q_PROPERTY(qreal glowIntensity READ glowIntensity WRITE setGlowIntensity)

public:
    enum Mode { ClockMode, PomodoroMode };

    explicit ClockWidget(QWidget *parent = nullptr);
    void setStyle(const ClockStyle &style);
    void setTime(const QTime &time);

    int heightForWidth(int w) const override { return w; }
    QSize sizeHint() const override { return QSize(300, 300); }
    QSize minimumSizeHint() const override { return QSize(120, 120); }

    Mode mode() const { return m_mode; }
    bool isPomodoroRunning() const { return m_pomodoroRunning; }

signals:
    void pomodoroStarted();
    void pomodoroPaused();
    void pomodoroStopped();
    void pomodoroFinished();
    void modeChanged(Mode newMode);

public slots:
    void togglePomodoro();
    void startPomodoro();
    void pausePomodoro();
    void stopPomodoro();
    void switchMode(Mode mode);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void drawNeumorphismAnalogClock(QPainter &painter, const QRect &rect);
    void drawNeumorphismDigitalClock(QPainter &painter, const QRect &rect);
    void drawPomodoroMode(QPainter &painter, const QRect &rect);
    void drawClockHands(QPainter &painter, const QPoint &center, int radius);
    void drawNeumorphismHand(QPainter &painter, const QPoint &center, qreal length,
                              qreal width, qreal angle, const QColor &color, bool castShadow = true);
    void drawPomodoroProgress(QPainter &painter, const QRect &rect);
    void drawModeIndicator(QPainter &painter, const QRect &rect);

    void updatePomodoro();
    qreal pomodoroProgress() const { return m_pomodoroProgress; }
    void setPomodoroProgress(qreal progress);
    qreal glowIntensity() const { return m_glowIntensity; }
    void setGlowIntensity(qreal intensity);

    QTime m_time;
    ClockStyle m_style;
    AIVisualEngine *m_visualEngine;
    QTimer *m_animTimer;
    QTimer *m_pomodoroTimer;
    qreal m_timeMs = 0;

    // 番茄钟
    Mode m_mode = ClockMode;
    bool m_pomodoroRunning = false;
    bool m_pomodoroPaused = false;
    int m_pomodoroSecondsRemaining = 0;
    int m_pomodoroTotalSeconds = 25 * 60; // 25分钟
    qreal m_pomodoroProgress = 0.0;

    // 动画
    qreal m_glowIntensity = 0.0;
    bool m_hovered = false;
    QPropertyAnimation *m_glowAnim = nullptr;

    // 完成动画
    qreal m_finishAnimationPhase = 0.0;
    bool m_showingFinishAnimation = false;
};

#endif // CLOCKWIDGET_H
