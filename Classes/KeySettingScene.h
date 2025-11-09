#ifndef __KEY_SETTING_SCENE_H__
#define __KEY_SETTING_SCENE_H__

#include "cocos2d.h"
#include "KeyConfig.h"

USING_NS_CC;

class KeySettingScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();

    void updateKeyDisplay();
    void startRebinding(int trackIndex);
    bool isValidBindingKey(EventKeyboard::KeyCode keyCode);
    char* FontToUTF8(const char* font);

    CREATE_FUNC(KeySettingScene);

private:
    bool isRebinding;
    int rebindingTrack;
    cocos2d::Label* keyLabels[4];
    cocos2d::Label* waitingLabel;
};

#endif