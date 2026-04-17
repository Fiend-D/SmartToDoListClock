#include "weatherwidget.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>
#include <QTimer>
#include <QDebug>

WeatherWidget::WeatherWidget(const QString &apiKey, const QString &city, QWidget *parent)
    : QWidget(parent)
    , m_network(new QNetworkAccessManager(this))
    , m_apiKey(apiKey)
    , m_city(city)
    , m_temperature(0)
{
    setMinimumHeight(40);
    updateWeather();
}

void WeatherWidget::updateWeather()
{
    if (m_apiKey.isEmpty()) {
        m_weatherText = "未配置天气API";
        update();
        return;
    }

    QString cityKey = m_city;
    QMap<QString, QString> cnToEn = {
        {"北京", "Beijing"}, {"上海", "Shanghai"}, {"广州", "Guangzhou"},
        {"深圳", "Shenzhen"}, {"成都", "Chengdu"}, {"杭州", "Hangzhou"},
        {"武汉", "Wuhan"}, {"西安", "Xi'an"}, {"南京", "Nanjing"},
        {"重庆", "Chongqing"}, {"天津", "Tianjin"}, {"苏州", "Suzhou"},
        {"大连", "Dalian"}, {"青岛", "Qingdao"}, {"郑州", "Zhengzhou"},
        {"长沙", "Changsha"}, {"沈阳", "Shenyang"}, {"哈尔滨", "Harbin"},
        {"昆明", "Kunming"}, {"厦门", "Xiamen"}, {"香港", "Hong Kong"},
        {"台北", "Taipei"}, {"东京", "Tokyo"}, {"纽约", "New York"},
        {"伦敦", "London"}, {"巴黎", "Paris"}, {"悉尼", "Sydney"},
        {"新加坡", "Singapore"}, {"迪拜", "Dubai"}, {"柏林", "Berlin"},
        {"莫斯科", "Moscow"}, {"多伦多", "Toronto"}, {"温哥华", "Vancouver"},
        {"洛杉矶", "Los Angeles"}, {"旧金山", "San Francisco"},
        {"西雅图", "Seattle"}, {"芝加哥", "Chicago"}, {"波士顿", "Boston"},
        {"迈阿密", "Miami"}, {"首尔", "Seoul"}, {"曼谷", "Bangkok"},
        {"孟买", "Mumbai"}, {"开罗", "Cairo"}
    };
    
    if (cnToEn.contains(m_city)) {
        cityKey = cnToEn[m_city];
    }
    
    static const QMap<QString, QPair<double, double>> coords = {
        {"Beijing", {39.9042, 116.4074}},
        {"Shanghai", {31.2304, 121.4737}},
        {"Guangzhou", {23.1291, 113.2644}},
        {"Shenzhen", {22.5431, 114.0579}},
        {"Chengdu", {30.5728, 104.0668}},
        {"Hangzhou", {30.2741, 120.1551}},
        {"Wuhan", {30.5928, 114.3055}},
        {"Xi'an", {34.3416, 108.9398}},
        {"Nanjing", {32.0603, 118.7969}},
        {"Chongqing", {29.5630, 106.5516}},
        {"Tianjin", {39.0842, 117.2010}},
        {"Suzhou", {31.2989, 120.5853}},
        {"Dalian", {38.9140, 121.6147}},
        {"Qingdao", {36.0671, 120.3826}},
        {"Zhengzhou", {34.7466, 113.6253}},
        {"Changsha", {28.2280, 112.9388}},
        {"Shenyang", {41.8057, 123.4315}},
        {"Harbin", {45.8038, 126.5350}},
        {"Kunming", {24.8801, 102.8329}},
        {"Xiamen", {24.4798, 118.0894}},
        {"Hong Kong", {22.3193, 114.1694}},
        {"Taipei", {25.0330, 121.5654}},
        {"Tokyo", {35.6762, 139.6503}},
        {"New York", {40.7128, -74.0060}},
        {"London", {51.5074, -0.1278}},
        {"Paris", {48.8566, 2.3522}},
        {"Sydney", {-33.8688, 151.2093}},
        {"Singapore", {1.3521, 103.8198}},
        {"Dubai", {25.2048, 55.2708}},
        {"Berlin", {52.5200, 13.4050}},
        {"Moscow", {55.7558, 37.6173}},
        {"Toronto", {43.6532, -79.3832}},
        {"Vancouver", {49.2827, -123.1207}},
        {"Los Angeles", {34.0522, -118.2437}},
        {"San Francisco", {37.7749, -122.4194}},
        {"Seattle", {47.6062, -122.3321}},
        {"Chicago", {41.8781, -87.6298}},
        {"Boston", {42.3601, -71.0589}},
        {"Miami", {25.7617, -80.1918}},
        {"Seoul", {37.5665, 126.9780}},
        {"Bangkok", {13.7563, 100.5018}},
        {"Mumbai", {19.0760, 72.8777}},
        {"Cairo", {30.0444, 31.2357}},
        {"Jakarta", {-6.2088, 106.8456}}
    };
    
    if (coords.contains(cityKey)) {
        auto c = coords[cityKey];
        // ✅ 改用免费的 Current Weather API
        fetchCurrentWeather(c.first, c.second);
        qDebug() << "使用内置坐标:" << cityKey;
    } else {
        // 不在内置列表，尝试 Geocoding API
        qDebug() << "尝试 Geocoding API...";
        QString geoUrl = QString("http://api.openweathermap.org/geo/1.0/direct?q=%1&limit=1&appid=%2")
            .arg(m_city).arg(m_apiKey);
        
        QNetworkReply *geoReply = m_network->get(QNetworkRequest(QUrl(geoUrl)));
        connect(geoReply, &QNetworkReply::finished, this, [this, geoReply]() {
            if (geoReply->error() == QNetworkReply::NoError) {
                QJsonDocument doc = QJsonDocument::fromJson(geoReply->readAll());
                QJsonArray arr = doc.array();
                if (!arr.isEmpty()) {
                    QJsonObject obj = arr[0].toObject();
                    fetchCurrentWeather(obj["lat"].toDouble(), obj["lon"].toDouble());
                } else {
                    m_weatherText = "城市未找到";
                    update();
                }
            } else {
                qDebug() << "Geocoding失败:" << geoReply->errorString();
                m_weatherText = "城市不在内置列表";
                update();
            }
            geoReply->deleteLater();
        });
    }
}

// ✅ 新函数：使用免费的 Current Weather API 2.5（替代 One Call 3.0）
void WeatherWidget::fetchCurrentWeather(double lat, double lon)
{
    QString url = QString("http://api.openweathermap.org/data/2.5/weather?"
                         "lat=%1&lon=%2&appid=%3&units=metric&lang=zh_cn")
        .arg(lat, 0, 'f', 6)
        .arg(lon, 0, 'f', 6)
        .arg(m_apiKey);
    
    QNetworkReply *reply = m_network->get(QNetworkRequest(QUrl(url)));
    connect(reply, &QNetworkReply::finished, this, &WeatherWidget::onNetworkReply);
}

void WeatherWidget::onNetworkReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    // 先读取数据再检查错误，避免数据丢失
    QByteArray data = reply->readAll();
    int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "天气API错误:" << reply->errorString();
        qDebug() << "HTTP状态码:" << httpCode;
        qDebug() << "错误详情:" << data;
        
        // 解析API返回的错误信息
        QJsonDocument errDoc = QJsonDocument::fromJson(data);
        if (!errDoc.isNull() && errDoc.object().contains("message")) {
            m_weatherText = errDoc.object()["message"].toString();
        } else {
            m_weatherText = QString("错误 %1").arg(httpCode);
        }
        update();
        reply->deleteLater();
        return;
    }
    
    parseWeatherData(data);
    reply->deleteLater();
}

// ✅ 修改：适配 Current Weather API 2.5 的数据结构
void WeatherWidget::parseWeatherData(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();
    
    // 检查API错误码（Current API 用 cod 字段）
    if (obj.contains("cod")) {
        int cod = obj["cod"].toInt();
        if (cod != 200) {
            QString msg = obj["message"].toString();
            qDebug() << "API错误:" << cod << msg;
            m_weatherText = msg.isEmpty() ? "API错误" : msg;
            update();
            return;
        }
    }
    
    // ✅ Current API 2.5 数据结构：
    // 温度在 main.temp，天气在 weather 数组
    QJsonObject main = obj["main"].toObject();
    m_temperature = static_cast<int>(main["temp"].toDouble());
    
    QJsonArray weatherArr = obj["weather"].toArray();
    if (!weatherArr.isEmpty()) {
        QJsonObject weather = weatherArr[0].toObject();
        m_weatherText = weather["description"].toString();
        m_icon = weather["icon"].toString();
    } else {
        m_weatherText = "未知天气";
    }
    
    qDebug() << "天气更新:" << m_temperature << "°C" << m_weatherText << "图标:" << m_icon;
    
    emit weatherUpdated(m_weatherText, m_temperature);
    update();
}

void WeatherWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QFont font = painter.font();
    font.setPointSize(12);
    painter.setFont(font);
    painter.setPen(Qt::white);
    
    QString text = QString("%1 %2°C %3")
        .arg(m_city)
        .arg(m_temperature)
        .arg(m_weatherText);
    
    painter.drawText(rect(), Qt::AlignCenter, text);
}