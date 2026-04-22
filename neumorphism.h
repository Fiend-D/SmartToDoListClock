#ifndef NEUMORPHISM_H
#define NEUMORPHISM_H

#include <QPainter>
#include <QColor>

/**
 * 新拟态 (Neumorphism) 绘制工具类
 * 提供凸起/凹陷效果的绘制方法
 */
class Neumorphism
{
public:
    // 绘制凸起效果的圆角矩形
    static void drawRaisedRect(QPainter *painter, const QRect &rect, int radius,
                                const QColor &baseColor, qreal shadowOpacity = 0.4,
                                int blurRadius = 12, int offset = 6)
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        // 计算亮色和暗色
        QColor lightColor = lighter(baseColor, 15);
        QColor darkColor = darker(baseColor, 25);
        lightColor.setAlphaF(shadowOpacity);
        darkColor.setAlphaF(shadowOpacity);

        // 绘制底层暗色阴影（右下）
        for (int i = blurRadius; i > 0; --i) {
            qreal alpha = shadowOpacity * (1.0 - i / (qreal)(blurRadius + 2));
            QColor c = darkColor;
            c.setAlphaF(alpha);
            painter->setPen(Qt::NoPen);
            painter->setBrush(c);
            QRect shadowRect = rect.adjusted(i * 0.3, i * 0.3, i * 0.6, i * 0.6);
            painter->drawRoundedRect(shadowRect, radius, radius);
        }

        // 绘制顶层亮色阴影（左上）
        for (int i = blurRadius; i > 0; --i) {
            qreal alpha = shadowOpacity * (1.0 - i / (qreal)(blurRadius + 2));
            QColor c = lightColor;
            c.setAlphaF(alpha);
            painter->setPen(Qt::NoPen);
            painter->setBrush(c);
            QRect shadowRect = rect.adjusted(-i * 0.6, -i * 0.6, -i * 0.3, -i * 0.3);
            painter->drawRoundedRect(shadowRect, radius, radius);
        }

        // 绘制主体
        painter->setPen(Qt::NoPen);
        painter->setBrush(baseColor);
        painter->drawRoundedRect(rect, radius, radius);

        // 内高光（左上边缘）
        QLinearGradient highlightGrad(rect.topLeft(), rect.bottomRight());
        highlightGrad.setColorAt(0.0, QColor(255, 255, 255, 25));
        highlightGrad.setColorAt(0.4, QColor(255, 255, 255, 0));
        painter->setBrush(highlightGrad);
        painter->drawRoundedRect(rect, radius, radius);

        painter->restore();
    }

    // 绘制凹陷效果的圆角矩形
    static void drawInsetRect(QPainter *painter, const QRect &rect, int radius,
                               const QColor &baseColor, qreal shadowOpacity = 0.5,
                               int blurRadius = 8, int offset = 4)
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        // 计算亮色和暗色
        QColor lightColor = lighter(baseColor, 10);
        QColor darkColor = darker(baseColor, 35);
        lightColor.setAlphaF(shadowOpacity);
        darkColor.setAlphaF(shadowOpacity);

        // 绘制主体（先画底）
        painter->setPen(Qt::NoPen);
        painter->setBrush(baseColor);
        painter->drawRoundedRect(rect, radius, radius);

        // 内阴影 - 左上暗色（凹陷感）
        for (int i = blurRadius; i > 0; --i) {
            qreal alpha = shadowOpacity * (1.0 - i / (qreal)(blurRadius + 2));
            QColor c = darkColor;
            c.setAlphaF(alpha);
            painter->setPen(Qt::NoPen);
            painter->setBrush(c);
            QRect shadowRect = rect.adjusted(i * 0.3, i * 0.3, -i * 0.1, -i * 0.1);
            painter->drawRoundedRect(shadowRect, radius, radius);
        }

        // 内高光 - 右下亮色
        for (int i = blurRadius; i > 0; --i) {
            qreal alpha = shadowOpacity * 0.5 * (1.0 - i / (qreal)(blurRadius + 2));
            QColor c = lightColor;
            c.setAlphaF(alpha);
            painter->setPen(Qt::NoPen);
            painter->setBrush(c);
            QRect shadowRect = rect.adjusted(-i * 0.1, -i * 0.1, -i * 0.3, -i * 0.3);
            painter->drawRoundedRect(shadowRect, radius, radius);
        }

        // 顶部边缘暗线（强化凹陷感）
        QLinearGradient edgeGrad(rect.topLeft(), rect.bottomLeft());
        edgeGrad.setColorAt(0.0, QColor(0, 0, 0, 40));
        edgeGrad.setColorAt(0.15, QColor(0, 0, 0, 0));
        painter->setBrush(edgeGrad);
        painter->drawRoundedRect(rect, radius, radius);

        painter->restore();
    }

    // 绘制凸起圆形（用于按钮等）
    static void drawRaisedEllipse(QPainter *painter, const QRect &rect,
                                   const QColor &baseColor, qreal shadowOpacity = 0.4)
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        QColor lightColor = lighter(baseColor, 15);
        QColor darkColor = darker(baseColor, 25);
        lightColor.setAlphaF(shadowOpacity);
        darkColor.setAlphaF(shadowOpacity);

        QPoint center = rect.center();
        int rx = rect.width() / 2;
        int ry = rect.height() / 2;

        // 暗色阴影（右下偏移）
        painter->setPen(Qt::NoPen);
        painter->setBrush(darkColor);
        painter->drawEllipse(center.x() - rx + 4, center.y() - ry + 4,
                             rect.width(), rect.height());

        // 亮色阴影（左上偏移）
        painter->setBrush(lightColor);
        painter->drawEllipse(center.x() - rx - 3, center.y() - ry - 3,
                             rect.width(), rect.height());

        // 主体
        painter->setBrush(baseColor);
        painter->drawEllipse(rect);

        // 顶部高光
        QRadialGradient highlightGrad(center.x(), center.y() - ry * 0.3, rx);
        highlightGrad.setColorAt(0.0, QColor(255, 255, 255, 35));
        highlightGrad.setColorAt(0.5, QColor(255, 255, 255, 10));
        highlightGrad.setColorAt(1.0, QColor(255, 255, 255, 0));
        painter->setBrush(highlightGrad);
        painter->drawEllipse(rect);

        painter->restore();
    }

    // 绘制凹陷圆形（用于表盘等）
    static void drawInsetEllipse(QPainter *painter, const QRect &rect,
                                  const QColor &baseColor, qreal shadowOpacity = 0.5)
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        QColor lightColor = lighter(baseColor, 10);
        QColor darkColor = darker(baseColor, 35);
        lightColor.setAlphaF(shadowOpacity);
        darkColor.setAlphaF(shadowOpacity);

        QPoint center = rect.center();
        int rx = rect.width() / 2;
        int ry = rect.height() / 2;

        // 主体
        painter->setPen(Qt::NoPen);
        painter->setBrush(baseColor);
        painter->drawEllipse(rect);

        // 内阴影 - 顶部暗色
        QRadialGradient shadowGrad(center.x(), center.y() + ry * 0.2, rx * 1.2);
        shadowGrad.setColorAt(0.0, QColor(0, 0, 0, 0));
        shadowGrad.setColorAt(0.7, QColor(0, 0, 0, 0));
        shadowGrad.setColorAt(1.0, darkColor);
        painter->setBrush(shadowGrad);
        painter->drawEllipse(rect);

        // 内高光 - 底部亮色
        QRadialGradient highlightGrad(center.x(), center.y() - rx * 0.3, rx);
        highlightGrad.setColorAt(0.0, QColor(255, 255, 255, 0));
        highlightGrad.setColorAt(0.6, QColor(255, 255, 255, 0));
        highlightGrad.setColorAt(1.0, QColor(255, 255, 255, 20));
        painter->setBrush(highlightGrad);
        painter->drawEllipse(rect);

        // 边缘暗线
        painter->setPen(QPen(darkColor, 1.5));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(rect.adjusted(1, 1, -1, -1));

        painter->restore();
    }

    // 绘制新拟态按钮（带按下状态）
    static void drawButton(QPainter *painter, const QRect &rect, int radius,
                            const QColor &baseColor, bool pressed = false,
                            bool hovered = false)
    {
        if (pressed) {
            drawInsetRect(painter, rect, radius, baseColor, 0.5, 6, 3);
        } else {
            drawRaisedRect(painter, rect, radius, baseColor,
                           hovered ? 0.5 : 0.35, hovered ? 14 : 10, hovered ? 7 : 5);
        }
    }

    // 颜色工具
    static QColor lighter(const QColor &c, int percent)
    {
        int h, s, v;
        c.getHsv(&h, &s, &v);
        v = qMin(255, v + (255 - v) * percent / 100);
        return QColor::fromHsv(h, s, v);
    }

    static QColor darker(const QColor &c, int percent)
    {
        int h, s, v;
        c.getHsv(&h, &s, &v);
        v = qMax(0, v - v * percent / 100);
        return QColor::fromHsv(h, s, v);
    }

    static QColor mix(const QColor &c1, const QColor &c2, qreal ratio)
    {
        int r = qRound(c1.red() * (1 - ratio) + c2.red() * ratio);
        int g = qRound(c1.green() * (1 - ratio) + c2.green() * ratio);
        int b = qRound(c1.blue() * (1 - ratio) + c2.blue() * ratio);
        return QColor(r, g, b);
    }

    // 根据基础色生成一套新拟态配色
    static void generatePalette(const QColor &accent,
                                 QColor *bg, QColor *surface, QColor *surfaceRaised)
    {
        int h, s, v;
        accent.getHsv(&h, &s, &v);

        // 背景色：偏暗的低饱和色
        *bg = QColor::fromHsv(h, qMax(10, s / 4), qMax(30, v / 3));

        // 表面色：比背景稍亮
        *surface = QColor::fromHsv(h, qMax(8, s / 3), qMax(45, v / 2));

        // 凸起表面：再亮一些
        *surfaceRaised = QColor::fromHsv(h, qMax(6, s / 2), qMax(55, v * 2 / 3));
    }

private:
    Neumorphism() = delete;
};

#endif // NEUMORPHISM_H
