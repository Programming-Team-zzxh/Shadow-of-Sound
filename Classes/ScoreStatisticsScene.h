#ifndef __SCORE_STATISTICS_SCENE_H__
#define __SCORE_STATISTICS_SCENE_H__

#include "cocos2d.h"
#include "ScoreSystem.h"

class ScoreStatisticsScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    void backToMenu();
    void refreshDisplay();

private:
    void createStatisticsInfo(const cocos2d::Size& visibleSize);
    void createRankingSection(const cocos2d::Size& visibleSize);
    void createChartSection(const cocos2d::Size& visibleSize);

    CREATE_FUNC(ScoreStatisticsScene);
};

#endif
