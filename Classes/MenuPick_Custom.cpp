#include "MenuPick_Custom.h"
#include "GamePlay.h"
#include <cocos/editor-support/cocostudio/SimpleAudioEngine.h>
#include <windows.h>
#include <vector>
#include <string>
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;
using namespace rapidjson;

extern std::string Diff;
extern std::string Filename;
extern int PLay_Back;
extern float Play_Sp;

Scene* MenuPick_Custom::createScene()
{
    return MenuPick_Custom::create();
}

bool MenuPick_Custom::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建歌曲容器
    auto songContainer = Node::create();
    this->addChild(songContainer, 1, 100); // tag = 100

    // 黑色背景层
    auto Backlayer = LayerColor::create(Color4B::BLACK);
    this->addChild(Backlayer, 0, 0);

    auto Main_interface = Sprite::create("Cover/esm.png");
    Main_interface->setOpacity(100);
    Main_interface->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    Backlayer->addChild(Main_interface, 1);

    // 动态获取Custom文件夹中的歌曲
    std::vector<std::string> songList = getCustomSongs();

    // 创建菜单和按钮
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);

    float startY = 900.0f; // 起始Y坐标
    float spacing = 100.0f; // 按钮间距

    for (int i = 0; i < songList.size(); i++)
    {
        // 创建按钮
        auto button = CustomSongButton::create(songList[i], i + 1,
            CC_CALLBACK_2(MenuPick_Custom::onSongButtonClicked, this));
        if (button)
        {
            button->setPosition(Vec2(300, startY - i * spacing));
            menu->addChild(button);
        }
    }

    songContainer->addChild(menu, 1, 1);

    if (songList.empty())
    {
        auto noSongsLabel = Label::createWithTTF("No custom songs found in Resources/Custom/", "fonts/arial.ttf", 24);
        noSongsLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
        noSongsLabel->setColor(Color3B::WHITE);
        this->addChild(noSongsLabel, 5);
    }

    // 难度选择背景（保持与原界面一致）
    auto BackGround = Sprite::create("Cover/Difficulty.png");
    BackGround->setAnchorPoint(Vec2(0, 1));
    BackGround->setPosition(Vec2(visibleSize.width / 2 + 16, visibleSize.height / 2 - 230));
    this->addChild(BackGround, 2, 2);

    // 默认难度选择
    auto Pick_diff = Sprite::create("Cover/In2.png");
    this->addChild(Pick_diff, 4, 3);
    Pick_diff->setPosition(BackGround->getPosition() + Vec2(110, -25));

    auto GameLevel = Label::createWithTTF(" ", "fonts/arial.ttf", 48);
    this->addChild(GameLevel, 4, 6);
    GameLevel->setAnchorPoint(Vec2(0, 0.5));
    GameLevel->setPosition(Pick_diff->getPosition() + Vec2(595, 0));

    auto GameScore = Label::createWithTTF("0000000", "fonts/Saira-Light.ttf", 48);
    this->addChild(GameScore, 4, 4);
    GameScore->setAnchorPoint(Vec2(0, 0.5));
    GameScore->setPosition(Pick_diff->getPosition() + Vec2(265, 0));

    auto GameDiff = Label::createWithTTF("Lv.0", "fonts/arial.ttf", 36);
    this->addChild(GameDiff, 4, 5);
    GameDiff->setAnchorPoint(Vec2(0, 0.5));
    GameDiff->setPosition(Pick_diff->getPosition() + Vec2(125, 0));

    auto Ez_Diff = MenuItemImage::create("Cover/Ez1.png", "Cover/Ez1.png",
        CC_CALLBACK_0(MenuPick_Custom::pickdiffprer, this, 1));
    auto Hd_Diff = MenuItemImage::create("Cover/Hd1.png", "Cover/Hd1.png",
        CC_CALLBACK_0(MenuPick_Custom::pickdiffprer, this, 2));
    auto In_Diff = MenuItemImage::create("Cover/In1.png", "Cover/In1.png",
        CC_CALLBACK_0(MenuPick_Custom::pickdiffprer, this, 3));
    auto Difficluty = Menu::create(Ez_Diff, Hd_Diff, In_Diff, NULL);
    Difficluty->setPosition(Vec2(0, 0));
    Ez_Diff->setPosition(BackGround->getPosition() + Vec2(25, -25));
    Hd_Diff->setPosition(Ez_Diff->getPosition() + Vec2(85, 0));
    In_Diff->setPosition(Hd_Diff->getPosition() + Vec2(85, 0));
    this->addChild(Difficluty, 3);

    auto Speed = Sprite::create("Cover/Butt/Speed.png");
    Speed->setAnchorPoint(Vec2(0, 1));
    Speed->setPosition(BackGround->getPosition() + Vec2(0, -80));
    this->addChild(Speed, 3);
    auto Sp_Left = MenuItemImage::create("Cover/Butt/Speed_Left.png", "Cover/Butt/Speed_Left.png",
        CC_CALLBACK_0(MenuPick_Custom::pickspeed, this, 1));
    auto Sp_Right = MenuItemImage::create("Cover/Butt/Speed_Right.png", "Cover/Butt/Speed_Right.png",
        CC_CALLBACK_0(MenuPick_Custom::pickspeed, this, 2));
    Sp_Left->setAnchorPoint(Vec2(0, 0));
    Sp_Right->setAnchorPoint(Vec2(1, 0));
    auto SpeedPick = Menu::create(Sp_Left, Sp_Right, NULL);
    SpeedPick->setPosition(Vec2(0, 0));
    Sp_Left->setPosition(Speed->getPosition() + Vec2(0, -50));
    Sp_Right->setPosition(Speed->getPosition() + Vec2(100, -50));
    this->addChild(SpeedPick, 3);
    //显示速度文本
    auto GameSp = Label::createWithTTF(" ", "fonts/Saira-Light.ttf", 20);
    GameSp->setColor(Color3B::BLACK);
    GameSp->setPosition(Speed->getPosition() + Vec2(50, -33));
    this->addChild(GameSp, 3, 9);

    // 箭头指示器
    auto Pick_Arrow = Sprite::create("Cover/Arrow.png");
    Pick_Arrow->setAnchorPoint(Vec2(0, 0.5));
    if (songList.size() > 0)
    {
        Pick_Arrow->setPosition(Vec2(515, startY)); // 指向第一个按钮
    }
    songContainer->addChild(Pick_Arrow, 2, 8);

    // 进入按钮
    auto Enter = MenuItemImage::create("Cover/Enter.png", "Cover/Enter2.png", "Cover/Enter.png",
        CC_CALLBACK_1(MenuPick_Custom::gameplay,this));
    auto EnterMenu = Menu::create(Enter, NULL);
    EnterMenu->setPosition(Vec2::ZERO);
    Enter->setAnchorPoint(Vec2(1, 0));
    Enter->setPosition(Vec2(visibleSize.width - 1, 0));
    this->addChild(EnterMenu, 1);

    //创建键盘监听事件
    auto listener = EventListenerKeyboard::create();
    listener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
            SimpleAudioEngine::getInstance()->playEffect("Music file/Pick.mp3");
            this->gameplay(this);
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
        {
            SimpleAudioEngine::getInstance()->playEffect("Music file/Pick.mp3");
            backmeun();
        }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    // 鼠标滚轮事件（保持滚动功能）
    auto scrollListener = EventListenerMouse::create();
    scrollListener->onMouseScroll = [=](Event* event) {
        EventMouse* e = (EventMouse*)event;
        float scrollY = e->getScrollY();

        Vec2 mousePos = e->getLocationInView();
        auto visibleSize = Director::getInstance()->getVisibleSize();

        if (mousePos.x > visibleSize.width / 4)
            return;

        auto container = this->getChildByTag(100);
        if (!container) return;

        float scrollSpeed = 40.0f;
        float deltaY = scrollY * scrollSpeed;

        Vec2 pos = container->getPosition();
        pos.y += deltaY;

        float minY = -400.0f;
        float maxY = 400.0f;
        pos.y = clampf(pos.y, minY, maxY);

        container->setPosition(pos);
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(scrollListener, this);

    if (!songList.empty()) {
        // 默认选中第一个歌曲
        _currentSelectedIndex = 1;
        _currentSelectedSong = songList[0];
        updateSongCover(_currentSelectedSong);
        updateArrowPosition(_currentSelectedIndex);
    
    // 自动调用第一个按钮的点击逻辑来初始化所有数据
    music_name = songList[0]; // 设置 music_name
    onSongButtonClicked(1, songList[0]); // 模拟点击第一个按钮

    // 强制立即更新一次难度和分数显示
    this->scheduleOnce([this](float dt) {
        this->pickdifficulty(0);
        }, 0.1f, "initial_update");
    }

    this->schedule(CC_SCHEDULE_SELECTOR(MenuPick_Custom::pickdifficulty));

    return true;
}

std::vector<std::string> MenuPick_Custom::getCustomSongs()
{
    std::vector<std::string> songs;

    // 使用宽字符版本支持Unicode
    std::wstring searchPath = L"../Resources/Custom/*";

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        CCLOG("Error scanning folder. Error code: %d", error);

        // 返回默认歌曲或空列表
        songs.push_back("No Songs Available");
        return songs;
    }

    do {
        // 跳过特殊目录
        if (wcscmp(findFileData.cFileName, L".") == 0 ||
            wcscmp(findFileData.cFileName, L"..") == 0) {
            continue;
        }

        // 只处理文件夹
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // 将宽字符串转换为普通字符串
            std::wstring wideName = findFileData.cFileName;
            std::string folderName(wideName.begin(), wideName.end());

            // 过滤系统/隐藏文件夹
            if (!(findFileData.dwFileAttributes &
                (FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN))) {
                songs.push_back(folderName);
            }
        }

    } while (FindNextFileW(hFind, &findFileData) != 0);

    FindClose(hFind);

    // 按字母顺序排序
    std::sort(songs.begin(), songs.end());

    return songs;
}

// CustomSongButton 类的实现
CustomSongButton* CustomSongButton::create(const std::string& songName, int index,ButtonClickCallback callback)
{
    CustomSongButton* button = new (std::nothrow) CustomSongButton();
    if (button && button->initWithSong(songName,index,callback))
    {
        button->autorelease();
        return button;
    }
    CC_SAFE_DELETE(button);
    return nullptr;
}

bool CustomSongButton::initWithSong(const std::string& songName, int index, ButtonClickCallback callback)
{
    // 创建白色背景精灵
    auto whiteBg = Sprite::create("Cover/White.png");
    if (!whiteBg)
    {
        return false;
    }

    // 初始化MenuItemSprite
    if (!MenuItemSprite::initWithNormalSprite(whiteBg, whiteBg, whiteBg, nullptr))
    {
        return false;
    }

    // 添加歌曲名标签
    auto songLabel = Label::createWithTTF(songName, "fonts/arial.ttf", 36);
    if (songLabel)
    {
        songLabel->setPosition(Vec2(199, 29));
        this->addChild(songLabel, 0);
    }

    _songName = songName;
    _index = index;
    _callback = callback;

    // 设置点击回调
    this->setCallback(CC_CALLBACK_1(CustomSongButton::onButtonClicked, this));

    return true;
}

void CustomSongButton::onButtonClicked(Ref* sender)
{
    // 触发回调函数，传递索引和歌曲名
    if (_callback) {
        _callback(_index, _songName);
    }
}

void MenuPick_Custom::onSongButtonClicked(int index, const std::string& songName)
{
    // 更新当前选中的索引和歌曲名
    _currentSelectedIndex = index;
    _currentSelectedSong = songName;
    music_name = songName;

    updateSongCover(songName);
    updateArrowPosition(index);
}

void MenuPick_Custom::updateSongCover(const std::string& songName)
{
    // 移除旧的曲绘
    this->removeChildByTag(7);

    // 构建曲绘路径 - 假设曲绘文件名为 "cover.png"
    std::string coverPath = "Custom/" + songName + "/cover.png";

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto newCover = Sprite::create(coverPath);

    // 如果找不到曲绘，使用默认图片
    if (!newCover) {
        newCover = Sprite::create("Cover/default.png");
        CCLOG("Cover image not found for song: %s", songName.c_str());
    }

    newCover->setScale(0.4);
    newCover->setPosition(Vec2(visibleSize.width / 2 + 400, visibleSize.height / 2));
    this->addChild(newCover, 3, 7); // 使用相同的tag 7
}

void MenuPick_Custom::updateArrowPosition(int index)
{
    auto songContainer = this->getChildByTag(100);
    if (!songContainer) return;

    auto pickarrow = dynamic_cast<Sprite*>(songContainer->getChildByTag(8));
    if (pickarrow) {
        float startY = 900.0f;
        float spacing = 100.0f;
        float newY = startY - (index - 1) * spacing;


        // 关键修改：设置的是在songContainer内的相对位置
        pickarrow->setPosition(Vec2(515, newY));

        CCLOG("Arrow moved to relative position in container: (%f, %f)", 515.0f, newY);
    }
}

void MenuPick_Custom::pickdiffprer(int pick)
{
    Pick_diff = pick;
    auto background = (Sprite*)this->getChildByTag(2);
    this->getChildByTag(3)->removeFromParentAndCleanup(true);
    if (Pick_diff == 1)
    {
        auto Pick_diff = Sprite::create("Cover/Ez2.png");
        this->addChild(Pick_diff, 4, 3);
        Pick_diff->setPosition(background->getPosition() + Vec2(25, -25));
    }
    else if (Pick_diff == 2)
    {
        auto Pick_diff = Sprite::create("Cover/Hd2.png");
        this->addChild(Pick_diff, 4, 3);
        Pick_diff->setPosition(background->getPosition() + Vec2(110, -25));
    }
    else if (Pick_diff == 3)
    {
        auto Pick_diff = Sprite::create("Cover/In2.png");
        this->addChild(Pick_diff, 4, 3);
        Pick_diff->setPosition(background->getPosition() + Vec2(195, -25));
    }
}

void MenuPick_Custom::pickspeed(int pick)
{
    Rec = FileUtils::getInstance()->getStringFromFile("Record/GameSet.json");
    RecJson.Parse<rapidjson::kParseDefaultFlags>(Rec.c_str());
    if (pick == 1 && Play_Sp > 1.05)
    {
        float Speed = Play_Sp - 0.1f;
        RecJson["Speed"].SetFloat(Speed);
    }
    if (pick == 2 && Play_Sp < 2.95)
    {
        float Speed = Play_Sp + 0.1f;
        RecJson["Speed"].SetFloat(Speed);
    }
    //将json数据重新写入文件中
    StringBuffer buffer;
    rapidjson::Writer<StringBuffer> writer(buffer);
    RecJson.Accept(writer);
    std::string filepath = FileUtils::getInstance()->fullPathForFilename("Record/GameSet.json");
    FILE* Readfile = fopen(filepath.c_str(), "wb");
    fputs(buffer.GetString(), Readfile);
    fclose(Readfile);
}

void MenuPick_Custom::pickdifficulty(float dt)
{
    // 检查自定义歌曲的JSON文件是否存在
    std::string recordPath = "Custom/" + _currentSelectedSong + "/record.json";
    std::string defaultPath = "Record/Default.json";

    // 先尝试读取自定义歌曲的JSON文件
    Rec = FileUtils::getInstance()->getStringFromFile(recordPath);

    // 如果自定义文件不存在，则使用默认文件
    if (Rec.empty()) {
        js_exist=0;
        Rec = FileUtils::getInstance()->getStringFromFile(defaultPath);
    }
    else { js_exist = 1; }

    // 解析JSON
    RecJson.Parse<rapidjson::kParseDefaultFlags>(Rec.c_str());

    auto background = (Sprite*)this->getChildByTag(2);
    auto gamescore = (Label*)this->getChildByTag(4);
    auto gamediff = (Label*)this->getChildByTag(5);
    auto gameLevel = (Label*)this->getChildByTag(6);

    char Temporary[32];

    if (js_exist == 0)
    {
        sprintf(Temporary, "%08d", RecJson["Score"]["Ez"].GetInt());
        gamescore->setString(Temporary);
        sprintf(Temporary, "Lv.%d", RecJson["Diff"]["Ez"].GetInt());
        gamediff->setString(Temporary);
        sprintf(Temporary, "%s", RecJson["Level"]["Ez"].GetString());
        gameLevel->setString(Temporary);
    }
   else if (Pick_diff == 1)
    {
        sprintf(Temporary, "%08d", RecJson[music_name.c_str()]["Score"]["Ez"].GetInt());
        gamescore->setString(Temporary);
        sprintf(Temporary, "Lv.%d", RecJson[music_name.c_str()]["Diff"]["Ez"].GetInt());
        gamediff->setString(Temporary);
        sprintf(Temporary, "%s", RecJson[music_name.c_str()]["Level"]["Ez"].GetString());
        gameLevel->setString(Temporary);
    }
    else if (Pick_diff == 2)
    {
        sprintf(Temporary, "%08d", RecJson[music_name.c_str()]["Score"]["Hd"].GetInt());
        gamescore->setString(Temporary);
        sprintf(Temporary, "Lv.%d", RecJson[music_name.c_str()]["Diff"]["Hd"].GetInt());
        gamediff->setString(Temporary);
        sprintf(Temporary, "%s", RecJson[music_name.c_str()]["Level"]["Hd"].GetString());
        gameLevel->setString(Temporary);
    }
    else if (Pick_diff == 3)
    {
        sprintf(Temporary, "%08d", RecJson[music_name.c_str()]["Score"]["In"].GetInt());
        gamescore->setString(Temporary);
        sprintf(Temporary, "Lv.%d", RecJson[music_name.c_str()]["Diff"]["In"].GetInt());
        gamediff->setString(Temporary);
        sprintf(Temporary, "%s", RecJson[music_name.c_str()]["Level"]["In"].GetString());
        gameLevel->setString(Temporary);
    }

    //更新速度（这部分不变）
    Rec = FileUtils::getInstance()->getStringFromFile("Record/GameSet.json");
    RecJson.Parse<rapidjson::kParseDefaultFlags>(Rec.c_str());
    Play_Sp = RecJson["Speed"].GetFloat();
    auto gamespeed = (Label*)this->getChildByTag(9);
    sprintf(Temporary, "%.1f", Play_Sp);
    gamespeed->setString(Temporary);
}



void MenuPick_Custom::gameplay(Ref* pSender)
{
   

    // 设置全局文件名 - 使用自定义歌曲的文件夹名
    Filename = _currentSelectedSong;

    // 设置难度
    if (Pick_diff == 1)
    {
        Diff = "Ez";
    }
    else if (Pick_diff == 2)
    {
        Diff = "Hd";
    }
    else if (Pick_diff == 3)
    {
        Diff = "In";
    }

    Scene* scene = GamePlay::createScene();
    Director::getInstance()->pushScene(TransitionFade::create(1.5, scene));
}

void MenuPick_Custom::backmeun()
{
    // 关闭场景或执行其他操作
    Director::getInstance()->popScene();
}