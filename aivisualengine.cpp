#include "aivisualengine.h"
#include <QJsonDocument>
#include <QRandomGenerator>
#include <QtMath>
#include <QRegularExpression>
#include <QPainterPath>  // ✅ 添加这行
#include <cmath>  // ✅ 添加这行，用于sin和cos函数

AIVisualEngine::AIVisualEngine(QObject *parent) : QObject(parent)
{
    // 初始化所有成员变量
    m_timeMs = 0;
    m_maxParticles = 20;
    m_shapeType = "circle";
    m_physics = QJsonObject();
    m_colors = QJsonObject();
    m_script = QJsonObject();
}

void AIVisualEngine::loadScript(const QString &jsonString)
{
    qDebug() << "AIVisualEngine::loadScript(QString) 开始";
    if (jsonString.isEmpty()) {
        qDebug() << "JSON为空，调用reset()";
        reset();
        // 确保即使JSON为空，也能正确初始化所有成员变量
        m_physics = QJsonObject();
        m_colors = QJsonObject();
        m_maxParticles = 20;
        m_shapeType = "circle";
        qDebug() << "JSON为空处理完成";
        return;
    }
    
    qDebug() << "解析JSON字符串";
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (!doc.isNull() && doc.isObject()) {
        qDebug() << "JSON解析成功，调用loadScript(QJsonObject)";
        loadScript(doc.object());
        qDebug() << "loadScript(QJsonObject) 调用完成";
    } else {
        qDebug() << "JSON解析失败，调用reset()";
        reset();
        // 确保即使JSON解析失败，也能正确初始化所有成员变量
        m_physics = QJsonObject();
        m_colors = QJsonObject();
        m_maxParticles = 20;
        m_shapeType = "circle";
        qDebug() << "JSON解析失败处理完成";
    }
    qDebug() << "AIVisualEngine::loadScript(QString) 结束";
}

void AIVisualEngine::loadScript(const QJsonObject &obj)
{
    qDebug() << "AIVisualEngine::loadScript(QJsonObject) 开始";
    m_script = obj;
    qDebug() << "调用reset()";
    reset();
    
    // 确保即使JSON缺少字段，也能正确初始化所有成员变量
    qDebug() << "获取emitter对象";
    QJsonObject emitter = m_script["emitter"].toObject();
    qDebug() << "获取max_count";
    m_maxParticles = emitter["max_count"].toInt(20);
    qDebug() << "获取shape";
    m_shapeType = emitter["shape"].toString("circle");
    qDebug() << "获取physics";
    m_physics = m_script["physics"].toObject();
    qDebug() << "获取colors";
    m_colors = m_script["colors"].toObject();
    
    // 确保physics和colors至少是空对象，避免后续使用时出现问题
    qDebug() << "检查physics是否为空";
    if (m_physics.isEmpty()) {
        m_physics = QJsonObject();
        qDebug() << "physics为空，设置为空对象";
    }
    qDebug() << "检查colors是否为空";
    if (m_colors.isEmpty()) {
        m_colors = QJsonObject();
        qDebug() << "colors为空，设置为空对象";
    }
    qDebug() << "AIVisualEngine::loadScript(QJsonObject) 结束";
}

void AIVisualEngine::reset()
{
    qDebug() << "AIVisualEngine::reset 开始";
    m_particles.clear();
    m_timeMs = 0;
    qDebug() << "AIVisualEngine::reset 结束";
}

void AIVisualEngine::render(QPainter *painter, const QRect &rect, qreal timeMs)
{
    qDebug() << "AIVisualEngine::render 开始";
    qreal dt = (timeMs - m_timeMs) / 1000.0;
    if (dt <= 0 || dt > 0.1) dt = 0.016;
    m_timeMs = timeMs;
    
    qreal currentTime = timeMs / 1000.0;
    
    // 全局变量
    qDebug() << "清除并设置全局变量";
    m_globals.clear();
    m_globals["time"] = currentTime;
    m_globals["width"] = rect.width();
    m_globals["height"] = rect.height();
    m_globals["center_x"] = rect.center().x();
    m_globals["center_y"] = rect.center().y();
    
    qDebug() << "调用 updateParticles";
    updateParticles(rect, dt, currentTime);
    qDebug() << "updateParticles 调用完成";
    
    qDebug() << "调用 spawnParticles";
    spawnParticles(rect, currentTime);
    qDebug() << "spawnParticles 调用完成";
    
    // 按Y排序
    qDebug() << "按Y排序粒子";
    QList<Particle> sorted = m_particles;
    std::sort(sorted.begin(), sorted.end(), [](const Particle &a, const Particle &b) {
        return a.y < b.y;
    });
    
    qDebug() << "渲染粒子，数量:" << sorted.size();
    for (const auto &p : sorted) {
        if (!p.isDead(currentTime)) {
            qDebug() << "渲染粒子:" << &p;
            renderParticle(painter, p, currentTime);
        }
    }
    qDebug() << "AIVisualEngine::render 结束";
}

void AIVisualEngine::updateParticles(const QRect &rect, qreal dt, qreal currentTime)
{
    qDebug() << "AIVisualEngine::updateParticles 开始";
    // 解析物理参数，确保即使m_physics为空也能正常工作
    qreal gravity = 9.8;
    qreal windX = 0;
    qreal windY = 0;
    
    if (!m_physics.isEmpty()) {
        qDebug() << "解析物理参数";
        gravity = evalNumber(m_physics["gravity"].toString("9.8"), m_globals, 9.8);
        windX = evalNumber(m_physics["wind_x"].toString("0"), m_globals, 0);
        windY = evalNumber(m_physics["wind_y"].toString("0"), m_globals, 0);
        qDebug() << "物理参数解析完成: gravity=" << gravity << " windX=" << windX << " windY=" << windY;
    }
    
    qDebug() << "更新粒子，数量:" << m_particles.size();
    for (auto &p : m_particles) {
        // 飘动
        p.swayPhase += p.swaySpeed * dt;
        qreal sway = sin(p.swayPhase) * 20;
        
        // 物理更新
        p.vx += (windX * 10 + sway) * dt / p.mass;
        p.vy += (gravity * 10 + windY) * dt / p.mass;
        p.vx *= (1.0 - p.drag * dt);
        p.vy *= (1.0 - p.drag * dt);
        
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.rotation += p.rotationSpeed * dt;
        
        // 出界重置
        if (p.y > rect.height() + 50 || p.x < -50 || p.x > rect.width() + 50) {
            p.birthTime = -999999; // 标记死亡
        }
    }
    
    // 清理
    qDebug() << "清理死亡粒子";
    m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(),
        [currentTime](const Particle &p) { return p.isDead(currentTime); }),
        m_particles.end());
    qDebug() << "清理完成，剩余粒子数量:" << m_particles.size();
    qDebug() << "AIVisualEngine::updateParticles 结束";
}

void AIVisualEngine::spawnParticles(const QRect &rect, qreal currentTime)
{
    qDebug() << "AIVisualEngine::spawnParticles 开始";
    qDebug() << "当前粒子数量:" << m_particles.size() << "最大粒子数量:" << m_maxParticles;
    while (m_particles.size() < m_maxParticles) {
        qDebug() << "生成新粒子";
        Particle p;
        p.birthTime = currentTime;
        p.lifeSpan = 999999;
        
        // 顶部随机位置
        p.x = QRandomGenerator::global()->bounded(rect.width());
        p.y = -20 - QRandomGenerator::global()->bounded(100);
        
        p.vx = QRandomGenerator::global()->bounded(20) - 10;
        p.vy = 20 + QRandomGenerator::global()->bounded(30);
        p.mass = 0.5 + QRandomGenerator::global()->generateDouble();
        p.drag = 0.1 + QRandomGenerator::global()->generateDouble() * 0.3;
        
        p.rotation = QRandomGenerator::global()->bounded(360);
        p.rotationSpeed = (QRandomGenerator::global()->bounded(100) - 50);
        
        p.scale = 0.5 + QRandomGenerator::global()->generateDouble() * 0.8;
        p.alpha = 0.6 + QRandomGenerator::global()->generateDouble() * 0.4;
        
        p.swayPhase = QRandomGenerator::global()->generateDouble() * M_PI * 2;
        p.swaySpeed = 1.0 + QRandomGenerator::global()->generateDouble() * 2.0;
        
        p.shapeType = m_shapeType;
        // 确保即使m_colors为空也能正常工作
        qDebug() << "设置粒子颜色";
        QString primaryColor = "#ffffff";
        QString secondaryColor = "#cccccc";
        if (!m_colors.isEmpty()) {
            primaryColor = m_colors["primary"].toString("#ffffff");
            secondaryColor = m_colors["secondary"].toString("#cccccc");
        }
        p.color = parseColor(primaryColor);
        p.secondaryColor = parseColor(secondaryColor);
        
        // 颜色随机变化
        int v = QRandomGenerator::global()->bounded(30) - 15;
        p.color = QColor(
            qBound(0, p.color.red() + v, 255),
            qBound(0, p.color.green() + v, 255),
            qBound(0, p.color.blue() + v, 255)
        );
        
        qDebug() << "添加粒子到列表";
        m_particles.append(p);
        qDebug() << "粒子添加完成，当前数量:" << m_particles.size();
    }
    qDebug() << "AIVisualEngine::spawnParticles 结束";
}

void AIVisualEngine::renderParticle(QPainter *painter, const Particle &p, qreal currentTime)
{
    qDebug() << "AIVisualEngine::renderParticle 开始，形状:" << p.shapeType;
    painter->save();
    qDebug() << "设置变换矩阵";
    painter->translate(p.x, p.y);
    painter->rotate(p.rotation);
    painter->scale(p.scale, p.scale);
    painter->setOpacity(p.alpha);
    
    qDebug() << "根据形状绘制粒子";
    if (p.shapeType == "petal") {
        qDebug() << "绘制花瓣";
        drawPetal(painter, p);
    } else if (p.shapeType == "snowflake") {
        qDebug() << "绘制雪花";
        drawSnowflake(painter, p);
    } else if (p.shapeType == "maple") {
        qDebug() << "绘制枫叶";
        drawMaple(painter, p);
    } else if (p.shapeType == "ginkgo") {
        qDebug() << "绘制银杏叶";
        drawGinkgo(painter, p);
    } else if (p.shapeType == "geometric") {
        qDebug() << "绘制几何形状";
        drawGeometric(painter, p);
    } else {
        qDebug() << "绘制圆形";
        drawCircle(painter, p);
    }
    
    qDebug() << "恢复painter状态";
    painter->restore();
    qDebug() << "AIVisualEngine::renderParticle 结束";
}

void AIVisualEngine::drawPetal(QPainter *p, const Particle &particle)
{
    QPainterPath path;
    path.moveTo(0, -10);
    path.cubicTo(6, -8, 10, -2, 8, 2);
    path.cubicTo(6, 6, 3, 8, 0, 10);
    path.cubicTo(-3, 8, -6, 6, -8, 2);
    path.cubicTo(-10, -2, -6, -8, 0, -10);
    
    QLinearGradient grad(0, -10, 0, 10);
    grad.setColorAt(0, particle.color);
    grad.setColorAt(0.7, particle.secondaryColor);
    grad.setColorAt(1, particle.color.darker(120));
    
    p->setBrush(grad);
    p->setPen(Qt::NoPen);
    p->drawPath(path);
    
    // 花脉
    p->setPen(QPen(particle.color.darker(150), 0.5));
    p->drawLine(0, -8, 0, 6);
    p->drawLine(0, -2, 4, -5);
    p->drawLine(0, 0, -3, -4);
}

void AIVisualEngine::drawSnowflake(QPainter *p, const Particle &particle)
{
    p->setPen(QPen(particle.color, 1));
    p->setBrush(Qt::NoBrush);
    
    for (int i = 0; i < 6; ++i) {
        p->drawLine(0, 0, 0, -8);
        p->drawLine(0, -4, -2, -6);
        p->drawLine(0, -4, 2, -6);
        p->rotate(60);
    }
    
    p->setBrush(particle.color);
    p->drawEllipse(QPoint(0, 0), 2, 2);
}

void AIVisualEngine::drawMaple(QPainter *p, const Particle &particle)
{
    QPolygon poly;
    for (int i = 0; i < 5; ++i) {
        qreal a1 = (i * 72 - 90) * M_PI / 180;
        qreal a2 = ((i + 0.5) * 72 - 90) * M_PI / 180;
        poly << QPoint(10 * cos(a1), 10 * sin(a1));
        poly << QPoint(4 * cos(a2), 4 * sin(a2));
    }
    
    QLinearGradient grad(0, -10, 0, 10);
    grad.setColorAt(0, particle.color);
    grad.setColorAt(1, QColor(139, 69, 19));
    
    p->setBrush(grad);
    p->setPen(QPen(QColor(139, 69, 19), 0.5));
    p->drawPolygon(poly);
}

void AIVisualEngine::drawGinkgo(QPainter *p, const Particle &particle)
{
    QPainterPath path;
    path.moveTo(0, 8);
    path.quadTo(-6, 0, -8, -6);
    path.arcTo(-8, -10, 16, 8, 180, -180);
    path.quadTo(6, 0, 0, 8);
    
    QRadialGradient grad(0, -2, 10);
    grad.setColorAt(0, particle.color);
    grad.setColorAt(1, particle.secondaryColor);
    
    p->setBrush(grad);
    p->setPen(QPen(particle.color.darker(120), 0.5));
    p->drawPath(path);
}

void AIVisualEngine::drawCircle(QPainter *p, const Particle &particle)
{
    QRadialGradient grad(0, 0, 8);
    grad.setColorAt(0, particle.color);
    grad.setColorAt(1, particle.secondaryColor);
    
    p->setBrush(grad);
    p->setPen(Qt::NoPen);
    p->drawEllipse(QPoint(0, 0), 8, 8);
}

void AIVisualEngine::drawGeometric(QPainter *p, const Particle &particle)
{
    p->setPen(QPen(particle.color, 1));
    p->setBrush(Qt::NoBrush);
    
    int sides = 3 + (qRound(particle.x) % 4);
    QPolygon poly;
    for (int i = 0; i < sides; ++i) {
        qreal a = i * 2 * M_PI / sides;
        poly << QPoint(8 * cos(a), 8 * sin(a));
    }
    p->drawPolygon(poly);
    
    p->drawLine(-4, -4, 4, 4);
    p->drawLine(-4, 4, 4, -4);
}

QVariant AIVisualEngine::evalExpr(const QString &expr, const QHash<QString, QVariant> &ctx)
{
    if (expr.isEmpty()) return QVariant();
    
    bool ok;
    qreal num = expr.toDouble(&ok);
    if (ok) return num;
    
    if (ctx.contains(expr)) return ctx[expr];
    
    QString e = expr;
    for (auto it = ctx.begin(); it != ctx.end(); ++it) {
        e.replace(it.key(), QString::number(it.value().toDouble()));
    }
    
    // sin
    QRegularExpression sinRe("sin\\s*\\(\\s*([^)]+)\\s*\\)");
    QRegularExpressionMatch m;
    while ((m = sinRe.match(e)).hasMatch()) {
        QString captured = m.captured(1);
        if (!captured.isEmpty()) {
            qreal v = captured.toDouble();
            e.replace(m.captured(0), QString::number(sin(v * M_PI / 180)));
        }
    }
    
    // cos
    QRegularExpression cosRe("cos\\s*\\(\\s*([^)]+)\\s*\\)");
    while ((m = cosRe.match(e)).hasMatch()) {
        QString captured = m.captured(1);
        if (!captured.isEmpty()) {
            qreal v = captured.toDouble();
            e.replace(m.captured(0), QString::number(cos(v * M_PI / 180)));
        }
    }
    
    // rand
    QRegularExpression randRe("rand\\s*\\(\\s*([^,]+)\\s*,\\s*([^)]+)\\s*\\)");
    while ((m = randRe.match(e)).hasMatch()) {
        QString minStr = m.captured(1);
        QString maxStr = m.captured(2);
        if (!minStr.isEmpty() && !maxStr.isEmpty()) {
            qreal min = minStr.toDouble();
            qreal max = maxStr.toDouble();
            qreal r = min + (max - min) * QRandomGenerator::global()->generateDouble();
            e.replace(m.captured(0), QString::number(r));
        }
    }
    
    num = e.toDouble(&ok);
    // ✅ 修复：统一返回 QVariant
    if (ok) {
        return QVariant(num);
    } else {
        return QVariant(e);
    }
}

qreal AIVisualEngine::evalNumber(const QString &expr, const QHash<QString, QVariant> &ctx, qreal def)
{
    QVariant v = evalExpr(expr, ctx);
    bool ok;
    qreal n = v.toDouble(&ok);
    return ok ? n : def;
}

QColor AIVisualEngine::parseColor(const QString &s)
{
    if (s.startsWith("#")) return QColor(s);
    if (s == "random_pink") return QColor(255, 183 + QRandomGenerator::global()->bounded(40), 197);
    if (s == "random_gold") return QColor(255, 215, 0);
    return QColor(s);
}