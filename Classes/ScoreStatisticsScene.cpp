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

void ScoreStatisticsScene::createRankingSection(const Size& visibleSize) {
    // 排行榜标题背景
    auto rankTitleBg = LayerColor::create(Color4B(80, 80, 120, 220), visibleSize.width - 80, 30);
    rankTitleBg->setPosition(40, visibleSize.height - 200);
    this->addChild(rankTitleBg);

    auto rankTitle = Label::createWithSystemFont("TOP 20 RANKING", "Arial", 20);
    rankTitle->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 185));
    rankTitle->setColor(Color3B(255, 255, 100));
    this->addChild(rankTitle);

    // 排行榜背景
    auto rankBg = LayerColor::create(Color4B(0, 0, 0, 120), visibleSize.width - 80, 220);
    rankBg->setPosition(40, visibleSize.height - 420);
    this->addChild(rankBg);

    // 显示前20首歌曲详情
    auto topScores = ScoreSystem::getInstance()->getTopScores(20);
    float startY = visibleSize.height - 230;

    if (!topScores.empty()) {
        // 列标题
        auto headerLabel = Label::createWithSystemFont("RANK    SONG NAME                   DIFF   SCORE", "Arial", 16);
        headerLabel->setPosition(Vec2(visibleSize.width / 2, startY));
        headerLabel->setColor(Color3B(255, 255, 150));
        this->addChild(headerLabel);

        // 分隔线
        auto separator = DrawNode::create();
        separator->drawLine(Vec2(60, startY - 12), Vec2(visibleSize.width - 60, startY - 12), Color4F(1, 1, 1, 0.3f));
        this->addChild(separator);

        // 显示每首歌曲
        for (int i = 0; i < topScores.size(); ++i) {
            // 简化歌曲名显示，固定宽度
            std::string displayName = topScores[i].songName;
            if (displayName.length() > 25) {
                displayName = displayName.substr(0, 22) + "...";
            }
            else {
                // 填充空格以对齐
                displayName.append(25 - displayName.length(), ' ');
            }

            // 格式化输出，确保各列对齐
            char scoreText[256];
            sprintf(scoreText, " %2d     %-25s   %-4s   %6.3f",
                i + 1,
                displayName.c_str(),
                topScores[i].difficulty.c_str(),
                topScores[i].calculatedScore);

            auto scoreLabel = Label::createWithSystemFont(scoreText, "Arial", 14);
            scoreLabel->setPosition(Vec2(visibleSize.width / 2, startY - 20 * (i + 1) - 5));

            // 行背景色交替
            if (i % 2 == 0 && i < 10) { // 只为前10名添加背景色
                auto rowBg = LayerColor::create(Color4B(40, 40, 60, 80), visibleSize.width - 120, 18);
                rowBg->setPosition(60, startY - 20 * (i + 1) - 15);
                this->addChild(rowBg, -1);
            }

            // 根据排名设置不同颜色
            if (i == 0) {
                scoreLabel->setColor(Color3B(255, 215, 0)); // 金色
            }
            else if (i == 1) {
                scoreLabel->setColor(Color3B(192, 192, 192)); // 银色
            }
            else if (i == 2) {
                scoreLabel->setColor(Color3B(205, 127, 50)); // 铜色
            }
            else if (i < 10) {
                scoreLabel->setColor(Color3B(100, 255, 150)); // 前10名亮绿色
            }
            else {
                scoreLabel->setColor(Color3B(200, 200, 200)); // 其他浅灰色
            }
            this->addChild(scoreLabel);
        }

        // 底部边框
        auto bottomLine = DrawNode::create();
        float bottomY = startY - 20 * (topScores.size() + 1) - 5;
        bottomLine->drawLine(Vec2(60, bottomY), Vec2(visibleSize.width - 60, bottomY), Color4F(1, 1, 1, 0.3f));
        this->addChild(bottomLine);

    }
    else {
        // 无数据时的显示
        auto noDataLabel = Label::createWithSystemFont("No game records yet", "Arial", 20);
        noDataLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 320));
        noDataLabel->setColor(Color3B::WHITE);
        this->addChild(noDataLabel);

        auto hintLabel = Label::createWithSystemFont("Play some songs to see your rankings!", "Arial", 16);
        hintLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 350));
        hintLabel->setColor(Color3B(150, 150, 150));
        this->addChild(hintLabel);
    }
}

void ScoreStatisticsScene::createChartSection(const Size& visibleSize) {
    // 折线图标题背景
    auto chartTitleBg = LayerColor::create(Color4B(80, 80, 120, 220), visibleSize.width - 80, 30);
    chartTitleBg->setPosition(40, visibleSize.height - 440);
    this->addChild(chartTitleBg);

    auto chartTitle = Label::createWithSystemFont("PROGRESS TRACKING", "Arial", 20);
    chartTitle->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 425));
    chartTitle->setColor(Color3B(255, 255, 100));
    this->addChild(chartTitle);

    // 折线图背景
    auto chartBg = LayerColor::create(Color4B(0, 0, 0, 120), visibleSize.width - 80, 200);
    chartBg->setPosition(40, visibleSize.height - 640);
    this->addChild(chartBg);

    // 创建折线图 - 使用新的createWithSize函数
    auto scoreChart = ScoreChart::createWithSize(Size(visibleSize.width - 120, 160));
    if (scoreChart) {
        scoreChart->setPosition(60, visibleSize.height - 630);
        this->addChild(scoreChart);
    }

    // 图表说明
    auto explanation = Label::createWithSystemFont(
        "Green = Improvement, Red = Decline | Shows cumulative average score progression",
        "Arial", 12
    );
    explanation->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 650));
    explanation->setColor(Color3B(180, 180, 180));
    this->addChild(explanation);

    // 分隔线 - 区分上下部分
    auto sectionSeparator = DrawNode::create();
    sectionSeparator->drawLine(
        Vec2(40, visibleSize.height - 420),
        Vec2(visibleSize.width - 40, visibleSize.height - 420),
        Color4F(1, 1, 0, 0.5f)
    );
    this->addChild(sectionSeparator);
}
