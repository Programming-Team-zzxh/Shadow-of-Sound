#ifndef __SCORE_CHART_H__
#define __SCORE_CHART_H__

#include "cocos2d.h"
#include "ScoreSystem.h"

class ScoreChart : public cocos2d::Node {
public:
    // 使用静态create函数接受参数
    static ScoreChart* createWithSize(const cocos2d::Size& size);

    virtual bool init(const cocos2d::Size& size);
    void updateChartData();
    void setChartTitle(const std::string& title);

private:
    void drawBackground();
    void drawGrid();
    void drawLineChart();
    void drawLabels();
    void calculateChartData();
    void drawTrendLine(cocos2d::DrawNode* drawNode);
    void drawDashedLine(cocos2d::DrawNode* drawNode,
        const cocos2d::Vec2& start,
        const cocos2d::Vec2& end,
        const cocos2d::Color4F& color,
        float dashLength);

    cocos2d::Size _chartSize;
    std::vector<float> _scoreHistory;
    std::vector<std::string> _timeLabels;
    float _minScore, _maxScore;
    int _dataPointCount;

    // 移除 CREATE_FUNC，因为我们需要自定义create函数
};

#endif
