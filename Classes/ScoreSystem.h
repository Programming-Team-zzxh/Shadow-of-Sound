#ifndef __SCORE_SYSTEM_H__
#define __SCORE_SYSTEM_H__

#include "cocos2d.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include <string>
#include <vector>
#include <algorithm>

USING_NS_CC;
using namespace rapidjson;

struct SongScore {
    std::string songName;
    std::string difficulty;
    int rawScore;           // 实际得分
    int maxPossibleScore;   // 理论最高分
    int difficultyLevel;    // 难度等级
    float calculatedScore;  // 计算后的分数
    time_t playTime;        // 游玩时间

    SongScore() : rawScore(0), maxPossibleScore(1000000), difficultyLevel(1), calculatedScore(0.0f), playTime(0) {}

    // 计算单曲分数：难度 × (得分/理论最高分)
    void calculateScore() {
        if (maxPossibleScore > 0) {
            float ratio = static_cast<float>(rawScore) / static_cast<float>(maxPossibleScore);
            calculatedScore = difficultyLevel * ratio;
        }
    }

    // 比较函数，用于排序
    bool operator<(const SongScore& other) const {
        return calculatedScore > other.calculatedScore; // 降序排列
    }
};

class ScoreSystem {
public:
    static ScoreSystem* getInstance();

    // 添加新的歌曲成绩
    void addSongScore(const std::string& songName, const std::string& difficulty,
        int rawScore, int difficultyLevel, int maxPossibleScore = 1000000);

    // 计算前20首歌曲的平均分（不足补0）
    float calculateTop20AverageWithZeroPadding();

    // 获取前N首歌曲的成绩
    std::vector<SongScore> getTopScores(int count = 20);

    // 保存到文件
    void saveToFile();

    // 从文件加载
    void loadFromFile();

    // 清除所有记录
    void clearAllScores();

    // 获取总歌曲数量
    int getTotalSongsCount() const { return m_allScores.size(); }

    // 获取前20首的详细统计信息
    std::string getTop20Statistics();

private:
    ScoreSystem();
    ~ScoreSystem();

    static ScoreSystem* m_instance;
    std::vector<SongScore> m_allScores;

    std::string getScoreFilePath();
    void sortScores();
};

#endif // __SCORE_SYSTEM_H__