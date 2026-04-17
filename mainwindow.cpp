#include "mainwindow.h"
#include "emotionanalyzer.h"
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_dragging(false)
    , m_resizing(false)
    , m_previewingAI(false)
    , m_locked(false)
    , m_currentLayer(LayerTop)
    , m_resizeEdge(EdgeNone)
    , m_weatherPoemLabel(nullptr)
    , m_raiseTimer(nullptr)
    , m_clockWidget(nullptr)
    , m_weatherWidget(nullptr)
    , m_aiService(nullptr)
    , m_config(nullptr)
    , m_trayIcon(nullptr)
    , m_quoteLabel(nullptr)
    , m_quoteEffect(nullptr)
    , m_todoWidget(nullptr)        // ✅ 确保这行也存在
    , m_timer(nullptr)
    , m_globalShortcut(nullptr)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // 设置大小策略，允许窗口大小变化
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(200, 250);  // 减小最小尺寸限制
    resize(400, 550);
    
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
    
    applyWindowFlags();
    setupTray();
    setupUI();
    qDebug() << "MainWindow: 开始";
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::updateTime);
    m_timer->start(1000);
    updateTime();
    qDebug() << "MainWindow: updateTime 调用完成";
    QTimer *quoteTimer = new QTimer(this);
    connect(quoteTimer, &QTimer::timeout, [this]() { 
        if (m_aiService) m_aiService->fetchQuote(); 
    });
    qDebug() << "MainWindow: quoteTimer 开始";
    quoteTimer->start(60 * 60 * 1000);
    
    if (m_aiService) m_aiService->fetchQuote();
    qDebug() << "MainWindow: fetchQuote 调用完成";
    setupGlobalShortcuts();
    qDebug() << "MainWindow: 开始";
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    central->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setCentralWidget(central);
    
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setContentsMargins(15, 0, 15, 10);  // 减小底部边距
    layout->setSpacing(3);  // 减小控件间距，更紧凑
    layout->setSizeConstraint(QLayout::SetNoConstraint);  // 允许布局自由调整大小
    
    // 天气诗句
    m_weatherPoemLabel = new QLabel(this);
    if(m_weatherPoemLabel){
        m_weatherPoemLabel->setWordWrap(true);
        m_weatherPoemLabel->setAlignment(Qt::AlignCenter);
        m_weatherPoemLabel->setMinimumHeight(25);
        m_weatherPoemLabel->setMaximumHeight(60);
        m_weatherPoemLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        layout->addWidget(m_weatherPoemLabel, 0);
    }
    
    // 信号连接（建议移到构造函数，这里也可以）
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
    
    // 时钟
    m_clockWidget = new ClockWidget(this);
    m_clockWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_clockWidget, 2, Qt::AlignCenter);  // 增加拉伸因子，使时钟占据更多空间
    
    // 天气温度
    m_weatherWidget = new WeatherWidget(m_config->weatherApiKey(), m_config->city(), this);
    m_weatherWidget->setMinimumHeight(25);
    m_weatherWidget->setMaximumHeight(60);
    m_weatherWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(m_weatherWidget, &WeatherWidget::weatherUpdated, this, &MainWindow::onWeatherUpdated);
    layout->addWidget(m_weatherWidget, 0);
    
    // AI语录
    m_quoteLabel = new QLabel(this);
    m_quoteLabel->setWordWrap(true);
    m_quoteLabel->setAlignment(Qt::AlignCenter);
    m_quoteLabel->setMinimumHeight(60);
    m_quoteLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    m_quoteEffect = new QGraphicsOpacityEffect(this);
    m_quoteEffect->setOpacity(1.0);
    m_quoteLabel->setGraphicsEffect(m_quoteEffect);
    
    layout->addWidget(m_quoteLabel, 1);  // 增加拉伸因子，使语录区域适度拉伸
    
    // 添加TodoWidget
    m_todoWidget = new TodoWidget(this);
    m_todoWidget->setMinimumHeight(40);
    m_todoWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    layout->addWidget(m_todoWidget, 0);  // 固定高度，不拉伸
    
    // 连接TodoWidget的信号
    connect(m_todoWidget, &TodoWidget::addButtonClicked, this, &MainWindow::onAddTodoClicked);
    connect(m_todoWidget, &TodoWidget::todoItemClicked, this, &MainWindow::onTodoItemClicked);
    connect(m_todoWidget, &TodoWidget::todoItemCompleted, this, &MainWindow::onTodoItemCompleted);
    connect(m_todoWidget, &TodoWidget::heightChanged, 
            this, &MainWindow::onTodoHeightChanged, 
            Qt::QueuedConnection);  // ✅ 异步连接，打破递归链
    
    // 加载代办事项
    if (m_config) {
        QList<TodoItem> todoItems = m_config->todoItems();
        m_todoWidget->setTodoItems(todoItems);
    }
    
    // 可选：如果你希望底部留白，把内容往上顶，取消下面这行注释
    // layout->addStretch(1);
    
    applyStyle(m_config->currentStyle());
    qDebug() << "MainWindow: applyStyle 调用完成";
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
    emit lockChanged(m_locked);  // 添加这行
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
        
        // ✅ 确保只有一个定时器实例，缩短到 50ms（20fps，人眼无感知但响应更快）
        if (!m_raiseTimer) {
            m_raiseTimer = new QTimer(this);
            m_raiseTimer->setInterval(50);  // 原来是 100ms，现在 50ms
            connect(m_raiseTimer, &QTimer::timeout, this, [this]() {
                // 只在窗口失去置顶属性时才 raise，减少无效调用
                if (!(windowFlags() & Qt::WindowStaysOnTopHint)) {
                    setWindowFlag(Qt::WindowStaysOnTopHint, true);
                    show();
                }
                raise();
            });
        }
        
        // ✅ 防止重复启动
        if (!m_raiseTimer->isActive()) {
            m_raiseTimer->start();
        }
        
    } else {
        setAttribute(Qt::WA_TransparentForMouseEvents, false);
        setWindowFlag(Qt::WindowTransparentForInput, false);
        
        // ✅ 完全停止并删除定时器（避免残留）
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
    
    QAction *lockAction = menu->addAction("锁定🔒[L]", this, &MainWindow::toggleLock);
    lockAction->setCheckable(true);
    connect(this, &MainWindow::lockChanged, [lockAction](bool locked) {
        lockAction->setChecked(locked);
    });
    
    QMenu *layerMenu = menu->addMenu("层级📌[T]");
    
    // ✅ 使用 QActionGroup 实现单选互斥
    QActionGroup *layerGroup = new QActionGroup(this);
    layerGroup->setExclusive(true);
    
    QAction *topAct = layerMenu->addAction("[顶] 置顶", [this]() { setWindowLayer("top"); });
    QAction *normAct = layerMenu->addAction("[普] 普通", [this]() { setWindowLayer("normal"); });
    QAction *botAct = layerMenu->addAction("[底] 底层", [this]() { setWindowLayer("bottom"); });
    
    // 加入互斥组
    layerGroup->addAction(topAct);
    layerGroup->addAction(normAct);
    layerGroup->addAction(botAct);
    
    // 都设为可选中
    topAct->setCheckable(true);
    normAct->setCheckable(true);
    botAct->setCheckable(true);
    
    // 设置当前选中
    switch (m_currentLayer) {
    case LayerTop: topAct->setChecked(true); break;
    case LayerNormal: normAct->setChecked(true); break;
    case LayerBottom: botAct->setChecked(true); break;
    }
    
    // ✅ 关键：切换时更新选中状态（如果不使用 QActionGroup 的自动互斥）
    connect(this, &MainWindow::layerChanged, [topAct, normAct, botAct](MainWindow::WindowLayer layer) {
        topAct->setChecked(layer == MainWindow::LayerTop);
        normAct->setChecked(layer == MainWindow::LayerNormal);
        botAct->setChecked(layer == MainWindow::LayerBottom);
    });
    
    menu->addSeparator();
    
    QMenu *styleMenu = menu->addMenu("样式🎨[S]");
    styleMenu->addAction("选择样式...", this, &MainWindow::showStyleSelector);
    QMenu *aiMenu = styleMenu->addMenu("AI设计师🤖");
    aiMenu->addAction("随机创造🎲", this, &MainWindow::startGenerateAIStyle);
    aiMenu->addAction("描述生成📝...", this, &MainWindow::generateWithPrompt);
    // 4. AI提供商（原有）
    QMenu *providerMenu = menu->addMenu("🤖 AI提供商");
    providerMenu->addAction("智谱AI", [this]() { switchProvider("zhipu"); });
    providerMenu->addAction("Kimi", [this]() { switchProvider("kimi"); });
    providerMenu->addAction("硅基流动", [this]() { switchProvider("siliconflow"); });
    providerMenu->addAction("Ollama本地", [this]() { switchProvider("ollama"); });

    menu->addSeparator();
    menu->addAction("刷新语录🔄[R]", [this]() { if (m_aiService) m_aiService->fetchQuote(); });
    menu->addAction("刷新天气🔄", [this]() { if (m_weatherWidget) m_weatherWidget->updateWeather(); });
    
    menu->addSeparator();
    menu->addAction("显示/隐藏👁️[H]", this, &MainWindow::toggleVisibility);
    menu->addAction("退出❌[Q]", this, &QWidget::close);
    
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
    qDebug() << "setupGlobalShortcuts: 开始";
    m_globalShortcut = new X11GlobalShortcut(this);
    qDebug() << "setupGlobalShortcuts: 创建 X11GlobalShortcut 完成";
    
    // 连接信号
    connect(m_globalShortcut, &X11GlobalShortcut::activated, 
            this, &MainWindow::onGlobalShortcutActivated);
    qDebug() << "setupGlobalShortcuts: 连接信号完成";
    
    // 注册所有快捷键
    // L - 锁定/解锁
    qDebug() << "setupGlobalShortcuts: 注册快捷键 Ctrl+Alt+L";
    m_globalShortcut->registerShortcut(QKeySequence("Ctrl+Alt+L"), 1);
    // T - 切换层级
    qDebug() << "setupGlobalShortcuts: 注册快捷键 Ctrl+Alt+T";
    m_globalShortcut->registerShortcut(QKeySequence("Ctrl+Alt+T"), 2);
    // H - 显示/隐藏
    qDebug() << "setupGlobalShortcuts: 注册快捷键 Ctrl+Alt+H";
    m_globalShortcut->registerShortcut(QKeySequence("Ctrl+Alt+H"), 3);
    // S - 样式选择
    qDebug() << "setupGlobalShortcuts: 注册快捷键 Ctrl+Alt+S";
    m_globalShortcut->registerShortcut(QKeySequence("Ctrl+Alt+S"), 4);
    // R - 刷新语录
    qDebug() << "setupGlobalShortcuts: 注册快捷键 Ctrl+Alt+R";
    m_globalShortcut->registerShortcut(QKeySequence("Ctrl+Alt+R"), 5);
    // // Ctrl++ - 放大
    // m_globalShortcut->registerShortcut(QKeySequence("Ctrl+Equal"), 6);  // 主键盘 +
    // m_globalShortcut->registerShortcut(QKeySequence("Ctrl+Plus"), 6);   // 小键盘 +
    // // Ctrl+- - 缩小
    // m_globalShortcut->registerShortcut(QKeySequence("Ctrl+Minus"), 7);
    // Q - 退出
    qDebug() << "setupGlobalShortcuts: 注册快捷键 Ctrl+Alt+Q";
    m_globalShortcut->registerShortcut(QKeySequence("Ctrl+Alt+Q"), 8);
    qDebug() << "setupGlobalShortcuts: 结束";
}

void MainWindow::applyStyle(const ClockStyle &style, bool preview)
{
    qDebug() << "applyStyle: 开始";
    // ✅ 检查所有需要的控件
    if (!m_clockWidget || !m_quoteLabel || !m_weatherPoemLabel) {
        qDebug() << "applyStyle: 控件未初始化，跳过应用样式";
        return;
    }
    qDebug() << "applyStyle: 控件检查完成";
    
    // ✅ 安全调用 setStyle
    qDebug() << "applyStyle: 调用 m_clockWidget->setStyle";
    m_clockWidget->setStyle(style);
    qDebug() << "applyStyle: m_clockWidget->setStyle 调用完成";
    
    // 更新TodoWidget的时针颜色
    if (m_todoWidget) {
        qDebug() << "applyStyle: 调用 m_todoWidget->setHourHandColor";
        m_todoWidget->setHourHandColor(style.hourHandColor);
        qDebug() << "applyStyle: m_todoWidget->setHourHandColor 调用完成";
    }
    
    // AI语录样式 - ✅ 每次都创建新的 effect，避免野指针
    qDebug() << "applyStyle: 获取 widgetHeight";
    int widgetHeight = centralWidget() ? centralWidget()->height() : height();
    qDebug() << "applyStyle: widgetHeight:" << widgetHeight;
    int quoteFontSize = qMax(12, widgetHeight / 35);
    qDebug() << "applyStyle: quoteFontSize:" << quoteFontSize;
    qDebug() << "applyStyle: style.quoteColor.isValid():" << style.quoteColor.isValid();
    QString quoteSheet = QString("color: %1; font-size: %2px; padding: 5px;")
        .arg(style.quoteColor.name())
        .arg(quoteFontSize);
    qDebug() << "applyStyle: quoteSheet:" << quoteSheet;
    qDebug() << "applyStyle: 调用 m_quoteLabel->setStyleSheet";
    m_quoteLabel->setStyleSheet(quoteSheet);
    qDebug() << "applyStyle: m_quoteLabel->setStyleSheet 调用完成";
    
    // ✅ 关键修改：只创建一次 m_quoteEffect，避免重复删除和重建
    qDebug() << "applyStyle: 处理 m_quoteEffect";
    if (!m_quoteEffect && m_quoteLabel) {
        qDebug() << "applyStyle: 创建新的 m_quoteEffect";
        m_quoteEffect = new QGraphicsOpacityEffect(this);
        qDebug() << "applyStyle: 新的 m_quoteEffect 创建完成，地址:" << m_quoteEffect;
        m_quoteEffect->setOpacity(1.0);
        qDebug() << "applyStyle: 调用 m_quoteLabel->setGraphicsEffect";
        m_quoteLabel->setGraphicsEffect(m_quoteEffect);
        qDebug() << "applyStyle: m_quoteLabel->setGraphicsEffect 调用完成";
    }
    
    // 天气名言样式
    qDebug() << "applyStyle: 处理天气名言样式";
    int weatherFontSize = qMax(10, widgetHeight / 40);
    qDebug() << "applyStyle: weatherFontSize:" << weatherFontSize;
    qDebug() << "applyStyle: style.markColor.isValid():" << style.markColor.isValid();
    QString colorName = style.markColor.isValid() ? style.markColor.name() : "#888888";
    qDebug() << "applyStyle: colorName:" << colorName;
    QString weatherSheet = QString("color: %1; font-size: %2px;").arg(colorName).arg(weatherFontSize);
    qDebug() << "applyStyle: weatherSheet:" << weatherSheet;
    qDebug() << "applyStyle: 调用 m_weatherPoemLabel->setStyleSheet";
    m_weatherPoemLabel->setStyleSheet(weatherSheet);
    qDebug() << "applyStyle: m_weatherPoemLabel->setStyleSheet 调用完成";
    
    qDebug() << "applyStyle: 调用 update()";
    update();
    qDebug() << "applyStyle: update() 调用完成";
    
    if (!preview && m_trayIcon) {
        qDebug() << "applyStyle: 调用 m_trayIcon->showMessage";
        m_trayIcon->showMessage("样式切换", style.name);
        qDebug() << "applyStyle: m_trayIcon->showMessage 调用完成";
    }
    qDebug() << "applyStyle: 结束";
}

void MainWindow::onGlobalShortcutActivated(int id)
{
    // 如果窗口隐藏，先显示（用于显示/隐藏快捷键的特殊处理）
    switch (id) {
    case 1: // L - 锁定/解锁
        toggleLock();
        break;
    case 2: // T - 切换层级
        cycleWindowLayer();
        break;
    case 3: // H - 显示/隐藏
        toggleVisibility();
        break;
    case 4: // S - 样式选择
        // 解锁状态下才允许选择样式
        if (!m_locked) {
            showStyleSelector();
        } else {
            // 锁定时先解锁再选择，或提示用户
            toggleLock();  // 先解锁
            QTimer::singleShot(100, this, &MainWindow::showStyleSelector);
        }
        break;
    case 5: // R - 刷新语录
        if (m_aiService) m_aiService->fetchQuote();
        break;
    case 6: // Ctrl++ - 放大
        resize(size() * 1.1);
        break;
    case 7: // Ctrl+- - 缩小
        resize(size() * 0.9);
        break;
    case 8: // Q - 退出
        close();
        break;
    }
    
    // 快捷键触发后，确保窗口显示在最前（如果是显示状态）
    if (isVisible() && id != 3) {  // 显示/隐藏快捷键不需要额外处理
        raise();
    }
}

void MainWindow::onQuoteReceived(const QString &quote, const QString &author, const QString &emotion)
{
    if (!m_quoteLabel) return;
    
    fadeInQuote(quote + "\n— " + author);
    
    // if (m_config && m_config->currentStyle().emotion != emotion) {
    //     QString matched = m_config->findStyleForEmotion(emotion);
    //     if (!matched.isEmpty()) {
    //         int ret = QMessageBox::question(this, "情绪匹配", 
    //             QString("检测到%1情绪，是否切换？").arg(emotion));
    //         if (ret == QMessageBox::Yes) {
    //             applyStyle(m_config->getStyle(matched));
    //         }
    //     }
    // }
}

void MainWindow::onWeatherUpdated(const QString &weather, int temp)
{
    if (m_aiService) m_aiService->fetchWeatherPoem(weather, temp);
}

void MainWindow::onWeatherPoem(const QString &poem)
{
    // 优先使用独立的天气标签
    if (m_weatherPoemLabel) {
        m_weatherPoemLabel->setText("🌤️ " + poem);
        return;
    }
    
    // 兼容旧版本：共用 quoteLabel
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
    // 检查解析是否成功（如果 styleData 为空对象，说明解析失败了）
    if (styleData.isEmpty() || !styleData.contains("name")) {
        QMessageBox::warning(this, "解析失败", "AI返回的数据格式不正确，请重试");
        return;
    }
    // 检查是否有 aiVisualScript
    if (styleData.contains("aiVisualScript")) {
        qDebug() << "视觉脚本:" << styleData["aiVisualScript"].toString();
    } else {
        qDebug() << "警告: 没有 aiVisualScript 字段!";
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
    msg.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Retry);
    // 新代码（推荐）：
    QPushButton *saveBtn = msg.addButton("保存并使用", QMessageBox::AcceptRole);
    QPushButton *cancelBtn = msg.addButton("放弃", QMessageBox::RejectRole);
    QPushButton *retryBtn = msg.addButton("再来一次", QMessageBox::ResetRole);
    
    // 然后检查返回值：
    int ret = msg.exec();
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
    style.aiVisualScript = obj["aiVisualScript"].toString();  // ✅ 添加这行！
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

void MainWindow::paintEvent(QPaintEvent *)
{
    if (!m_config) return;
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 关键：设置合成模式，确保透明区域真正透明
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    
    // 先填充整个窗口为透明
    painter.fillRect(rect(), Qt::transparent);
    
    // 再绘制圆角背景
    ClockStyle style = m_config->currentStyle();
    QColor bgColor = QColor(0, 0, 0, style.bgAlpha);
    
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 20, 20);
    
    // 边框（可选）
    int penWidth = m_locked ? 2 : 0;
    if (penWidth > 0) {
        QColor borderColor = m_locked ? QColor(255, 100, 100) : style.hourHandColor;
        painter.setPen(QPen(borderColor, penWidth));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 20, 20);
    }
    
    if (m_locked) {
        painter.setPen(Qt::white);
        painter.drawText(width() - 30, 20, "🔒");
    }
}

void MainWindow::updateTime()
{
    if (m_clockWidget) m_clockWidget->setTime(QTime::currentTime());
}

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
        
        if (geom.width() >= 300 && geom.height() >= 400) {
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
    // 窗口大小变化时重新应用样式，使字体大小能够随窗口大小变化而变化
    // 避免无限递归，只在窗口大小真正变化时才应用样式
    static QSize lastSize;
    if (size() != lastSize && m_config) {
        lastSize = size();
        applyStyle(m_config->currentStyle());
    }
}

void MainWindow::onAddTodoClicked()
{
    AddTodoDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        TodoItem item = dialog.getTodoItem();
        m_todoWidget->addTodoItem(item);
        // 保存到配置
        if (m_config) {
            m_config->setTodoItems(m_todoWidget->todoItems());
        }
    }
}

void MainWindow::onTodoItemClicked(int index)
{
    // 编辑代办事项
    QList<TodoItem> items = m_todoWidget->todoItems();
    if (index >= 0 && index < items.size()) {
        AddTodoDialog dialog(this);
        dialog.setTodoItem(items[index]);
        if (dialog.exec() == QDialog::Accepted) {
            TodoItem updatedItem = dialog.getTodoItem();
            m_todoWidget->updateTodoItem(index, updatedItem);
            // 保存到配置
            if (m_config) {
                m_config->setTodoItems(m_todoWidget->todoItems());
            }
        }
    }
}

void MainWindow::onTodoItemCompleted(int index, bool completed)
{
    // 可以在这里添加完成代办事项的逻辑，比如播放提示音等
    Q_UNUSED(index);
    Q_UNUSED(completed);
    // 保存到配置
    if (m_config) {
        m_config->setTodoItems(m_todoWidget->todoItems());
    }
}

void MainWindow::onTodoHeightChanged(int newHeight)
{
    Q_UNUSED(newHeight);
    // ✅ 添加延迟，确保所有 deleteLater 执行完毕
    QTimer::singleShot(10, this, [this]() {
        if (m_config) {
            adjustSize();
        }
    });
}

MainWindow::~MainWindow()
{
    // 释放动态分配的资源
    if (m_raiseTimer) {
        m_raiseTimer->stop();
        delete m_raiseTimer;
        m_raiseTimer = nullptr;
    }
    
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }
    
    if (m_globalShortcut) {
        delete m_globalShortcut;
        m_globalShortcut = nullptr;
    }
    
    if (m_trayIcon) {
        m_trayIcon->hide();
        delete m_trayIcon;
        m_trayIcon = nullptr;
    }
    
    // 其他控件由Qt的对象树管理，会自动释放
}