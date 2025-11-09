#ifndef __KEY_CONFIG_H__
#define __KEY_CONFIG_H__

#include "cocos2d.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

USING_NS_CC;

class KeyConfig {
public:
    static KeyConfig* getInstance();

    // 配置管理函数
    void loadConfig();
    void saveConfig();
    EventKeyboard::KeyCode getKeyForTrack(int track);
    void setKeyForTrack(int track, EventKeyboard::KeyCode keyCode);

    // 按键名称功能
    static std::string getKeyDisplayName(EventKeyboard::KeyCode keyCode);
    static std::string getKeyName(EventKeyboard::KeyCode keyCode);

    // 按键检测
    static bool isKeyPressed(EventKeyboard::KeyCode keyCode);

private:
    KeyConfig();
    static KeyConfig* instance;

    EventKeyboard::KeyCode trackKeys[4];
    rapidjson::Document configDoc;

    static std::unordered_map<EventKeyboard::KeyCode, std::string> createKeyNameMap();
};

#endif