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
