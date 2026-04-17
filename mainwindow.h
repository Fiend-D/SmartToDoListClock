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
    // ✅ 枚举移到 public 区域，确保 signals 能看到
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
    void layerChanged(MainWindow::WindowLayer layer);  // ✅ 使用完整类型名

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
    void onAddTodoClicked();
    void onTodoItemClicked(int index);
    void onTodoItemCompleted(int index, bool completed);
    void onTodoHeightChanged(int newHeight);

private:
    void setupUI();
    void setupTray();
    void setupGlobalShortcuts();  // ✅ 改为全局快捷键
    void applyStyle(const ClockStyle &style, bool preview = false);
    ClockStyle parseStyleFromJson(const QJsonObject &obj);
    void applyWindowFlags();
    void onGlobalShortcutActivated(int id);  // ✅ 添加
    // ✅ 删除重复定义，使用 public 区域的枚举
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
    QLabel *m_weatherPoemLabel;  // 新增
    TodoWidget *m_todoWidget;  // 新增
    
    bool m_previewingAI = false;
    QJsonObject m_pendingStyleData;
    QString m_pendingStyleName;
    QString m_pendingStyleDesc;
    QString m_lastUserPrompt;
    QTimer *m_raiseTimer = nullptr;  // 置顶守护定时器
    ClockWidget *m_clockWidget;
    WeatherWidget *m_weatherWidget;
    AIQuoteService *m_aiService;
    ConfigManager *m_config;
    QSystemTrayIcon *m_trayIcon;
    QLabel *m_quoteLabel;
    QGraphicsOpacityEffect *m_quoteEffect;
    QTimer *m_timer;
    X11GlobalShortcut *m_globalShortcut = nullptr;
};

#endif