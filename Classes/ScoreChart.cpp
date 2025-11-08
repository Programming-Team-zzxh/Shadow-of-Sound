#include "ScoreChart.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

USING_NS_CC;

ScoreChart* ScoreChart::createWithSize(const Size& size) {
    ScoreChart* chart = new ScoreChart();
    if (chart && chart->init(size)) {
        chart->autorelease();
        return chart;
    }
    CC_SAFE_DELETE(chart);
    return nullptr;
}

bool ScoreChart::init(const Size& size) {
    if (!Node::init()) {
        return false;
    }
    
    _chartSize = size;
    _minScore = 0.0f;
    _maxScore = 3.0f; // 假设最大分数为3.0
    _dataPointCount = 0;
    
    this->setContentSize(size);
    updateChartData();
    
    return true;
}

void ScoreChart::updateChartData() {
    calculateChartData();
    this->removeAllChildren();
    drawBackground();
    drawGrid();
    drawLineChart();
    drawLabels();
}
