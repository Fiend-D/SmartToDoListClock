#include "aiquoteservice.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QDebug>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <functional>

// 离线语录库
static const QStringList s_offlineQuotes = {
    "岁月静好，现世安稳。\nTime is quiet, life is peaceful.",
    "花开不是为了花落，而是为了灿烂。\nFlowers bloom not to fall, but to shine.",
    "每一个不曾起舞的日子，都是对生命的辜负。\nEvery day without dancing is a betrayal to life.",
    "星光不问赶路人，时光不负有心人。\nStars don't ask travelers, time rewards the earnest.",
    "心若向阳，无惧悲伤。\nIf the heart faces the sun, there's no fear of sorrow.",
    "人生如逆旅，我亦是行人。\nLife is a journey against the current, and I am also a traveler.",
    "万物皆有裂痕，那是光照进来的地方。\nThere is a crack in everything, that's how the light gets in.",
    "且将新火试新茶，诗酒趁年华。\nTry new tea with fresh fire, enjoy poetry and wine while young.",
    "念念不忘，必有回响。\nWhat you keep thinking about will surely echo back.",
    "凡是过往，皆为序章。\nWhat's past is prologue."
};

AIQuoteService::AIQuoteService(const QString &apiKey, const QString &provider, QObject *parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this))
    , m_apiKey(apiKey)
    , m_provider(provider)
{
    // 初始化各平台配置
    m_configs[AIProvider::KIMI] = {
        "https://api.moonshot.cn/v1",
        "moonshot-v1-8k",
        "Moonshot Kimi"
    };
    m_configs[AIProvider::SILICONFLOW] = {
        "https://api.siliconflow.cn/v1",
        "Qwen/Qwen2.5-7B-Instruct",
        "硅基流动"
    };
    m_configs[AIProvider::ZHIPU] = {
        "https://open.bigmodel.cn/api/paas/v4",
        "glm-4-flash",  // 智谱免费版模型
        "智谱AI GLM"
    };
    m_configs[AIProvider::OLLAMA] = {
        "http://localhost:11434/v1",
        "qwen2.5:7b",
        "本地Ollama"
    };
    
    setupProviderConfig();
    qDebug() << "AI服务初始化:" << m_provider << "API Key长度:" << apiKey.length();
}

void AIQuoteService::setupProviderConfig()
{
    if (m_configs.contains(m_provider)) {
        m_baseUrl = m_configs[m_provider].baseUrl;
    } else {
        m_provider = AIProvider::KIMI;
        m_baseUrl = m_configs[AIProvider::KIMI].baseUrl;
    }
}

void AIQuoteService::setProvider(const QString &provider, const QString &apiKey)
{
    m_provider = provider;
    m_apiKey = apiKey;
    setupProviderConfig();
    qDebug() << "切换AI提供商:" << provider;
}

QString AIQuoteService::currentProvider() const
{
    return m_provider;
}

QNetworkRequest AIQuoteService::buildRequest()
{
    QUrl url(m_baseUrl + "/chat/completions");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    // 所有平台都使用 Bearer Token 格式
    request.setRawHeader("Authorization", "Bearer " + m_apiKey.toUtf8());
    
    return request;
}

QString AIQuoteService::getModelForCurrentProvider() const
{
    if (m_configs.contains(m_provider)) {
        return m_configs[m_provider].defaultModel;
    }
    return "moonshot-v1-8k";
}

void AIQuoteService::handleNetworkError(QNetworkReply *reply, const std::function<void(const QByteArray&)> &successCallback)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "HTTP 状态码:" << statusCode << "提供商:" << m_provider;
    
    if (reply->error() != QNetworkReply::NoError) {
        QByteArray errorBody = reply->readAll();
        qDebug() << "网络错误:" << reply->errorString();
        qDebug() << "错误详情:" << errorBody;
        
        // 检查特定错误
        QString errorStr = QString::fromUtf8(errorBody);
        if (errorStr.contains("insufficient") || errorStr.contains("quota") || statusCode == 429) {
            emit quoteReceived("⚠️ 账户余额不足，已切换离线模式\n" + s_offlineQuotes[QRandomGenerator::global()->bounded(s_offlineQuotes.size())], 
                              "系统提示", "calm");
        } else {
            int idx = QRandomGenerator::global()->bounded(s_offlineQuotes.size());
            emit quoteReceived(s_offlineQuotes[idx] + "\n\n[网络受限，使用本地语录]", "离线", "calm");
        }
        reply->deleteLater();
        return;
    }
    
    QByteArray responseData = reply->readAll();
    qDebug() << "响应:" << responseData.left(200);
    
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonObject obj = doc.object();
    
    if (obj.contains("error")) {
        QString errMsg = obj["error"].toObject()["message"].toString();
        qDebug() << "API错误:" << errMsg;
        int idx = QRandomGenerator::global()->bounded(s_offlineQuotes.size());
        emit quoteReceived(s_offlineQuotes[idx], "离线", "calm");
        reply->deleteLater();
        return;
    }
    
    if (!obj.contains("choices") || obj["choices"].toArray().isEmpty()) {
        qDebug() << "响应格式异常";
        emit quoteReceived("响应格式错误", "系统", "calm");
        reply->deleteLater();
        return;
    }
    
    successCallback(responseData);
    reply->deleteLater();
}

void AIQuoteService::fetchQuote()
{
    if (m_apiKey.isEmpty()) {
        qDebug() << "API Key 为空，使用离线语录";
        int idx = QRandomGenerator::global()->bounded(s_offlineQuotes.size());
        emit quoteReceived(s_offlineQuotes[idx], "离线语录", "calm");
        return;
    }

    QNetworkRequest request = buildRequest();
    
    int hour = QDateTime::currentDateTime().time().hour();
    QString timeDesc = (hour >= 6 && hour < 18) ? "白天" : "夜晚";
    
    // 修改提示词，生成古诗或词中的名句，去掉英文翻译
    QString systemPrompt = QString(
        "你是中国古典文学专家。请从中国古代诗词中选取一句名句，要求："  
        "1. 具有深刻的哲理或优美的意境"  
        "2. 中文18字以内"  
        "3. 注明诗词的出处（作者和作品名称）"  
        "必须严格按以下JSON格式返回，不要任何其他内容：\n"  
        "{\"chinese\": \"名句\", \"source\": \"作者 - 作品名称\", \"emotion\": \"happy/sad/calm/energetic\"}"
    );

    QJsonObject systemMsg;
    systemMsg["role"] = "user";
    systemMsg["content"] = systemPrompt;

    QJsonArray messages;
    messages.append(systemMsg);

    QJsonObject json;
    json["model"] = getModelForCurrentProvider();
    json["messages"] = messages;
    json["temperature"] = 0.8;
    json["max_tokens"] = 150;

    QByteArray postData = QJsonDocument(json).toJson();
    qDebug() << "请求" << m_provider << ":" << postData.left(100) << "...";

    QNetworkReply *reply = m_network->post(request, postData);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleNetworkError(reply, [this](const QByteArray& responseData) {
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            QJsonObject obj = doc.object();
            
            QString contentText = obj["choices"].toArray().first()
                             .toObject()["message"].toObject()["content"].toString();
            
            qDebug() << "AI返回:" << contentText.left(100);
            
            // 清理markdown
            contentText.remove("```json");
            contentText.remove("```");
            contentText = contentText.trimmed();
            
            // 解析JSON
            QJsonDocument contentDoc = QJsonDocument::fromJson(contentText.toUtf8());
            if (!contentDoc.isNull() && contentDoc.isObject()) {
                QJsonObject quoteObj = contentDoc.object();
                QString chinese = quoteObj["chinese"].toString();
                QString source = quoteObj["source"].toString();
                QString emotion = quoteObj["emotion"].toString("calm");
                
                if (chinese.isEmpty()) chinese = contentText;
                if (source.isEmpty()) source = "未知出处";
                
                emit quoteReceived(chinese, source, emotion);
            } else {
                // 非JSON格式，直接显示
                emit quoteReceived(contentText, "未知出处", "calm");
            }
        });
    });
}

void AIQuoteService::fetchWeatherPoem(const QString &weather, int temp)
{
    if (m_apiKey.isEmpty()) {
        emit weatherPoemReceived("天气API未配置");
        return;
    }

    QNetworkRequest request = buildRequest();

    QString prompt = QString("当前天气：%1，温度%2°C。请生成一句应景的简短诗句或感悟（15字以内），"
                          "贴合天气氛围，优美有意境。直接返回纯文本。").arg(weather).arg(temp);

    QJsonArray messages;
    QJsonObject userMsg;
    userMsg["role"] = "user";
    userMsg["content"] = prompt;
    messages.append(userMsg);

    QJsonObject json;
    json["model"] = getModelForCurrentProvider();
    json["messages"] = messages;
    json["temperature"] = 0.9;
    json["max_tokens"] = 50;

    QNetworkReply *reply = m_network->post(request, QJsonDocument(json).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleNetworkError(reply, [this](const QByteArray& responseData) {
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            QString contentText = doc.object()["choices"].toArray().first()
                             .toObject()["message"].toObject()["content"].toString();
            emit weatherPoemReceived(contentText);
        });
    });
}

void AIQuoteService::generateClockStyle(const QString &userPrompt)
{
    if (m_apiKey.isEmpty()) {
        emit networkError("未配置 API Key");
        return;
    }

    QNetworkRequest request = buildRequest();

    // 智谱专用：简化提示词，避免复杂格式
    // 合并成一个完整的 prompt
    QString promptText = QString(
        "设计一个时钟风格，返回JSON包含：\n"
        "1. 基础样式（颜色、字体等）\n"
        "2. 视觉特效脚本（粒子系统描述）\n\n"
        "输出格式：\n"
        "{\n"
        "  \"name\": \"风格名\",\n"
        "  \"description\": \"描述\",\n"
        "  \"backgroundColor\": \"#RRGGBB\",\n"
        "  \"hourHandColor\": \"#RRGGBB\",\n"
        "  \"minuteHandColor\": \"#RRGGBB\",\n"
        "  \"secondHandColor\": \"#RRGGBB\",\n"
        "  \"markColor\": \"#RRGGBB\",\n"
        "  \"textColor\": \"#RRGGBB\",\n"
        "  \"quoteColor\": \"#RRGGBB\",\n"
        "  \"fontFamily\": \"字体名\",\n"
        "  \"isDigital\": false,\n"
        "  \"bgAlpha\": 80,\n"
        "  \"glowEffect\": true,\n"
        "  \"emotion\": \"energetic|sad|happy|calm\",\n"
        "  \"aiVisualScript\": \"{\\\"description\\\":\\\"...\\\",\\\"physics\\\":{\\\"gravity\\\":\\\"5\\\",\\\"wind_x\\\":\\\"sin(time*0.5)\\\",\\\"wind_y\\\":\\\"0\\\"},\\\"emitter\\\":{\\\"shape\\\":\\\"petal|snowflake|maple|ginkgo|circle|geometric\\\",\\\"max_count\\\":20},\\\"colors\\\":{\\\"primary\\\":\\\"#RRGGBB\\\",\\\"secondary\\\":\\\"#RRGGBB\\\"}}\"\n"
        "}\n\n"
        "视觉脚本说明：\n"
        "- shape: petal(花瓣), snowflake(雪花), maple(枫叶), ginkgo(银杏), circle(圆点), geometric(几何)\n"
        "- physics: gravity重力, wind_x横向风力(可用sin/cos/time表达式), wind_y纵向风力\n"
        "- colors: primary主色, secondary副色\n\n"
        "风格要求：%1\n"
        "注意：emotion必须是 energetic、sad、happy、calm 中的一个，不要其他值。"
    ).arg(userPrompt.isEmpty() ? "随机创意，根据当前季节或氛围自动生成" : userPrompt);
    if (!userPrompt.isEmpty()) {
        promptText = "基于以下要求生成：按照模板生成，不要返回多余的内容，对于emotion，('energetic' or 'sad' or 'happy' or 'calm'?select one!)" + userPrompt + "\n" + promptText;
    }else{
        promptText = "基于以下要求生成：按照模板生成，但你可以生成一款独一无二随机的好看的风格样式,对于emotion，('energetic' or 'sad' or 'happy' or 'calm'?select one!)" + userPrompt + "\n" + promptText;
    }

    QJsonArray messages;
    
    // 智谱只用 user 角色
    QJsonObject msg;
    msg["role"] = "user";
    msg["content"] = promptText;
    messages.append(msg);

    QJsonObject json;
    json["model"] = getModelForCurrentProvider();
    json["messages"] = messages;
    
    // 只有非智谱平台才加额外参数
    json["temperature"] = 0.9;
    json["max_tokens"] = 2000;

    qDebug() << "样式生成请求:" << m_provider << QJsonDocument(json).toJson().left(200);

    QNetworkReply *reply = m_network->post(request, QJsonDocument(json).toJson());
    
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleNetworkError(reply, [this](const QByteArray& responseData) {
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            
            QString contentText = doc.object()["choices"].toArray().first()
                            .toObject()["message"].toObject()["content"].toString();
            
            qDebug() << "原始AI返回:" << contentText.left(200);
            
            // 步骤1: 移除 markdown 代码块标记
            contentText.remove("```json");
            contentText.remove("```");
            contentText = contentText.trimmed();
            
            // 步骤2: 移除 JSON 中的注释 (// 开头的内容)
            QRegularExpression commentRegex("//[^\\n]*");
            contentText.remove(commentRegex);
            
            // 步骤3: 提取大括号之间的内容（防止AI有多余文字）
            int startBrace = contentText.indexOf('{');
            int endBrace = contentText.lastIndexOf('}');
            if (startBrace != -1 && endBrace != -1 && endBrace > startBrace) {
                contentText = contentText.mid(startBrace, endBrace - startBrace + 1);
            }
            
            qDebug() << "清理后:" << contentText.left(200);
            
            QJsonDocument styleDoc = QJsonDocument::fromJson(contentText.toUtf8());
            if (!styleDoc.isNull() && styleDoc.isObject()) {
                QJsonObject obj = styleDoc.object();
                QString name = obj["name"].toString("AI风格");
                QString desc = obj["description"].toString("AI创意设计");
                emit styleGenerated(name, desc, obj);
            } else {
                qDebug() << "JSON解析失败:" << contentText.left(100);
                emit networkError("无法解析AI返回的数据，请重试");
            }
        });
    });
}