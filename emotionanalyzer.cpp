#include "emotionanalyzer.h"
#include <QStringList>
#include <QMap>

QString EmotionAnalyzer::analyze(const QString &text)
{
    // 简单关键词情绪分析
    QStringList happyWords = {"快乐", "幸福", "美好", "希望", "阳光", "温暖", "爱", "成功", "喜", "乐", "优秀", "棒", "好", "美"};
    QStringList sadWords = {"悲伤", "孤独", "失落", "痛苦", "遗憾", "雨", "冷", "泪", "伤", "难", "苦", "愁", "寂寞"};
    QStringList energeticWords = {"奋斗", "努力", "激情", "燃烧", "拼", "冲", "快", "急", "热", "烈", "强", "战"};
    QStringList calmWords = {"静", "宁", "安", "淡", "泊", "闲", "禅", "清", "远", "幽", "柔", "慢", "缓"};
    
    int happy = calculateScore(text, happyWords);
    int sad = calculateScore(text, sadWords);
    int energetic = calculateScore(text, energeticWords);
    int calm = calculateScore(text, calmWords);
    
    // 找最高分
    QMap<int, QString> scores;
    scores[happy] = "happy";
    scores[sad] = "sad";
    scores[energetic] = "energetic";
    scores[calm] = "calm";
    
    auto it = scores.end();
    --it;
    return it.value();
}

int EmotionAnalyzer::calculateScore(const QString &text, const QStringList &keywords)
{
    int score = 0;
    for (const QString &word : keywords) {
        score += text.count(word) * 10;
    }
    return score;
}