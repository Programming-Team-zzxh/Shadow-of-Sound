#include "ScoreSystem.h"
#include "cocos2d.h"

USING_NS_CC;
using namespace rapidjson;

ScoreSystem* ScoreSystem::m_instance = nullptr;

ScoreSystem* ScoreSystem::getInstance() {
    if (!m_instance) {
        m_instance = new ScoreSystem();
        m_instance->loadFromFile();
    }
    return m_instance;
}

ScoreSystem::ScoreSystem() {
}

ScoreSystem::~ScoreSystem() {
    saveToFile();
    if (m_instance == this) {
        m_instance = nullptr;
    }
}

void ScoreSystem::addSongScore(const std::string& songName, const std::string& difficulty,
    int rawScore, int difficultyLevel, int maxPossibleScore) {
    // 检查是否已存在相同歌曲和难度的记录
    bool found = false;
    for (auto& score : m_allScores) {
        if (score.songName == songName && score.difficulty == difficulty) {
            // 如果新分数更高，则更新
            if (rawScore > score.rawScore) {
                score.rawScore = rawScore;
                score.difficultyLevel = difficultyLevel;
                score.maxPossibleScore = maxPossibleScore;
                score.calculateScore();
                score.playTime = time(nullptr);
            }
            found = true;
            break;
        }
    }

    // 如果是新记录
    if (!found) {
        SongScore newScore;
        newScore.songName = songName;
        newScore.difficulty = difficulty;
        newScore.rawScore = rawScore;
        newScore.difficultyLevel = difficultyLevel;
        newScore.maxPossibleScore = maxPossibleScore;
        newScore.playTime = time(nullptr);
        newScore.calculateScore();

        m_allScores.push_back(newScore);
    }

    // 重新排序
    sortScores();
    saveToFile();
}
