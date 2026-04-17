#ifndef EMOTIONANALYZER_H
#define EMOTIONANALYZER_H

#include <QString>

class EmotionAnalyzer
{
public:
    static QString analyze(const QString &text);
    
private:
    static int calculateScore(const QString &text, const QStringList &keywords);
};

#endif