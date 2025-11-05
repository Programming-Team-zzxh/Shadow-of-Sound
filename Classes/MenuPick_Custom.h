#ifndef __MENU_PICK_CUSTOM_H__
#define __MENU_PICK_CUSTOM_H__

#include "cocos2d.h"
#include <vector>
#include <string> 

class CustomSongButton : public cocos2d::MenuItemSprite
{
public:
    static CustomSongButton* create(const std::string& songName, int index);
    bool initWithSong(const std::string& songName, int index);

    virtual void selected() override;
    virtual void unselected() override;

private:
    std::string _songName;
    int _index;
};

class MenuPick_Custom : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();

    void backmeun();

    void gameplay(Ref* pSender);

private:
    std::vector<std::string> getCustomSongs();

    CREATE_FUNC(MenuPick_Custom);
};

#endif // __MENU_PICK_CUSTOM_H__