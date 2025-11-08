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

void ScoreChart::setChartTitle(const std::string& title) {
    // 可以用于动态设置图表标题
    // 当前实现中标题在drawBackground中固定
}

void ScoreChart::calculateChartData() {
    _scoreHistory.clear();
    _timeLabels.clear();
    
    // 获取所有成绩并按时间排序
    auto allScores = ScoreSystem::getInstance()->getTopScores(100); // 获取足够多的记录
    
    // 按时间排序（新的在前）
    std::sort(allScores.begin(), allScores.end(), [](const SongScore& a, const SongScore& b) {
        return a.playTime < b.playTime; // 时间戳小的在前（旧的）
    });
    
    // 计算累计平均分的变化
    float runningTotal = 0.0f;
    int count = 0;
    
    for (const auto& score : allScores) {
        count++;
        runningTotal += score.calculatedScore;
        float currentAverage = runningTotal / count;
        
        _scoreHistory.push_back(currentAverage);
        
        // 生成时间标签（简化显示）
        char timeLabel[32];
        if (count <= 5) {
            sprintf(timeLabel, "#%d", count); // 前5次显示具体编号
        } else if (count == allScores.size()) {
            sprintf(timeLabel, "Now"); // 最后一次显示"Now"
        } else if (count % 5 == 0) {
            sprintf(timeLabel, "#%d", count); // 每5次显示一次编号
        } else {
            sprintf(timeLabel, ""); // 其他情况不显示标签
        }
        _timeLabels.push_back(timeLabel);
    }
    
    _dataPointCount = _scoreHistory.size();
    
    // 计算Y轴范围
    if (!_scoreHistory.empty()) {
        auto minMax = std::minmax_element(_scoreHistory.begin(), _scoreHistory.end());
        _minScore = *minMax.first;
        _maxScore = *minMax.second;
        
        // 添加一些边距
        float range = _maxScore - _minScore;
        if (range < 0.5f) {
            // 如果数据范围太小，扩展范围
            _minScore = std::max(0.0f, _minScore - 0.25f);
            _maxScore = _maxScore + 0.25f;
        } else {
            _minScore = std::max(0.0f, _minScore - range * 0.1f);
            _maxScore = _maxScore + range * 0.1f;
        }
        
        // 确保最小值不为负
        _minScore = std::max(0.0f, _minScore);
    } else {
        // 没有数据时的默认范围
        _minScore = 0.0f;
        _maxScore = 2.0f;
    }
}
