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

float ScoreSystem::calculateTop20AverageWithZeroPadding() {
    int totalCount = 20; // 固定计算20首
    float total = 0.0f;

    // 计算实际存在的歌曲分数
    int actualCount = std::min(totalCount, static_cast<int>(m_allScores.size()));
    for (int i = 0; i < actualCount; ++i) {
        total += m_allScores[i].calculatedScore;
    }

    // 不足20首时，用0补足
    for (int i = actualCount; i < totalCount; ++i) {
        total += 0.0f; // 补0
    }

    return total / totalCount;
}

std::vector<SongScore> ScoreSystem::getTopScores(int count) {
    if (m_allScores.empty()) {
        return std::vector<SongScore>();
    }

    int actualCount = std::min(count, static_cast<int>(m_allScores.size()));
    return std::vector<SongScore>(m_allScores.begin(), m_allScores.begin() + actualCount);
}

void ScoreSystem::sortScores() {
    std::sort(m_allScores.begin(), m_allScores.end());
}

std::string ScoreSystem::getScoreFilePath() {
    return FileUtils::getInstance()->getWritablePath() + "SongScores.json";
}

std::string ScoreSystem::getTop20Statistics() {
    std::string result;
    char buffer[256];

    int actualCount = std::min(20, static_cast<int>(m_allScores.size()));
    int zeroPaddingCount = 20 - actualCount;

    sprintf(buffer, "实际游玩: %d首, 补零: %d首", actualCount, zeroPaddingCount);
    result = buffer;

    return result;
}

void ScoreSystem::saveToFile() {
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    rapidjson::Value scoresArray(rapidjson::kArrayType);

    for (const auto& score : m_allScores) {
        rapidjson::Value scoreObject(rapidjson::kObjectType);

        scoreObject.AddMember("songName",
            rapidjson::Value(score.songName.c_str(), allocator).Move(),
            allocator);
        scoreObject.AddMember("difficulty",
            rapidjson::Value(score.difficulty.c_str(), allocator).Move(),
            allocator);
        scoreObject.AddMember("rawScore", score.rawScore, allocator);
        scoreObject.AddMember("maxPossibleScore", score.maxPossibleScore, allocator);
        scoreObject.AddMember("difficultyLevel", score.difficultyLevel, allocator);
        scoreObject.AddMember("calculatedScore", score.calculatedScore, allocator);
        scoreObject.AddMember("playTime", static_cast<int64_t>(score.playTime), allocator);

        scoresArray.PushBack(scoreObject, allocator);
    }

    document.AddMember("scores", scoresArray, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    FILE* file = fopen(getScoreFilePath().c_str(), "wb");
    if (file) {
        fputs(buffer.GetString(), file);
        fclose(file);
    }
}

void ScoreSystem::loadFromFile() {
    m_allScores.clear();

    std::string filePath = getScoreFilePath();
    if (!FileUtils::getInstance()->isFileExist(filePath)) {
        return;
    }

    std::string content = FileUtils::getInstance()->getStringFromFile(filePath);
    if (content.empty()) {
        return;
    }

    rapidjson::Document document;
    document.Parse(content.c_str());

    if (document.HasParseError() || !document.IsObject()) {
        return;
    }

    if (!document.HasMember("scores") || !document["scores"].IsArray()) {
        return;
    }

    const rapidjson::Value& scoresArray = document["scores"];
    for (rapidjson::SizeType i = 0; i < scoresArray.Size(); i++) {
        const rapidjson::Value& scoreObject = scoresArray[i];

        if (!scoreObject.IsObject() ||
            !scoreObject.HasMember("songName") || !scoreObject["songName"].IsString() ||
            !scoreObject.HasMember("difficulty") || !scoreObject["difficulty"].IsString() ||
            !scoreObject.HasMember("rawScore") || !scoreObject["rawScore"].IsInt() ||
            !scoreObject.HasMember("difficultyLevel") || !scoreObject["difficultyLevel"].IsInt()) {
            continue;
        }
