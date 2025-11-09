#include "KeySettingScene.h"
#include <cocos/editor-support/cocostudio/SimpleAudioEngine.h>
using namespace CocosDenshion;

USING_NS_CC;

Scene* KeySettingScene::createScene() {
    return KeySettingScene::create();
}

char* KeySettingScene::FontToUTF8(const char* font) {
    int len = MultiByteToWideChar(CP_ACP, 0, font, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_ACP, 0, font, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    if (wstr) delete[] wstr;
    return str;
}

bool KeySettingScene::init() {
    if (!Scene::init()) {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();

    // Background
    auto background = LayerColor::create(Color4B(50, 50, 80, 255));
    this->addChild(background);

    // Title
    auto title = Label::createWithTTF(FontToUTF8("按键设置"), "fonts/simhei.ttf", 48);
    title->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 100));
    title->setColor(Color3B::WHITE);
    this->addChild(title);

    // Description text
    auto instruction = Label::createWithTTF(FontToUTF8("点击右侧按钮重新绑定按键"), "fonts/simhei.ttf", 24);
    instruction->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 150));
    instruction->setColor(Color3B::YELLOW);
    this->addChild(instruction);

    isRebinding = false;
    rebindingTrack = -1;

    // Waiting for binding prompt
    waitingLabel = Label::createWithTTF(FontToUTF8(""), "fonts/simhei.ttf", 28);
    waitingLabel->setPosition(Vec2(visibleSize.width / 2, 150));
    waitingLabel->setColor(Color3B::RED);
    this->addChild(waitingLabel);

    // Create key displays for 4 tracks
    for (int i = 0; i < 4; i++) {
        // Track labels
        std::string trackText = std::string(FontToUTF8("轨道 ")) + std::to_string(i + 1);
        auto trackLabel = Label::createWithTTF(trackText, "fonts/simhei.ttf", 36);
        trackLabel->setPosition(Vec2(visibleSize.width / 2 - 200, 450 - i * 80));
        trackLabel->setColor(Color3B::WHITE);
        trackLabel->setAnchorPoint(Vec2(1.0f, 0.5f));
        this->addChild(trackLabel);

        // Current key display
        keyLabels[i] = Label::createWithTTF("A", "fonts/simhei.ttf", 32);
        keyLabels[i]->setPosition(Vec2(visibleSize.width / 2 - 50, 450 - i * 80));
        keyLabels[i]->setColor(Color3B::GREEN);
        this->addChild(keyLabels[i]);

        // Rebind button
        auto rebindButton = MenuItemImage::create(
            "Picture/Butt/Rebind.png",
            "Picture/Butt/Rebind2.png",
            [=](Ref* sender) {
                SimpleAudioEngine::getInstance()->playEffect("Music file/Pick.mp3");
                startRebinding(i);
            });
        rebindButton->setPosition(Vec2(visibleSize.width / 2 + 100, 450 - i * 80));

        auto menu = Menu::create(rebindButton, nullptr);
        menu->setPosition(Vec2::ZERO);
        this->addChild(menu);
    }

    // Return button
    auto backButton = MenuItemImage::create(
        "Picture/Butt/Back.png",
        "Picture/Butt/Back2.png",
        [=](Ref* sender) {
            SimpleAudioEngine::getInstance()->playEffect("Music file/Pick.mp3");
            Director::getInstance()->popScene();
        });
    backButton->setPosition(Vec2(visibleSize.width - 80, 80));

    auto backMenu = Menu::create(backButton, nullptr);
    backMenu->setPosition(Vec2::ZERO);
    this->addChild(backMenu);

    // Initialize display
    updateKeyDisplay();

    //Keyboard listener
    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        if (isRebinding && rebindingTrack >= 0 && rebindingTrack < 4) {
            if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
                isRebinding = false;
                updateKeyDisplay();
                waitingLabel->setString(FontToUTF8("绑定已取消"));
                waitingLabel->setColor(Color3B::YELLOW);

                this->scheduleOnce([=](float dt) {
                    waitingLabel->setString(FontToUTF8(""));
                    }, 1.5f, "clear_cancel");
                return;
            }

            if (isValidBindingKey(keyCode)) {
                auto keyConfig = KeyConfig::getInstance();
                keyConfig->setKeyForTrack(rebindingTrack, keyCode);
                updateKeyDisplay();
                waitingLabel->setString(FontToUTF8("按键绑定成功!"));
                waitingLabel->setColor(Color3B::GREEN);
                isRebinding = false;
                rebindingTrack = -1;

                this->scheduleOnce([=](float dt) {
                    waitingLabel->setString(FontToUTF8(""));
                    }, 2.0f, "clear_message");
            }
            else {
                waitingLabel->setString(FontToUTF8("此按键不可绑定，请选择其他按键"));
                waitingLabel->setColor(Color3B::RED);

                this->scheduleOnce([=](float dt) {
                    if (isRebinding) {
                        waitingLabel->setString(FontToUTF8("请按下新的按键... (按ESC取消)"));
                        waitingLabel->setColor(Color3B::YELLOW);
                    }
                    }, 1.5f, "clear_invalid");
            }
        }
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void KeySettingScene::updateKeyDisplay() {
    auto keyConfig = KeyConfig::getInstance();

    for (int i = 0; i < 4; i++) {
        auto keyCode = keyConfig->getKeyForTrack(i);
        std::string keyName = KeyConfig::getKeyDisplayName(keyCode);

        if (keyLabels[i]) {
            keyLabels[i]->setString(keyName);
            if (!isRebinding || rebindingTrack != i) {
                keyLabels[i]->setColor(Color3B::GREEN);
            }
        }
    }
}

bool KeySettingScene::isValidBindingKey(EventKeyboard::KeyCode keyCode) {
    if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE ||
        keyCode == EventKeyboard::KeyCode::KEY_ENTER ||
        keyCode == EventKeyboard::KeyCode::KEY_TAB ||
        keyCode == EventKeyboard::KeyCode::KEY_CTRL ||
        keyCode == EventKeyboard::KeyCode::KEY_ALT ||
        keyCode == EventKeyboard::KeyCode::KEY_SHIFT) {
        return false;
    }
    return true;
}

void KeySettingScene::startRebinding(int trackIndex) {
    if (isRebinding) {
        waitingLabel->setString(FontToUTF8("请先完成当前绑定或按ESC取消"));
        waitingLabel->setColor(Color3B::RED);
        return;
    }

    isRebinding = true;
    rebindingTrack = trackIndex;

    if (keyLabels[trackIndex]) {
        keyLabels[trackIndex]->setColor(Color3B::RED);
        keyLabels[trackIndex]->setString(FontToUTF8("等待中..."));
    }

    waitingLabel->setString(FontToUTF8("请按下新的按键... (按ESC取消)"));
    waitingLabel->setColor(Color3B::YELLOW);
}