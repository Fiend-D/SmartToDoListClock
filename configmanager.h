#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QColor>
#include <QMap>
#include "todoitem.h"

struct ClockStyle {
    QString name;
    QString description;
    QColor backgroundColor;
    QColor hourHandColor;
    QColor minuteHandColor;
    QColor secondHandColor;
    QColor markColor;
    QColor textColor;
    QColor quoteColor;
    QString fontFamily;
    bool isDigital;
    int bgAlpha;
    bool glowEffect;
    QString emotion;
    QString aiVisualScript;  // ✅ 添加：AI生成的视觉脚本JSON
};

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    explicit ConfigManager(QObject *parent = nullptr);
    
    QString apiKey() const;
    QString weatherApiKey() const;
    QString city() const;
    ClockStyle currentStyle() const;
    QStringList availableStyles() const;
    ClockStyle getStyle(const QString &id) const;
    
    void setStyle(const QString &styleName);
    bool addCustomStyle(const QString &id, const QJsonObject &styleData);
    void deleteCustomStyle(const QString &id);
    void saveConfig();
    
    QString findStyleForEmotion(const QString &emotion) const;
    QString findStyleForWeather(const QString &weather) const;
    ClockStyle getStyleByName(const QString &name) const;  // 通过名称获取样式
    QString getStyleIdByName(const QString &name) const;     // 通过名称获取ID
    
    QList<TodoItem> todoItems() const;
    void setTodoItems(const QList<TodoItem> &items);
    // 在 public 区域添加
    QString aiProvider() const;
    void setAIProvider(const QString &provider);
    QString getAPIKeyForProvider(const QString &provider) const;
    void setAPIKeyForProvider(const QString &provider, const QString &key);
    // 在 public 区域添加
    QString currentLayer() const;
    void setCurrentLayer(const QString &layer);

private:
    void loadConfig();
    ClockStyle loadStyleFromJson(const QJsonObject &obj);
    void loadTodoItems();
    
    QJsonObject m_config;
    QString m_configPath;
    QMap<QString, ClockStyle> m_styles;
    ClockStyle m_currentStyle;
    QList<TodoItem> m_todoItems;
};

#endif // CONFIGMANAGER_H