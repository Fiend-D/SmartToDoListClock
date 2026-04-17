#ifndef AIQUOTESERVICE_H
#define AIQUOTESERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>

// 支持的AI提供商
namespace AIProvider {
    constexpr char KIMI[] = "kimi";
    constexpr char SILICONFLOW[] = "siliconflow";
    constexpr char ZHIPU[] = "zhipu";
    constexpr char OLLAMA[] = "ollama";
}

class AIQuoteService : public QObject
{
    Q_OBJECT

public:
    explicit AIQuoteService(const QString &apiKey, const QString &provider = AIProvider::KIMI, QObject *parent = nullptr);
    
    void setProvider(const QString &provider, const QString &apiKey);
    QString currentProvider() const;
    
    void fetchQuote();
    void fetchWeatherPoem(const QString &weather, int temp);
    void generateClockStyle(const QString &userPrompt = QString());

signals:
    void quoteReceived(const QString &quote, const QString &author, const QString &emotion);
    void weatherPoemReceived(const QString &poem);
    void styleGenerated(const QString &styleName, const QString &description, const QJsonObject &styleData);
    void networkError(const QString &error);

private:
    void setupProviderConfig();
    QNetworkRequest buildRequest();
    QString getModelForCurrentProvider() const;
    void handleNetworkError(QNetworkReply *reply, const std::function<void(const QByteArray&)> &successCallback);
    
    QNetworkAccessManager *m_network;
    QString m_apiKey;
    QString m_provider;
    QString m_baseUrl;
    
    // 各平台配置
    struct ProviderConfig {
        QString baseUrl;
        QString defaultModel;
        QString description;
    };
    QMap<QString, ProviderConfig> m_configs;
};

#endif // AIQUOTESERVICE_H