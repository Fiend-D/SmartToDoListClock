#include "mainwindow.h"
#include "emotionanalyzer.h"
#include "neumorphism.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QGuiApplication>
#include <QScreen>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QInputDialog>
#include <QShortcut>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_dragging(false)
    , m_resizing(false)
    , m_previewingAI(false)
    , m_locked(false)
    , m_currentLayer(LayerTop)
    , m_smartThemeEnabled(true)
    , m_weatherPoemLabel(nullptr)
    , m_raiseTimer(nullptr)
    , m_clockWidget(nullptr)
    , m_weatherWidget(nullptr)
    , m_aiService(nullptr)
    , m_config(nullptr)
    , m_trayIcon(nullptr)
    , m_quoteLabel(nullptr)
    , m_quoteEffect(nullptr)
    , m_todoWidget(nullptr)
    , m_timer(nullptr)
    , m_globalShortcut(nullptr)
    , m_smartThemeTimer(nullptr)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(220, 300);
    resize(420, 600);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect geo = screen->geometry();
    move(geo.width() - width() - 30, geo.height() - height() - 50);

    m_config = new ConfigManager(this);
    m_aiService = new AIQuoteService(
        m_config->getAPIKeyForProvider(m_config->aiProvider()),
        m_config->aiProvider(),
        this
    );

    // 读取保存的层级
    QString savedLayer = m_config->currentLayer();
    if (savedLayer == "bottom") m_currentLayer = LayerBottom;
    else if (savedLayer == "normal") m_currentLayer = LayerNormal;
    else m_currentLayer = LayerTop;

    // 读取智能主题设置
    m_smartThemeEnabled = m_config->smartThemeEnabled();

    applyWindowFlags();
    setupTray();
    setupUI();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::updateTime);
    m_timer->start(1000);
    updateTime();

    QTimer *quoteTimer = new QTimer(this);
    connect(quoteTimer, &QTimer::timeout, [this]() {
        if (m_aiService) m_aiService->fetchQuote();
    });
    quoteTimer->start(60 * 60 * 1000);

    if (m_aiService) m_aiService->fetchQuote();

    // 智能主题定时器（每10分钟检查一次）
    m_smartThemeTimer = new QTimer(this);
    connect(m_smartThemeTimer, &QTimer::timeout, this, &MainWindow::checkSmartTheme);
    m_smartThemeTimer->start(10 * 60 * 1000); // 10分钟
    QTimer::singleShot(2000, this, &MainWindow::checkSmartTheme); // 启动后2秒首次检查

    setupGlobalShortcuts();
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    central->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setCentralWidget(central);

    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setContentsMargins(20, 15, 20, 15);
    layout->setSpacing(8);
    layout->setSizeConstraint(QLayout::SetNoConstraint);

    // 信号连接
    connect(m_aiService, &AIQuoteService::quoteReceived,
            this, &MainWindow::onQuoteReceived);
    connect(m_aiService, &AIQuoteService::weatherPoemReceived,
            this, &MainWindow::onWeatherPoem);
    connect(m_aiService, &AIQuoteService::styleGenerated,
            this, &MainWindow::onStyleGenerated);
    connect(m_aiService, &AIQuoteService::networkError,
            this, [this](const QString &error) {
                QMessageBox::warning(this, "网络错误", error);
            });

    // 天气诗句
    m_weatherPoemLabel = new QLabel(this);
    m_weatherPoemLabel->setWordWrap(true);
    m_weatherPoemLabel->setAlignment(Qt::AlignCenter);
    m_weatherPoemLabel->setMinimumHeight(20);
    m_weatherPoemLabel->setMaximumHeight(50);
    m_weatherPoemLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(m_weatherPoemLabel, 0);

    // 时钟
    m_clockWidget = new ClockWidget(this);
    m_clockWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_clockWidget, 3, Qt::AlignCenter);

    // 连接番茄钟信号
    connect(m_clockWidget, &ClockWidget::pomodoroStarted, this, &MainWindow::onPomodoroStarted);
    connect(m_clockWidget, &ClockWidget::pomodoroFinished, this, &MainWindow::onPomodoroFinished);
    connect(m_clockWidget, &ClockWidget::pomodoroStopped, this, &MainWindow::onPomodoroStopped);

    // 天气温度
    m_weatherWidget = new WeatherWidget(m_config->weatherApiKey(), m_config->city(), this);
    m_weatherWidget->setMinimumHeight(25);
    m_weatherWidget->setMaximumHeight(50);
    m_weatherWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(m_weatherWidget, &WeatherWidget::weatherUpdated, this, &MainWindow::onWeatherUpdated);
    layout->addWidget(m_weatherWidget, 0);

    // AI语录
    m_quoteLabel = new QLabel(this);
    m_quoteLabel->setWordWrap(true);
    m_quoteLabel->setAlignment(Qt::AlignCenter);
    m_quoteLabel->setMinimumHeight(50);
    m_quoteLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_quoteEffect = new QGraphicsOpacityEffect(this);
    m_quoteEffect->setOpacity(1.0);
    m_quoteLabel->setGraphicsEffect(m_quoteEffect);

    layout->addWidget(m_quoteLabel, 1);

    // TodoWidget
    m_todoWidget = new TodoWidget(this);
    m_todoWidget->setMinimumHeight(40);
    m_todoWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    layout->addWidget(m_todoWidget, 0);

    connect(m_todoWidget, &TodoWidget::addButtonClicked, this, &MainWindow::onAddTodoClicked);
    connect(m_todoWidget, &TodoWidget::todoItemClicked, this, &MainWindow::onTodoItemClicked);
    connect(m_todoWidget, &TodoWidget::todoItemCompleted, this, &MainWindow::onTodoItemCompleted);
    connect(m_todoWidget, &TodoWidget::heightChanged,
            this, &MainWindow::onTodoHeightChanged,
            Qt::QueuedConnection);

    // 加载代办事项
    if (m_config) {
        QList<TodoItem> todoItems = m_config->todoItems();
        m_todoWidget->setTodoItems(todoItems);
    }

    applyStyle(m_config->currentStyle());
}

void MainWindow::applyWindowFlags()
{
    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::Tool;

    switch (m_currentLayer) {
    case LayerTop:
        flags |= Qt::WindowStaysOnTopHint;
        break;
    case LayerBottom:
        flags |= Qt::WindowStaysOnBottomHint;
        break;
    case LayerNormal:
        break;
    }

    bool wasVisible = isVisible();
    setWindowFlags(flags);
    setAttribute(Qt::WA_TranslucentBackground, true);
    if (wasVisible) show();
}

QString MainWindow::layerToString(WindowLayer layer) const
{
    switch (layer) {
    case LayerTop: return "置顶";
    case LayerNormal: return "普通";
    case LayerBottom: return "桌面底层";
    }
    return "置顶";
}

void MainWindow::setWindowLayer(const QString &layer)
{
    if (layer == "top") m_currentLayer = LayerTop;
    else if (layer == "normal") m_currentLayer = LayerNormal;
    else if (layer == "bottom") m_currentLayer = LayerBottom;

    applyWindowFlags();
    m_config->setCurrentLayer(layer);

    if (m_trayIcon) {
        m_trayIcon->showMessage("层级切换", "当前: " + layerToString(m_currentLayer));
    }
}

void MainWindow::cycleWindowLayer()
{
    switch (m_currentLayer) {
    case LayerTop: setWindowLayer("normal"); break;
    case LayerNormal: setWindowLayer("bottom"); break;
    case LayerBottom: setWindowLayer("top"); break;
    }
}

void MainWindow::toggleLock()
{
    m_locked = !m_locked;
    updateLockState();
    emit lockChanged(m_locked);
    QString status = m_locked ? "已锁定" : "已解锁";
    if (m_trayIcon) {
        m_trayIcon->showMessage("锁定状态", status);
    }
}

void MainWindow::updateLockState()
{
    if (m_locked) {
        setAttribute(Qt::WA_TransparentForMouseEvents, true);
        setWindowFlag(Qt::WindowTransparentForInput, true);
        setWindowFlag(Qt::WindowStaysOnTopHint, true);

        show();
        raise();

        if (!m_raiseTimer) {
            m_raiseTimer = new QTimer(this);
            m_raiseTimer->setInterval(50);
            connect(m_raiseTimer, &QTimer::timeout, this, [this]() {
                if (!(windowFlags() & Qt::WindowStaysOnTopHint)) {
                    setWindowFlag(Qt::WindowStaysOnTopHint, true);
                    show();
                }
                raise();
            });
        }

        if (!m_raiseTimer->isActive()) {
            m_raiseTimer->start();
        }

    } else {
        setAttribute(Qt::WA_TransparentForMouseEvents, false);
        setWindowFlag(Qt::WindowTransparentForInput, false);

        if (m_raiseTimer) {
            m_raiseTimer->stop();
            delete m_raiseTimer;
            m_raiseTimer = nullptr;
        }

        setWindowFlag(Qt::WindowStaysOnTopHint, true);
        show();
        raise();
        activateWindow();
    }

    update();
}

void MainWindow::setupTray()
{
    m_trayIcon = new QSystemTrayIcon(this);

    QMenu *menu = new QMenu(this);

    QAction *lockAction = menu->addAction("锁定", this, &MainWindow::toggleLock);
    lockAction->setCheckable(true);
    connect(this, &MainWindow::lockChanged, [lockAction](bool locked) {
        lockAction->setChecked(locked);
    });

    QMenu *layerMenu = menu->addMenu("层级");
    QActionGroup *layerGroup = new QActionGroup(this);
    layerGroup->setExclusive(true);

    QAction *topAct = layerMenu->addAction("置顶", [this]() { setWindowLayer("top"); });
    QAction *normAct = layerMenu->addAction("普通", [this]() { setWindowLayer("normal"); });
    QAction *botAct = layerMenu->addAction("底层", [this]() { setWindowLayer("bottom"); });

    layerGroup->addAction(topAct);
    layerGroup->addAction(normAct);
    layerGroup->addAction(botAct);

    topAct->setCheckable(true);
    normAct->setCheckable(true);
    botAct->setCheckable(true);

    switch (m_currentLayer) {
    case LayerTop: topAct->setChecked(true); break;
    case LayerNormal: normAct->setChecked(true); break;
    case LayerBottom: botAct->setChecked(true); break;
    }

    connect(this, &MainWindow::layerChanged, [topAct, normAct, botAct](MainWindow::WindowLayer layer) {
        topAct->setChecked(layer == MainWindow::LayerTop);
        normAct->setChecked(layer == MainWindow::LayerNormal);
        botAct->setChecked(layer == MainWindow::LayerBottom);
    });

    menu->addSeparator();

    // 智能主题开关
    QAction *smartThemeAction = menu->addAction("智能主题");
    smartThemeAction->setCheckable(true);
    smartThemeAction->setChecked(m_smartThemeEnabled);
    connect(smartThemeAction, &QAction::toggled, this, &MainWindow::onSmartThemeToggled);

    menu->addSeparator();

    QMenu *styleMenu = menu->addMenu("样式");
    styleMenu->addAction("选择样式...", this, &MainWindow::showStyleSelector);
    QMenu *aiMenu = styleMenu->addMenu("AI设计师");
    aiMenu->addAction("随机创造", this, &MainWindow::startGenerateAIStyle);
    aiMenu->addAction("描述生成...", this, &MainWindow::generateWithPrompt);

    QMenu *providerMenu = menu->addMenu("AI提供商");
    providerMenu->addAction("智谱AI", [this]() { switchProvider("zhipu"); });
    providerMenu->addAction("Kimi", [this]() { switchProvider("kimi"); });
    providerMenu->addAction("硅基流动", [this]() { switchProvider("siliconflow"); });
    providerMenu->addAction("Ollama本地", [this]() { switchProvider("ollama"); });

    menu->addSeparator();
    menu->addAction("刷新语录", [this]() { if (m_aiService) m_aiService->fetchQuote(); });
    menu->addAction("刷新天气", [this]() { if (m_weatherWidget) m_weatherWidget->updateWeather(); });

    menu->addSeparator();
    menu->addAction("显示/隐藏", this, &MainWindow::toggleVisibility);
    menu->addAction("退出", this, &QWidget::close);

    m_trayIcon->setContextMenu(menu);
    m_trayIcon->show();

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
            toggleVisibility();
        }
    });
}

void MainWindow::setupGlobalShortcuts()
{
    m_globalShortcut = new X11GlobalShortcut(this);
    connect(m_globalShortcut, &X11GlobalShortcut::activated,
            this, &MainWindow::onGlobalShortcutActivated);

    m_globalShortcut->registerShortcut(QKeySequence("Ctrl+Alt+L"), 1); // 锁定
    m_globalShortcut->registerShortcut(QKeySequence("Ctrl+Alt+T"), 2); // 层级
    m_globalShortcut->registerShortcut(QKeySequence("Ctrl+Alt+H"), 3); // 显示/隐藏
    m_globalShortcut->registerShortcut(QKeySequence("Ctrl+Alt+S"), 4); // 样式
    m_globalShortcut->registerShortcut(QKeySequence("Ctrl+Alt+R"), 5); // 刷新语录
    m_globalShortcut->registerShortcut(QKeySequence("Ctrl+Alt+Q"), 8); // 退出
}

void MainWindow::applyStyle(const ClockStyle &style, bool preview)
{
    if (!m_clockWidget || !m_quoteLabel || !m_weatherPoemLabel) return;

    m_clockWidget->setStyle(style);

    if (m_todoWidget) {
        m_todoWidget->setHourHandColor(style.hourHandColor);
        m_todoWidget->setAccentColor(style.hourHandColor);
    }

    int widgetHeight = centralWidget() ? centralWidget()->height() : height();
    int quoteFontSize = qMax(11, widgetHeight / 32);
    QString quoteSheet = QString("color: %1; font-size: %2px; padding: 6px; line-height: 1.4;")
        .arg(style.quoteColor.name())
        .arg(quoteFontSize);
    m_quoteLabel->setStyleSheet(quoteSheet);

    if (!m_quoteEffect && m_quoteLabel) {
        m_quoteEffect = new QGraphicsOpacityEffect(this);
        m_quoteEffect->setOpacity(1.0);
        m_quoteLabel->setGraphicsEffect(m_quoteEffect);
    }

    int weatherFontSize = qMax(9, widgetHeight / 38);
    QString colorName = style.markColor.isValid() ? style.markColor.name() : "#888888";
    QString weatherSheet = QString("color: %1; font-size: %2px;").arg(colorName).arg(weatherFontSize);
    m_weatherPoemLabel->setStyleSheet(weatherSheet);

    update();

    if (!preview && m_trayIcon) {
        m_trayIcon->showMessage("样式切换", style.name);
    }
}

void MainWindow::onGlobalShortcutActivated(int id)
{
    switch (id) {
    case 1: toggleLock(); break;
    case 2: cycleWindowLayer(); break;
    case 3: toggleVisibility(); break;
    case 4:
        if (!m_locked) {
            showStyleSelector();
        } else {
            toggleLock();
            QTimer::singleShot(100, this, &MainWindow::showStyleSelector);
        }
        break;
    case 5:
        if (m_aiService) m_aiService->fetchQuote();
        break;
    case 8: close(); break;
    }

    if (isVisible() && id != 3) {
        raise();
    }
}

void MainWindow::onQuoteReceived(const QString &quote, const QString &author, const QString &emotion)
{
    if (!m_quoteLabel) return;
    m_currentEmotion = emotion;
    fadeInQuote(quote + "\n— " + author);

    // 如果启用了智能主题且情绪变化明显，可以触发微调
    if (m_smartThemeEnabled && !m_previewingAI && !emotion.isEmpty()) {
        // 情绪驱动的微调可以在这里实现
        // 暂时只记录，不自动切换避免打扰
    }
}

void MainWindow::onWeatherUpdated(const QString &weather, int temp)
{
    m_currentWeather = weather;
    m_currentTemp = temp;
    if (m_aiService) m_aiService->fetchWeatherPoem(weather, temp);

    // 天气更新时，如果启用了智能主题，触发检查
    if (m_smartThemeEnabled) {
        QTimer::singleShot(1000, this, &MainWindow::checkSmartTheme);
    }
}

void MainWindow::onWeatherPoem(const QString &poem)
{
    if (m_weatherPoemLabel) {
        m_weatherPoemLabel->setText("🌤️ " + poem);
        return;
    }

    if (!m_quoteLabel) return;
    static QDateTime lastPoemTime;
    lastPoemTime = QDateTime::currentDateTime();

    if (m_quoteLabel->text().startsWith("🌤️")) return;

    QString original = m_quoteLabel->text();
    m_quoteLabel->setText("🌤️ " + poem);

    QTimer::singleShot(5000, [this, original, poemTime = lastPoemTime]() {
        if (lastPoemTime != poemTime) return;
        if (!m_previewingAI && m_quoteLabel && m_quoteLabel->text().startsWith("🌤️")) {
            m_quoteLabel->setText(original.isEmpty() ? "..." : original);
        }
    });
}

void MainWindow::onStyleGenerated(const QString &name, const QString &desc, const QJsonObject &styleData)
{
    if (styleData.isEmpty() || !styleData.contains("name")) {
        QMessageBox::warning(this, "解析失败", "AI返回的数据格式不正确，请重试");
        return;
    }

    m_pendingStyleName = name;
    m_pendingStyleDesc = desc;
    m_pendingStyleData = styleData;
    m_previewingAI = true;

    ClockStyle style = parseStyleFromJson(styleData);
    applyStyle(style, true);

    QMessageBox msg(this);
    msg.setWindowTitle("AI设计完成");
    msg.setText(QString("🎨 %1\n\n📝 %2\n\n喜欢这个风格吗？").arg(name, desc));

    QPushButton *saveBtn = msg.addButton("保存并使用", QMessageBox::AcceptRole);
    QPushButton *cancelBtn = msg.addButton("放弃", QMessageBox::RejectRole);
    QPushButton *retryBtn = msg.addButton("再来一次", QMessageBox::ResetRole);

    msg.exec();
    if (msg.clickedButton() == saveBtn) {
        saveAIStyle();
    } else if (msg.clickedButton() == retryBtn) {
        regenerateStyle();
    } else {
        rejectAIStyle();
    }
}

ClockStyle MainWindow::parseStyleFromJson(const QJsonObject &obj)
{
    ClockStyle style;
    style.name = obj["name"].toString("AI风格");
    style.description = obj["description"].toString();
    style.backgroundColor = QColor(obj["backgroundColor"].toString("#000000"));
    style.hourHandColor = QColor(obj["hourHandColor"].toString("#00ff9d"));
    style.minuteHandColor = QColor(obj["minuteHandColor"].toString("#00d4ff"));
    style.secondHandColor = QColor(obj["secondHandColor"].toString("#ff006e"));
    style.markColor = QColor(obj["markColor"].toString("#ffffff"));
    style.textColor = QColor(obj["textColor"].toString("#ffffff"));
    style.quoteColor = QColor(obj["quoteColor"].toString("#00ff9d"));
    style.fontFamily = obj["fontFamily"].toString("Noto Sans CJK SC");
    style.isDigital = obj["isDigital"].toBool(false);
    style.bgAlpha = obj["bgAlpha"].toInt(80);
    style.glowEffect = obj["glowEffect"].toBool(true);
    style.emotion = obj["emotion"].toString("calm");
    style.aiVisualScript = obj["aiVisualScript"].toString();
    return style;
}

void MainWindow::startGenerateAIStyle()
{
    if (!m_aiService) return;
    m_lastUserPrompt.clear();
    m_aiService->generateClockStyle(QString());
}

void MainWindow::generateWithPrompt()
{
    if (!m_aiService) return;

    bool ok;
    QString prompt = QInputDialog::getText(this, "AI设计师",
        "描述风格（如：赛博朋克+水墨、极简日系）：", QLineEdit::Normal, "", &ok);
    if (ok && !prompt.isEmpty()) {
        m_lastUserPrompt = prompt;
        m_aiService->generateClockStyle(prompt);
    }
}

void MainWindow::saveAIStyle()
{
    if (!m_config) return;

    QString id = "ai_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    if (m_config->addCustomStyle(id, m_pendingStyleData)) {
        applyStyle(m_config->currentStyle());
        m_previewingAI = false;
    }
}

void MainWindow::rejectAIStyle()
{
    m_previewingAI = false;
    if (m_config) applyStyle(m_config->currentStyle());
}

void MainWindow::regenerateStyle()
{
    if (!m_aiService) return;

    if (m_lastUserPrompt.isEmpty()) {
        m_aiService->generateClockStyle(QString());
    } else {
        m_aiService->generateClockStyle(m_lastUserPrompt);
    }
}

void MainWindow::showStyleSelector()
{
    if (!m_config) return;

    QStringList styles = m_config->availableStyles();
    bool ok;
    QString selected = QInputDialog::getItem(this, "选择样式", "可用样式：", styles, 0, false, &ok);
    if (ok) {
        ClockStyle style = m_config->getStyleByName(selected);
        QString id = m_config->getStyleIdByName(selected);
        m_config->setStyle(id);
        applyStyle(style);
        // 手动选择样式时，临时禁用智能主题
        m_previewingAI = false;
    }
}

void MainWindow::switchProvider(const QString &provider)
{
    QString key = m_config->getAPIKeyForProvider(provider);

    if (key.isEmpty() && provider != "ollama") {
        bool ok;
        key = QInputDialog::getText(this, "配置API Key",
            QString("请输入%1的API Key:").arg(provider),
            QLineEdit::Password, "", &ok);
        if (!ok || key.isEmpty()) return;
        m_config->setAPIKeyForProvider(provider, key);
    }

    m_config->setAIProvider(provider);
    m_aiService->setProvider(provider, key);

    QMessageBox::information(this, "切换成功",
        QString("已切换至 %1").arg(provider));

    m_aiService->fetchQuote();
}

void MainWindow::toggleVisibility()
{
    if (isVisible() && !isMinimized()) {
        hide();
    } else {
        show();
        raise();
        activateWindow();
    }
}

void MainWindow::fadeInQuote(const QString &text)
{
    if (m_quoteLabel) m_quoteLabel->setText(text);
}

// ===== 番茄钟槽函数 =====
void MainWindow::onPomodoroStarted()
{
    if (m_trayIcon) {
        m_trayIcon->showMessage("番茄钟", "专注模式已开启，祝你好运！🍅", QSystemTrayIcon::Information, 2000);
    }
}

void MainWindow::onPomodoroFinished()
{
    if (m_config) {
        m_config->incrementPomodoroCount();
    }
    if (m_trayIcon) {
        int count = m_config ? m_config->pomodoroCompletedToday() : 0;
        QString msg = QString("恭喜完成一个番茄！今日已完成 %1 个 🎉").arg(count);
        m_trayIcon->showMessage("番茄钟", msg, QSystemTrayIcon::Information, 5000);
    }
}

void MainWindow::onPomodoroStopped()
{
    if (m_trayIcon) {
        m_trayIcon->showMessage("番茄钟", "专注模式已停止", QSystemTrayIcon::Information, 1500);
    }
}

// ===== 智能主题 =====
void MainWindow::onSmartThemeToggled(bool enabled)
{
    m_smartThemeEnabled = enabled;
    if (m_config) m_config->setSmartThemeEnabled(enabled);

    if (enabled) {
        checkSmartTheme();
        if (m_trayIcon) {
            m_trayIcon->showMessage("智能主题", "已启用智能主题切换");
        }
    } else {
        if (m_trayIcon) {
            m_trayIcon->showMessage("智能主题", "已禁用，恢复手动样式");
        }
    }
}

void MainWindow::checkSmartTheme()
{
    if (!m_smartThemeEnabled || m_previewingAI) return;

    int hour = QTime::currentTime().hour();
    QString timeSlot;

    if (hour >= 6 && hour < 9) {
        timeSlot = "morning";
    } else if (hour >= 9 && hour < 17) {
        timeSlot = "daytime";
    } else if (hour >= 17 && hour < 20) {
        timeSlot = "dusk";
    } else if (hour >= 20 && hour < 23) {
        timeSlot = "evening";
    } else {
        timeSlot = "night";
    }

    // 如果时段没变，不重复切换
    if (timeSlot == m_lastTimeSlot) return;
    m_lastTimeSlot = timeSlot;

    applySmartTheme(timeSlot, m_currentWeather);
}

void MainWindow::applySmartTheme(const QString &timeSlot, const QString &weather)
{
    if (!m_config) return;

    // 内置智能主题配色（新拟态风格）
    ClockStyle style;
    style.name = timeSlot;
    style.isDigital = false;
    style.glowEffect = true;
    style.bgAlpha = 85;
    style.fontFamily = "Noto Sans CJK SC";

    if (timeSlot == "morning") {
        // 清晨：暖金+薄荷
        style.backgroundColor = QColor(45, 48, 58);
        style.hourHandColor = QColor(255, 200, 100);
        style.minuteHandColor = QColor(150, 220, 180);
        style.secondHandColor = QColor(255, 150, 120);
        style.markColor = QColor(200, 190, 170);
        style.textColor = QColor(240, 235, 220);
        style.quoteColor = QColor(255, 200, 120);
        style.emotion = "peaceful";
    } else if (timeSlot == "daytime") {
        // 白天：明亮蓝白
        style.backgroundColor = QColor(42, 50, 65);
        style.hourHandColor = QColor(100, 200, 255);
        style.minuteHandColor = QColor(120, 220, 200);
        style.secondHandColor = QColor(255, 120, 150);
        style.markColor = QColor(180, 190, 210);
        style.textColor = QColor(230, 235, 245);
        style.quoteColor = QColor(120, 200, 255);
        style.emotion = "energetic";
    } else if (timeSlot == "dusk") {
        // 黄昏：橙红暖色
        style.backgroundColor = QColor(50, 42, 48);
        style.hourHandColor = QColor(255, 160, 80);
        style.minuteHandColor = QColor(220, 140, 120);
        style.secondHandColor = QColor(255, 100, 100);
        style.markColor = QColor(200, 175, 160);
        style.textColor = QColor(245, 230, 220);
        style.quoteColor = QColor(255, 170, 90);
        style.emotion = "warm";
    } else if (timeSlot == "evening") {
        // 夜晚：深蓝紫
        style.backgroundColor = QColor(38, 40, 58);
        style.hourHandColor = QColor(160, 140, 255);
        style.minuteHandColor = QColor(140, 180, 240);
        style.secondHandColor = QColor(220, 120, 200);
        style.markColor = QColor(170, 170, 200);
        style.textColor = QColor(225, 225, 240);
        style.quoteColor = QColor(180, 160, 255);
        style.emotion = "calm";
    } else {
        // 深夜：暗色护眼
        style.backgroundColor = QColor(30, 32, 38);
        style.hourHandColor = QColor(100, 180, 160);
        style.minuteHandColor = QColor(120, 160, 180);
        style.secondHandColor = QColor(180, 100, 120);
        style.markColor = QColor(130, 135, 145);
        style.textColor = QColor(200, 205, 215);
        style.quoteColor = QColor(120, 180, 160);
        style.emotion = "quiet";
    }

    // 根据天气微调
    if (!weather.isEmpty()) {
        if (weather.contains("雨") || weather.contains("Rain")) {
            // 雨天降低饱和度，偏灰蓝
            style.markColor = Neumorphism::mix(style.markColor, QColor(140, 150, 170), 0.3);
            style.quoteColor = Neumorphism::mix(style.quoteColor, QColor(130, 160, 200), 0.2);
        } else if (weather.contains("雪") || weather.contains("Snow")) {
            // 雪天偏冷白
            style.markColor = Neumorphism::mix(style.markColor, QColor(200, 210, 230), 0.3);
            style.quoteColor = Neumorphism::mix(style.quoteColor, QColor(180, 200, 230), 0.2);
        } else if (weather.contains("晴") || weather.contains("Clear")) {
            // 晴天更明亮
            style.markColor = Neumorphism::lighter(style.markColor, 10);
        }
    }

    // 应用主题（如果当前不是AI预览模式）
    if (!m_previewingAI) {
        applyStyle(style, false);
    }
}

// ===== 绘制事件 =====
void MainWindow::paintEvent(QPaintEvent *)
{
    if (!m_config) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setCompositionMode(QPainter::CompositionMode_Source);

    // 透明背景
    painter.fillRect(rect(), Qt::transparent);

    ClockStyle style = m_config->currentStyle();
    QColor bgColor = style.backgroundColor;
    bgColor.setAlpha(style.bgAlpha);

    // 新拟态凸起背景
    Neumorphism::drawRaisedRect(&painter, rect().adjusted(4, 4, -4, -4), 24,
                                 bgColor, 0.35, 16, 8);

    // 内边框装饰线
    QColor innerBorder = Neumorphism::mix(style.markColor, bgColor, 0.7);
    innerBorder.setAlpha(40);
    painter.setPen(QPen(innerBorder, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect().adjusted(12, 12, -12, -12), 18, 18);

    // 锁定边框
    // int penWidth = m_locked ? 2 : 0;
    // if (penWidth > 0) {
    //     QColor borderColor = m_locked ? QColor(255, 100, 100) : style.hourHandColor;
    //     borderColor.setAlpha(120);
    //     painter.setPen(QPen(borderColor, penWidth));
    //     painter.setBrush(Qt::NoBrush);
    //     painter.drawRoundedRect(rect().adjusted(6, 6, -6, -6), 20, 20);
    // }

    if (m_locked) {
        painter.setPen(Qt::white);
        painter.drawText(width() - 35, 25, "🔒");
    }
}

void MainWindow::updateTime()
{
    if (m_clockWidget) m_clockWidget->setTime(QTime::currentTime());
}

// ===== 鼠标交互 =====
MainWindow::EdgeResize MainWindow::detectResizeEdge(const QPoint &pos) const
{
    const int margin = 8;
    bool left = pos.x() < margin;
    bool right = pos.x() > width() - margin;
    bool top = pos.y() < margin;
    bool bottom = pos.y() > height() - margin;

    if (top && left) return EdgeTopLeft;
    if (top && right) return EdgeTopRight;
    if (bottom && left) return EdgeBottomLeft;
    if (bottom && right) return EdgeBottomRight;
    if (left) return EdgeLeft;
    if (right) return EdgeRight;
    if (top) return EdgeTop;
    if (bottom) return EdgeBottom;
    return EdgeNone;
}

void MainWindow::updateCursor(EdgeResize edge)
{
    switch (edge) {
    case EdgeLeft:
    case EdgeRight:
        setCursor(Qt::SizeHorCursor);
        break;
    case EdgeTop:
    case EdgeBottom:
        setCursor(Qt::SizeVerCursor);
        break;
    case EdgeTopLeft:
    case EdgeBottomRight:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case EdgeTopRight:
    case EdgeBottomLeft:
        setCursor(Qt::SizeBDiagCursor);
        break;
    default:
        setCursor(Qt::ArrowCursor);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (m_locked) {
        event->ignore();
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_resizeEdge = detectResizeEdge(event->pos());
        if (m_resizeEdge != EdgeNone) {
            m_resizing = true;
            m_resizeStartPos = event->globalPosition().toPoint();
            m_resizeStartGeometry = geometry();
            event->accept();
        } else {
            m_dragging = true;
            m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_locked) {
        updateCursor(detectResizeEdge(event->pos()));
        return;
    }

    if (m_resizing) {
        QPoint delta = event->globalPosition().toPoint() - m_resizeStartPos;
        QRect geom = m_resizeStartGeometry;

        if (m_resizeEdge == EdgeLeft || m_resizeEdge == EdgeTopLeft || m_resizeEdge == EdgeBottomLeft)
            geom.setLeft(geom.left() + delta.x());
        if (m_resizeEdge == EdgeRight || m_resizeEdge == EdgeTopRight || m_resizeEdge == EdgeBottomRight)
            geom.setRight(geom.right() + delta.x());
        if (m_resizeEdge == EdgeTop || m_resizeEdge == EdgeTopLeft || m_resizeEdge == EdgeTopRight)
            geom.setTop(geom.top() + delta.y());
        if (m_resizeEdge == EdgeBottom || m_resizeEdge == EdgeBottomLeft || m_resizeEdge == EdgeBottomRight)
            geom.setBottom(geom.bottom() + delta.y());

        if (geom.width() >= 220 && geom.height() >= 300) {
            setGeometry(geom);
        }
        event->accept();
    } else if (m_dragging) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    } else {
        updateCursor(detectResizeEdge(event->pos()));
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        m_resizing = false;
        m_resizeEdge = EdgeNone;
        if (!m_locked) updateCursor(detectResizeEdge(event->pos()));
    }
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!m_locked && event->button() == Qt::LeftButton) {
        showStyleSelector();
    }
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    if (m_trayIcon && m_trayIcon->contextMenu()) {
        m_trayIcon->contextMenu()->exec(event->globalPos());
    }
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        double factor = event->angleDelta().y() > 0 ? 1.1 : 0.9;
        resize(size() * factor);
        event->accept();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    static QSize lastSize;
    if (size() != lastSize && m_config) {
        lastSize = size();
        applyStyle(m_config->currentStyle());
    }
}

// ===== Todo =====
void MainWindow::onAddTodoClicked()
{
    AddTodoDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        TodoItem item = dialog.getTodoItem();
        m_todoWidget->addTodoItem(item);
        if (m_config) {
            m_config->setTodoItems(m_todoWidget->todoItems());
        }
    }
}

void MainWindow::onTodoItemClicked(int index)
{
    QList<TodoItem> items = m_todoWidget->todoItems();
    if (index >= 0 && index < items.size()) {
        AddTodoDialog dialog(this);
        dialog.setTodoItem(items[index]);
        if (dialog.exec() == QDialog::Accepted) {
            TodoItem updatedItem = dialog.getTodoItem();
            m_todoWidget->updateTodoItem(index, updatedItem);
            if (m_config) {
                m_config->setTodoItems(m_todoWidget->todoItems());
            }
        }
    }
}

void MainWindow::onTodoItemCompleted(int index, bool completed)
{
    Q_UNUSED(index);
    Q_UNUSED(completed);
    if (m_config) {
        m_config->setTodoItems(m_todoWidget->todoItems());
    }
}

void MainWindow::onTodoHeightChanged(int newHeight)
{
    Q_UNUSED(newHeight);
    QTimer::singleShot(10, this, [this]() {
        if (m_config) {
            adjustSize();
        }
    });
}

void MainWindow::enableSmartTheme(bool enabled)
{
    m_smartThemeEnabled = enabled;
    if (m_smartThemeTimer) {
        if (enabled) {
            m_smartThemeTimer->start(300000); // 5分钟
        } else {
            m_smartThemeTimer->stop();
        }
    }
}

MainWindow::~MainWindow()
{
    if (m_smartThemeTimer) {
        m_smartThemeTimer->stop();
        delete m_smartThemeTimer;
    }
    if (m_raiseTimer) {
        m_raiseTimer->stop();
        delete m_raiseTimer;
    }
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
    }
    if (m_globalShortcut) {
        delete m_globalShortcut;
    }
    if (m_trayIcon) {
        m_trayIcon->hide();
        delete m_trayIcon;
    }
}
