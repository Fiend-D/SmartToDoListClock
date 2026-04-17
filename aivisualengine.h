#ifndef AIVISUALENGINE_H
#define AIVISUALENGINE_H

#include <QObject>
#include <QPainter>
#include <QJsonObject>
#include <QList>
#include <QHash>

struct Particle {
    qreal x = 0, y = 0;
    qreal vx = 0, vy = 0;
    qreal rotation = 0;
    qreal rotationSpeed = 0;
    qreal scale = 1;
    qreal alpha = 1;
    qreal mass = 1;
    qreal drag = 0.1;
    qreal swayPhase = 0;
    qreal swaySpeed = 1;
    QColor color;
    QColor secondaryColor;
    QString shapeType = "circle";
    qreal birthTime = 0;
    qreal lifeSpan = 999999;
    
    bool isDead(qreal t) const { return (t - birthTime) > lifeSpan; }
};

class AIVisualEngine : public QObject
{
    Q_OBJECT
public:
    explicit AIVisualEngine(QObject *parent = nullptr);
    void loadScript(const QString &jsonString);  // 接受JSON字符串
    void loadScript(const QJsonObject &obj);     // 接受QJsonObject
    void reset();
    void render(QPainter *painter, const QRect &rect, qreal timeMs);

private:
    void updateParticles(const QRect &rect, qreal dt, qreal currentTime);
    void spawnParticles(const QRect &rect, qreal currentTime);
    void renderParticle(QPainter *painter, const Particle &p, qreal currentTime);
    
    void drawPetal(QPainter *p, const Particle &particle);
    void drawSnowflake(QPainter *p, const Particle &particle);
    void drawMaple(QPainter *p, const Particle &particle);
    void drawGinkgo(QPainter *p, const Particle &particle);
    void drawCircle(QPainter *p, const Particle &particle);
    void drawGeometric(QPainter *p, const Particle &particle);
    
    QVariant evalExpr(const QString &expr, const QHash<QString, QVariant> &ctx);
    qreal evalNumber(const QString &expr, const QHash<QString, QVariant> &ctx, qreal def = 0);
    QColor parseColor(const QString &s);
    
    QJsonObject m_script;
    QList<Particle> m_particles;
    QHash<QString, QVariant> m_globals;
    qreal m_timeMs = 0;
    int m_maxParticles = 20;
    QString m_shapeType = "circle";
    QJsonObject m_physics;
    QJsonObject m_colors;
};

#endif