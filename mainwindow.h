#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QShortcut>
#include <QActionGroup>
#include "clockwidget.h"
#include "weatherwidget.h"
#include "aiquoteservice.h"
#include "configmanager.h"
#include "x11globalshortcut.h"
#include "todowidget.h"
#include "addtododialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum WindowLayer { LayerTop, LayerNormal, LayerBottom };

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

signals:
    void lockChanged(bool locked);
    void layerChanged(MainWindow::WindowLayer layer);

private slots:
    void updateTime();
    void onQuoteReceived(const QString &quote, const QString &author, const QString &emotion);
    void onWeatherUpdated(const QString &weather, int temp);
    void onWeatherPoem(const QString &poem);
    void onStyleGenerated(const QString &name, const QString &desc, const QJsonObject &styleData);

    void toggleLock();
    void updateLockState();

    void setWindowLayer(const QString &layer);
    void cycleWindowLayer();

    void startGenerateAIStyle();
    void generateWithPrompt();
    void saveAIStyle();
    void rejectAIStyle();
    void regenerateStyle();
    void showStyleSelector();
    void switchProvider(const QString &provider);

    void toggleVisibility();
    void fadeInQuote(const QString &text);

    // Todo
    void onAddTodoClicked();
    void onTodoItemClicked(int index);
    void onTodoItemCompleted(int index, bool completed);
    void onTodoHeightChanged(int newHeight);

    // 番茄钟
    void onPomodoroStarted();
    void onPomodoroFinished();
    void onPomodoroStopped();

    // 智能主题
    void checkSmartTheme();
    void applySmartTheme(const QString &timeSlot, const QString &weather);
    void enableSmartTheme(bool enabled);
    void onSmartThemeToggled(bool enabled);

private:
    void setupUI();
    void setupTray();
    void setupGlobalShortcuts();
    void applyStyle(const ClockStyle &style, bool preview = false);
    ClockStyle parseStyleFromJson(const QJsonObject &obj);
    void applyWindowFlags();
    void onGlobalShortcutActivated(int id);

    WindowLayer m_currentLayer = LayerTop;
    QString layerToString(WindowLayer layer) const;

    // 锁定状态
    bool m_locked = false;

    // 调整大小相关
    enum EdgeResize { EdgeNone, EdgeLeft, EdgeRight, EdgeTop, EdgeBottom,
                      EdgeTopLeft, EdgeTopRight, EdgeBottomLeft, EdgeBottomRight };
    EdgeResize m_resizeEdge = EdgeNone;
    bool m_resizing = false;
    QPoint m_resizeStartPos;
    QRect m_resizeStartGeometry;
    EdgeResize detectResizeEdge(const QPoint &pos) const;
    void updateCursor(EdgeResize edge);

    bool m_dragging = false;
    QPoint m_dragPosition;

    // 智能主题
    bool m_smartThemeEnabled = true;
    QTimer *m_smartThemeTimer = nullptr;
    QString m_lastTimeSlot;
    QString m_currentWeather;
    int m_currentTemp = 0;
    QString m_currentEmotion;

    // UI组件
    QLabel *m_weatherPoemLabel = nullptr;
    TodoWidget *m_todoWidget = nullptr;
    ClockWidget *m_clockWidget = nullptr;
    WeatherWidget *m_weatherWidget = nullptr;
    AIQuoteService *m_aiService = nullptr;
    ConfigManager *m_config = nullptr;
    QSystemTrayIcon *m_trayIcon = nullptr;
    QLabel *m_quoteLabel = nullptr;
    QGraphicsOpacityEffect *m_quoteEffect = nullptr;
    QTimer *m_timer = nullptr;
    QTimer *m_raiseTimer = nullptr;
    X11GlobalShortcut *m_globalShortcut = nullptr;

    // AI样式预览
    bool m_previewingAI = false;
    QJsonObject m_pendingStyleData;
    QString m_pendingStyleName;
    QString m_pendingStyleDesc;
    QString m_lastUserPrompt;
};

#endif // MAINWINDOW_H
