#include "KeyConfig.h"
#include "winuser.h"
#include "platform/CCFileUtils.h"
#include "json/stringbuffer.h"
#include "json/writer.h"

KeyConfig* KeyConfig::instance = nullptr;

KeyConfig* KeyConfig::getInstance() {
    if (!instance) {
        instance = new KeyConfig();
    }
    return instance;
}

KeyConfig::KeyConfig() {
    // Default key configuration: A, S, L, ;
    trackKeys[0] = EventKeyboard::KeyCode::KEY_A;
    trackKeys[1] = EventKeyboard::KeyCode::KEY_S;
    trackKeys[2] = EventKeyboard::KeyCode::KEY_L;
    trackKeys[3] = EventKeyboard::KeyCode::KEY_SEMICOLON;

    loadConfig();
}

void KeyConfig::loadConfig() {
    auto fileUtils = FileUtils::getInstance();

    // Create config directory
    std::string writablePath = fileUtils->getWritablePath();
    std::string configDir = writablePath + "config/";
    if (!fileUtils->isDirectoryExist(configDir)) {
        fileUtils->createDirectory(configDir);
    }

    std::string path = configDir + "keyconfig.json";

    if (fileUtils->isFileExist(path)) {
        std::string content = fileUtils->getStringFromFile(path);
        configDoc.Parse(content.c_str());

        if (!configDoc.HasParseError() && configDoc.IsObject()) {
            if (configDoc.HasMember("track_keys") && configDoc["track_keys"].IsArray()) {
                auto& keyArray = configDoc["track_keys"];
                for (rapidjson::SizeType i = 0; i < 4 && i < keyArray.Size(); i++) {
                    trackKeys[i] = static_cast<EventKeyboard::KeyCode>(keyArray[i].GetInt());
                }
            }
        }
    }
    else {
        // Create default configuration document
        configDoc.SetObject();
        rapidjson::Document::AllocatorType& allocator = configDoc.GetAllocator();

        rapidjson::Value keyArray(rapidjson::kArrayType);
        for (int i = 0; i < 4; i++) {
            keyArray.PushBack(static_cast<int>(trackKeys[i]), allocator);
        }
        configDoc.AddMember("track_keys", keyArray, allocator);

        saveConfig();
    }
}

void KeyConfig::saveConfig() {
    rapidjson::Document::AllocatorType& allocator = configDoc.GetAllocator();

    // Update key configuration
    if (configDoc.HasMember("track_keys")) {
        configDoc.RemoveMember("track_keys");
    }

    rapidjson::Value keyArray(rapidjson::kArrayType);
    for (int i = 0; i < 4; i++) {
        keyArray.PushBack(static_cast<int>(trackKeys[i]), allocator);
    }
    configDoc.AddMember("track_keys", keyArray, allocator);

    // Save to file
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    configDoc.Accept(writer);

    auto fileUtils = FileUtils::getInstance();
    std::string writablePath = fileUtils->getWritablePath();
    std::string configDir = writablePath + "config/";
    if (!fileUtils->isDirectoryExist(configDir)) {
        fileUtils->createDirectory(configDir);
    }

    std::string path = configDir + "keyconfig.json";
    FILE* file = fopen(path.c_str(), "wb");
    if (file) {
        fputs(buffer.GetString(), file);
        fclose(file);
    }
}

EventKeyboard::KeyCode KeyConfig::getKeyForTrack(int track) {
    if (track >= 0 && track < 4) {
        return trackKeys[track];
    }
    return EventKeyboard::KeyCode::KEY_NONE;
}

void KeyConfig::setKeyForTrack(int track, EventKeyboard::KeyCode keyCode) {
    if (track >= 0 && track < 4) {
        trackKeys[track] = keyCode;
        saveConfig(); // Ensure to save the configuration
}

std::string KeyConfig::getKeyDisplayName(EventKeyboard::KeyCode keyCode) {
    return getKeyName(keyCode);
}

std::string KeyConfig::getKeyName(EventKeyboard::KeyCode keyCode) {
    static auto keyMap = createKeyNameMap();
    auto it = keyMap.find(keyCode);
    if (it != keyMap.end()) {
        return it->second;
    }
    return "Key " + std::to_string(static_cast<int>(keyCode));
}

bool KeyConfig::isKeyPressed(EventKeyboard::KeyCode keyCode) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    int vkCode = 0;

    switch (keyCode) {
    case EventKeyboard::KeyCode::KEY_A: vkCode = 'A'; break;
    case EventKeyboard::KeyCode::KEY_B: vkCode = 'B'; break;
    case EventKeyboard::KeyCode::KEY_C: vkCode = 'C'; break;
    case EventKeyboard::KeyCode::KEY_D: vkCode = 'D'; break;
    case EventKeyboard::KeyCode::KEY_E: vkCode = 'E'; break;
    case EventKeyboard::KeyCode::KEY_F: vkCode = 'F'; break;
    case EventKeyboard::KeyCode::KEY_G: vkCode = 'G'; break;
    case EventKeyboard::KeyCode::KEY_H: vkCode = 'H'; break;
    case EventKeyboard::KeyCode::KEY_I: vkCode = 'I'; break;
    case EventKeyboard::KeyCode::KEY_J: vkCode = 'J'; break;
    case EventKeyboard::KeyCode::KEY_K: vkCode = 'K'; break;
    case EventKeyboard::KeyCode::KEY_L: vkCode = 'L'; break;
    case EventKeyboard::KeyCode::KEY_M: vkCode = 'M'; break;
    case EventKeyboard::KeyCode::KEY_N: vkCode = 'N'; break;
    case EventKeyboard::KeyCode::KEY_O: vkCode = 'O'; break;
    case EventKeyboard::KeyCode::KEY_P: vkCode = 'P'; break;
    case EventKeyboard::KeyCode::KEY_Q: vkCode = 'Q'; break;
    case EventKeyboard::KeyCode::KEY_R: vkCode = 'R'; break;
    case EventKeyboard::KeyCode::KEY_S: vkCode = 'S'; break;
    case EventKeyboard::KeyCode::KEY_T: vkCode = 'T'; break;
    case EventKeyboard::KeyCode::KEY_U: vkCode = 'U'; break;
    case EventKeyboard::KeyCode::KEY_V: vkCode = 'V'; break;
    case EventKeyboard::KeyCode::KEY_W: vkCode = 'W'; break;
    case EventKeyboard::KeyCode::KEY_X: vkCode = 'X'; break;
    case EventKeyboard::KeyCode::KEY_Y: vkCode = 'Y'; break;
    case EventKeyboard::KeyCode::KEY_Z: vkCode = 'Z'; break;
    case EventKeyboard::KeyCode::KEY_0: vkCode = '0'; break;
    case EventKeyboard::KeyCode::KEY_1: vkCode = '1'; break;
    case EventKeyboard::KeyCode::KEY_2: vkCode = '2'; break;
    case EventKeyboard::KeyCode::KEY_3: vkCode = '3'; break;
    case EventKeyboard::KeyCode::KEY_4: vkCode = '4'; break;
    case EventKeyboard::KeyCode::KEY_5: vkCode = '5'; break;
    case EventKeyboard::KeyCode::KEY_6: vkCode = '6'; break;
    case EventKeyboard::KeyCode::KEY_7: vkCode = '7'; break;
    case EventKeyboard::KeyCode::KEY_8: vkCode = '8'; break;
    case EventKeyboard::KeyCode::KEY_9: vkCode = '9'; break;
    case EventKeyboard::KeyCode::KEY_SPACE: vkCode = VK_SPACE; break;
    case EventKeyboard::KeyCode::KEY_ENTER: vkCode = VK_RETURN; break;
    case EventKeyboard::KeyCode::KEY_ESCAPE: vkCode = VK_ESCAPE; break;
    case EventKeyboard::KeyCode::KEY_TAB: vkCode = VK_TAB; break;
    case EventKeyboard::KeyCode::KEY_BACKSPACE: vkCode = VK_BACK; break;
    case EventKeyboard::KeyCode::KEY_SHIFT: vkCode = VK_SHIFT; break;
    case EventKeyboard::KeyCode::KEY_CTRL: vkCode = VK_CONTROL; break;
    case EventKeyboard::KeyCode::KEY_ALT: vkCode = VK_MENU; break;
    case EventKeyboard::KeyCode::KEY_SEMICOLON: vkCode = VK_OEM_1; break;
    case EventKeyboard::KeyCode::KEY_APOSTROPHE: vkCode = VK_OEM_7; break;
    case EventKeyboard::KeyCode::KEY_COMMA: vkCode = VK_OEM_COMMA; break;
    case EventKeyboard::KeyCode::KEY_PERIOD: vkCode = VK_OEM_PERIOD; break;
    case EventKeyboard::KeyCode::KEY_SLASH: vkCode = VK_OEM_2; break;
    case EventKeyboard::KeyCode::KEY_BACK_SLASH: vkCode = VK_OEM_5; break;
    case EventKeyboard::KeyCode::KEY_MINUS: vkCode = VK_OEM_MINUS; break;
    case EventKeyboard::KeyCode::KEY_EQUAL: vkCode = VK_OEM_PLUS; break;
    case EventKeyboard::KeyCode::KEY_UP_ARROW: vkCode = VK_UP; break;
    case EventKeyboard::KeyCode::KEY_DOWN_ARROW: vkCode = VK_DOWN; break;
    case EventKeyboard::KeyCode::KEY_LEFT_ARROW: vkCode = VK_LEFT; break;
    case EventKeyboard::KeyCode::KEY_RIGHT_ARROW: vkCode = VK_RIGHT; break;
    default: vkCode = static_cast<int>(keyCode); break;
    }

    return (GetAsyncKeyState(vkCode) & 0x8000) != 0;
#else
    return false;
#endif
}

std::unordered_map<EventKeyboard::KeyCode, std::string> KeyConfig::createKeyNameMap() {
    std::unordered_map<EventKeyboard::KeyCode, std::string> map;

    // Letter keys
    for (char c = 'A'; c <= 'Z'; c++) {
        EventKeyboard::KeyCode keyCode = static_cast<EventKeyboard::KeyCode>(
            static_cast<int>(EventKeyboard::KeyCode::KEY_A) + (c - 'A'));
        map[keyCode] = std::string(1, c);
    }

    // Number keys
    for (char c = '0'; c <= '9'; c++) {
        EventKeyboard::KeyCode keyCode = static_cast<EventKeyboard::KeyCode>(
            static_cast<int>(EventKeyboard::KeyCode::KEY_0) + (c - '0'));
        map[keyCode] = std::string(1, c);
    }

    // Function keys
    map[EventKeyboard::KeyCode::KEY_SPACE] = "Space";
    map[EventKeyboard::KeyCode::KEY_ENTER] = "Enter";
    map[EventKeyboard::KeyCode::KEY_ESCAPE] = "Esc";
    map[EventKeyboard::KeyCode::KEY_TAB] = "Tab";
    map[EventKeyboard::KeyCode::KEY_BACKSPACE] = "Backspace";
    map[EventKeyboard::KeyCode::KEY_SHIFT] = "Shift";
    map[EventKeyboard::KeyCode::KEY_CTRL] = "Ctrl";
    map[EventKeyboard::KeyCode::KEY_ALT] = "Alt";
    map[EventKeyboard::KeyCode::KEY_CAPS_LOCK] = "CapsLock";

    // Direction keys
    map[EventKeyboard::KeyCode::KEY_UP_ARROW] = "Up";
    map[EventKeyboard::KeyCode::KEY_DOWN_ARROW] = "Down";
    map[EventKeyboard::KeyCode::KEY_LEFT_ARROW] = "Left";
    map[EventKeyboard::KeyCode::KEY_RIGHT_ARROW] = "Right";

    // Symbol keys
    map[EventKeyboard::KeyCode::KEY_COMMA] = ",";
    map[EventKeyboard::KeyCode::KEY_PERIOD] = ".";
    map[EventKeyboard::KeyCode::KEY_SEMICOLON] = ";";
    map[EventKeyboard::KeyCode::KEY_APOSTROPHE] = "'";
    map[EventKeyboard::KeyCode::KEY_SLASH] = "/";
    map[EventKeyboard::KeyCode::KEY_BACK_SLASH] = "\\";
    map[EventKeyboard::KeyCode::KEY_MINUS] = "-";
    map[EventKeyboard::KeyCode::KEY_EQUAL] = "=";
    map[EventKeyboard::KeyCode::KEY_LEFT_BRACKET] = "[";
    map[EventKeyboard::KeyCode::KEY_RIGHT_BRACKET] = "]";
    map[EventKeyboard::KeyCode::KEY_GRAVE] = "`";

    // F key
    for (int i = 1; i <= 12; i++) {
        EventKeyboard::KeyCode keyCode = static_cast<EventKeyboard::KeyCode>(
            static_cast<int>(EventKeyboard::KeyCode::KEY_F1) + i - 1);
        map[keyCode] = "F" + std::to_string(i);
    }

    return map;
}
