#include "MenuPick_Custom.h"
#include "GamePlay.h"
#include <cocos/editor-support/cocostudio/SimpleAudioEngine.h>
#include <windows.h>
#include <vector>
#include <string>
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;

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
    auto Pick_diff = Sprite::create("Cover/In1.png");
    this->addChild(Pick_diff, 4, 3);
    Pick_diff->setPosition(BackGround->getPosition() + Vec2(110, -25));

    // 等级、分数和难度标签（占位）
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

    // 歌曲封面（默认）
    auto PickSprite1 = Sprite::create("Cover/Bad Apple -TouHou.png"); // 使用默认图片
    PickSprite1->setScale(0.4);
    PickSprite1->setPosition(Vec2(visibleSize.width / 2 + 400, visibleSize.height / 2));
    this->addChild(PickSprite1, 3, 7);

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

    // 在 init() 方法最后添加
    if (!songList.empty()) {
        // 默认选中第一个歌曲
        _currentSelectedIndex = 1;
        _currentSelectedSong = songList[0];
        updateSongCover(_currentSelectedSong);
        updateArrowPosition(_currentSelectedIndex);
    }

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

void MenuPick_Custom::backmeun()
{
    // 关闭场景或执行其他操作
    Director::getInstance()->popScene();
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

    // 切换曲绘
    updateSongCover(songName);

    // 更新箭头位置（如果需要）
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

void MenuPick_Custom::gameplay(Ref* pSender)
{
    Director::getInstance()->popScene();
}