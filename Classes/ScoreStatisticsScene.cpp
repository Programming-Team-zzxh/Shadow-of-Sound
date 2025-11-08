#include "ScoreStatisticsScene.h"
#include "HelloWorldScene.h"
#include "ScoreChart.h"

USING_NS_CC;

Scene* ScoreStatisticsScene::createScene() {
    return ScoreStatisticsScene::create();
}

bool ScoreStatisticsScene::init() {
    if (!Scene::init()) {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 1. 背景
    auto background = LayerGradient::create(Color4B(30, 30, 60, 255), Color4B(10, 10, 30, 255));
    this->addChild(background);

    // 2. 标题区域
    auto titleBg = LayerColor::create(Color4B(0, 0, 0, 180), visibleSize.width, 100);
    titleBg->setPosition(0, visibleSize.height - 100);
    this->addChild(titleBg);

    auto title = Label::createWithSystemFont("SCORE STATISTICS", "Arial", 36);
    title->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 40));
    title->setColor(Color3B::YELLOW);
    this->addChild(title);

    auto subtitle = Label::createWithSystemFont("Top 20 Ranking & Progress Chart", "Arial", 18);
    subtitle->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 70));
    subtitle->setColor(Color3B(200, 200, 200));
    this->addChild(subtitle);

    // 3. 统计信息区域（在标题下方）
    createStatisticsInfo(visibleSize);

    // 4. 排行榜区域（上半部分）
    createRankingSection(visibleSize);

    // 5. 折线图区域（下半部分）
    createChartSection(visibleSize);

    // 6. 底部信息区域
    auto bottomBg = LayerColor::create(Color4B(0, 0, 0, 180), visibleSize.width, 30);
    bottomBg->setPosition(0, 0);
    this->addChild(bottomBg);

    auto hintLabel = Label::createWithSystemFont("Press ESC to return to main menu", "Arial", 14);
    hintLabel->setPosition(Vec2(visibleSize.width / 2, 15));
    hintLabel->setColor(Color3B(150, 200, 255));
    this->addChild(hintLabel);

    // 7. 返回按钮
    auto backItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",
        CC_CALLBACK_0(ScoreStatisticsScene::backToMenu, this));
    auto menu = Menu::create(backItem, nullptr);
    menu->setPosition(Vec2(50, visibleSize.height - 50));
    this->addChild(menu);

    // 键盘监听
    auto listener = EventListenerKeyboard::create();
    listener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
            backToMenu();
        }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void ScoreStatisticsScene::createStatisticsInfo(const Size& visibleSize) {
    // 统计信息背景
    auto statsBg = LayerColor::create(Color4B(0, 0, 0, 150), visibleSize.width - 100, 50);
    statsBg->setPosition(50, visibleSize.height - 160);
    this->addChild(statsBg);

    // 获取成绩数据
    int totalSongs = ScoreSystem::getInstance()->getTotalSongsCount();
    float average = ScoreSystem::getInstance()->calculateTop20AverageWithZeroPadding();
    int playedCount = std::min(totalSongs, 20);

    // 显示平均分
    char avgText[128];
    sprintf(avgText, "Average Score: %6.3f", average);
    auto avgLabel = Label::createWithSystemFont(avgText, "Arial", 20);
    avgLabel->setPosition(Vec2(visibleSize.width / 2 - 150, visibleSize.height - 135));
    avgLabel->setColor(Color3B(100, 255, 100));
    this->addChild(avgLabel);

    // 显示游玩统计
    char statsText[128];
    sprintf(statsText, "Played: %2d/20", playedCount);
    auto statsLabel = Label::createWithSystemFont(statsText, "Arial", 20);
    statsLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 135));
    statsLabel->setColor(Color3B(200, 200, 255));
    this->addChild(statsLabel);

    // 显示总记录数
    char totalText[128];
    sprintf(totalText, "Total Records: %3d", totalSongs);
    auto totalLabel = Label::createWithSystemFont(totalText, "Arial", 20);
    totalLabel->setPosition(Vec2(visibleSize.width / 2 + 150, visibleSize.height - 135));
    totalLabel->setColor(Color3B(200, 200, 255));
    this->addChild(totalLabel);
}
