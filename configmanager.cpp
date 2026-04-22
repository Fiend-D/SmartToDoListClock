#include "configmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
{
    m_configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) 
                   + "/SmartDeskClock/config.json";
    QDir().mkpath(QFileInfo(m_configPath).path());
    loadConfig();
}

void ConfigManager::loadConfig()
{
    QFile file(m_configPath);
    if (!file.exists()) {
        // 默认配置
        m_config = QJsonObject{
            {"kimiApiKey", ""},
            {"weatherApiKey", ""},
            {"city", "Beijing"},
            {"currentStyle", "cyberpunk"},
            // 在默认 JSON 中添加
            {"aiProvider", "zhipu"},  // 默认使用智谱
            {"kimiApiKey", ""},
            {"siliconflowApiKey", ""},
            {"zhipuApiKey", ""},
            {"ollamaUrl", "http://localhost:11434"},
            {"styles", QJsonObject{
                {"cyberpunk", QJsonObject{
                    {"name", "赛博朋克"}, {"description", "霓虹与代码的交响"},
                    {"backgroundColor", "#0a0a0f"}, {"hourHandColor", "#00ff9d"},
                    {"minuteHandColor", "#00d4ff"}, {"secondHandColor", "#ff006e"},
                    {"markColor", "#00d4ff"}, {"textColor", "#ffffff"},
                    {"quoteColor", "#00ff9d"}, {"fontFamily", "Noto Sans CJK SC"},
                    {"isDigital", false}, {"bgAlpha", 80}, {"glowEffect", true},
                    {"emotion", "energetic"}
                }},
                {"minimal", QJsonObject{
                    {"name", "极简黑白"}, {"description", "少即是多"},
                    {"backgroundColor", "#ffffff"}, {"hourHandColor", "#333333"},
                    {"minuteHandColor", "#666666"}, {"secondHandColor", "#ff4444"},
                    {"markColor", "#999999"}, {"textColor", "#222222"},
                    {"quoteColor", "#555555"}, {"fontFamily", "Noto Sans CJK SC"},
                    {"isDigital", true}, {"bgAlpha", 0}, {"glowEffect", false},
                    {"emotion", "calm"}
                }},
                {"sakura", QJsonObject{
                    {"name", "樱花粉"}, {"description", "春日的温柔"},
                    {"backgroundColor", "#fff0f5"}, {"hourHandColor", "#ff69b4"},
                    {"minuteHandColor", "#ff1493"}, {"secondHandColor", "#db7093"},
                    {"markColor", "#ffb6c1"}, {"textColor", "#8b4513"},
                    {"quoteColor", "#d87093"}, {"fontFamily", "Noto Sans CJK SC"},
                    {"isDigital", false}, {"bgAlpha", 40}, {"glowEffect", true},
                    {"emotion", "happy"}
                }}
            }}
        };
        saveConfig();
    } else {
        file.open(QIODevice::ReadOnly);
        m_config = QJsonDocument::fromJson(file.readAll()).object();
        file.close();
    }
    
    QJsonObject styles = m_config["styles"].toObject();
    for (const QString &key : styles.keys()) {
        m_styles[key] = loadStyleFromJson(styles[key].toObject());
    }
    
    m_currentStyle = loadStyleFromJson(m_config["styles"].toObject()[m_config["currentStyle"].toString()].toObject());
    
    // 加载代办事项
    loadTodoItems();
}

ClockStyle ConfigManager::loadStyleFromJson(const QJsonObject &obj)
{
    ClockStyle style;
    style.name = obj["name"].toString("未命名");
    style.description = obj["description"].toString();
    style.backgroundColor = QColor(obj["backgroundColor"].toString("#000000"));
    style.hourHandColor = QColor(obj["hourHandColor"].toString("#ffffff"));
    style.minuteHandColor = QColor(obj["minuteHandColor"].toString("#ffffff"));
    style.secondHandColor = QColor(obj["secondHandColor"].toString("#ff0000"));
    style.markColor = QColor(obj["markColor"].toString("#888888"));
    style.textColor = QColor(obj["textColor"].toString("#ffffff"));
    style.quoteColor = QColor(obj["quoteColor"].toString("#ffffff"));
    style.fontFamily = obj["fontFamily"].toString("Noto Sans CJK SC");
    style.isDigital = obj["isDigital"].toBool(false);
    style.bgAlpha = obj["bgAlpha"].toInt(80);
    style.glowEffect = obj["glowEffect"].toBool(true);
    style.emotion = obj["emotion"].toString("calm");
    style.aiVisualScript = obj["aiVisualScript"].toString();  // ✅ 加载AI脚本
    return style;
}

QString ConfigManager::apiKey() const
{
    QString env = qEnvironmentVariable("KIMI_API_KEY");
    if (!env.isEmpty()) return env;
    return m_config["kimiApiKey"].toString();
}

QString ConfigManager::weatherApiKey() const
{
    return m_config["weatherApiKey"].toString();
}

QString ConfigManager::city() const
{
    return m_config["city"].toString("Beijing");
}

ClockStyle ConfigManager::currentStyle() const
{
    return m_currentStyle;
}

QStringList ConfigManager::availableStyles() const
{
    QStringList list;
    for (auto it = m_styles.begin(); it != m_styles.end(); ++it) {
        list.append(it.value().name);
    }
    return list;
}

ClockStyle ConfigManager::getStyle(const QString &id) const
{
    return m_styles.value(id);
}

ClockStyle ConfigManager::getStyleByName(const QString &name) const
{
    for (auto it = m_styles.begin(); it != m_styles.end(); ++it) {
        if (it.value().name == name) {
            return it.value();
        }
    }
    return m_styles.value("cyberpunk");
}

QString ConfigManager::getStyleIdByName(const QString &name) const
{
    for (auto it = m_styles.begin(); it != m_styles.end(); ++it) {
        if (it.value().name == name) {
            return it.key();
        }
    }
    return "cyberpunk";
}

void ConfigManager::setStyle(const QString &styleNameOrId)
{
    // 先尝试作为ID查找
    if (m_styles.contains(styleNameOrId)) {
        m_config["currentStyle"] = styleNameOrId;
        m_currentStyle = m_styles[styleNameOrId];
        saveConfig();
        return;
    }
    
    // 尝试作为名称查找
    for (auto it = m_styles.begin(); it != m_styles.end(); ++it) {
        if (it.value().name == styleNameOrId) {
            m_config["currentStyle"] = it.key();
            m_currentStyle = it.value();
            saveConfig();
            return;
        }
    }
}

bool ConfigManager::addCustomStyle(const QString &id, const QJsonObject &styleData)
{
    // 保存自定义样式
    QJsonObject styles = m_config["styles"].toObject();
    QString uniqueId = id;
    int counter = 1;
    while (styles.contains(uniqueId)) {
        uniqueId = id + "_" + QString::number(counter++);
    }
    
    styles[uniqueId] = styleData;
    m_config["styles"] = styles;
    // 配置已保存
    m_styles[uniqueId] = loadStyleFromJson(styleData);
    m_config["currentStyle"] = uniqueId;
    m_currentStyle = m_styles[uniqueId];
    saveConfig();
    return true;
}

void ConfigManager::deleteCustomStyle(const QString &id)
{
    if (!id.startsWith("ai_")) return;
    
    QJsonObject styles = m_config["styles"].toObject();
    styles.remove(id);
    m_config["styles"] = styles;
    m_styles.remove(id);
    saveConfig();
}

QString ConfigManager::findStyleForEmotion(const QString &emotion) const
{
    for (auto it = m_styles.begin(); it != m_styles.end(); ++it) {
        if (it.value().emotion == emotion) {
            return it.key();
        }
    }
    return "cyberpunk";
}

QString ConfigManager::findStyleForWeather(const QString &weather) const
{
    if (weather.contains("雨")) return "rainy";
    if (weather.contains("晴")) return "sunny";
    return "";
}

QString ConfigManager::aiProvider() const
{
    return m_config["aiProvider"].toString("zhipu"); // 默认智谱
}

void ConfigManager::setAIProvider(const QString &provider)
{
    m_config["aiProvider"] = provider;
    saveConfig();
}

QString ConfigManager::getAPIKeyForProvider(const QString &provider) const
{
    // 环境变量优先
    QString envVar;
    if (provider == "kimi") envVar = qEnvironmentVariable("KIMI_API_KEY");
    else if (provider == "siliconflow") envVar = qEnvironmentVariable("SILICONFLOW_API_KEY");
    else if (provider == "zhipu") envVar = qEnvironmentVariable("ZHIPU_API_KEY");
    
    if (!envVar.isEmpty()) return envVar;
    
    // 配置文件
    return m_config[provider + "ApiKey"].toString();
}

void ConfigManager::loadTodoItems()
{
    m_todoItems.clear();
    QJsonArray todoArray = m_config["todoItems"].toArray();
    for (const QJsonValue &value : todoArray) {
        if (value.isObject()) {
            m_todoItems.append(TodoItem::fromJson(value.toObject()));
        }
    }
}

QList<TodoItem> ConfigManager::todoItems() const
{
    return m_todoItems;
}

void ConfigManager::setTodoItems(const QList<TodoItem> &items)
{
    m_todoItems = items;
    saveConfig();
}

void ConfigManager::setAPIKeyForProvider(const QString &provider, const QString &key)
{
    m_config[provider + "ApiKey"] = key;
    saveConfig();
}

void ConfigManager::saveConfig()
{
    // 保存代办事项
    QJsonArray todoArray;
    for (const TodoItem &item : m_todoItems) {
        todoArray.append(item.toJson());
    }
    m_config["todoItems"] = todoArray;
    
    QFile file(m_configPath);
    file.open(QIODevice::WriteOnly);
    file.write(QJsonDocument(m_config).toJson(QJsonDocument::Indented));
    file.close();
}

QString ConfigManager::currentLayer() const
{
    return m_config["windowLayer"].toString("top");
}

void ConfigManager::setCurrentLayer(const QString &layer)
{
    m_config["windowLayer"] = layer;
    saveConfig();
}

bool ConfigManager::smartThemeEnabled() const
{
    return m_config["smartThemeEnabled"].toBool(true);
}

void ConfigManager::setSmartThemeEnabled(bool enabled)
{
    m_config["smartThemeEnabled"] = enabled;
    saveConfig();
}

int ConfigManager::pomodoroCompletedToday() const
{
    QString today = QDateTime::currentDateTime().toString("yyyyMMdd");
    QJsonObject pomodoroData = m_config["pomodoro"].toObject();
    QString lastDate = pomodoroData["lastDate"].toString();
    if (lastDate != today) return 0;
    return pomodoroData["count"].toInt(0);
}

void ConfigManager::incrementPomodoroCount()
{
    QString today = QDateTime::currentDateTime().toString("yyyyMMdd");
    QJsonObject pomodoroData = m_config["pomodoro"].toObject();
    QString lastDate = pomodoroData["lastDate"].toString();

    if (lastDate != today) {
        pomodoroData["lastDate"] = today;
        pomodoroData["count"] = 1;
    } else {
        pomodoroData["count"] = pomodoroData["count"].toInt(0) + 1;
    }

    m_config["pomodoro"] = pomodoroData;
    saveConfig();
}