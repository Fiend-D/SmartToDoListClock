#ifndef WEATHERWIDGET_H
#define WEATHERWIDGET_H

#include <QWidget>
#include <QNetworkAccessManager>

class WeatherWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WeatherWidget(const QString &apiKey, const QString &city, QWidget *parent = nullptr);
    void updateWeather();

signals:
    void weatherUpdated(const QString &weather, int temp);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onNetworkReply();

private:
    void parseWeatherData(const QByteArray &data);
    void fetchCurrentWeather(double lat, double lon);  // 替代原来的 fetchOneCall
    
    QNetworkAccessManager *m_network;
    QString m_apiKey;
    QString m_city;
    QString m_weatherText;
    QString m_icon;
    int m_temperature;
};

#endif // WEATHERWIDGET_H