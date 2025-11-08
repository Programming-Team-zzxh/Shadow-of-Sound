#ifndef __MENU_PICK_CUSTOM_H__
#define __MENU_PICK_CUSTOM_H__

#include "cocos2d.h"
#include <vector>
#include <string> 
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

class CustomSongButton : public cocos2d::MenuItemSprite
{
public:
    // 添加回调函数类型
    typedef std::function<void(int, const std::string&)> ButtonClickCallback;
    static CustomSongButton* create(const std::string& songName, int index, ButtonClickCallback callback);
    bool initWithSong(const std::string& songName, int index, ButtonClickCallback callback);

    // 添加点击处理方法
    void onButtonClicked(Ref* sender);

private:
    std::string _songName;
    int _index;
    ButtonClickCallback _callback; // 存储回调函数
};

class MenuPick_Custom : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();

    void backmeun();

    void gameplay(Ref* pSender);

private:
    int Pick_diff=3;// 当前难度 (1=Ez, 2=Hd, 3=In)
    int js_exist = 1;
    std::string music_name; // 当前选中的歌曲名
    rapidjson::Document RecJson; // JSON文档
    std::string Rec; // JSON字符串

    void pickdiffprer(int pick);
    void pickspeed(int pick);
    void pickdifficulty(float dt);

    std::vector<std::string> getCustomSongs();

    CREATE_FUNC(MenuPick_Custom);

    void onSongButtonClicked(int index, const std::string& songName);
    void updateSongCover(const std::string& songName);
    void updateArrowPosition(int index);


    int _currentSelectedIndex;
    std::string _currentSelectedSong;
};

#endif // __MENU_PICK_CUSTOM_H__