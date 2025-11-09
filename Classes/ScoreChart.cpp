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

void ScoreChart::drawBackground() {
    // 图表背景
    auto bg = LayerColor::create(Color4B(20, 20, 40, 200), _chartSize.width, _chartSize.height);
    bg->setPosition(0, 0);
    this->addChild(bg);
    
    // 边框
    auto border = DrawNode::create();
    border->drawRect(Vec2(0, 0), Vec2(_chartSize.width, _chartSize.height), Color4F(0.8f, 0.8f, 0.8f, 0.6f));
    this->addChild(border);
}

void ScoreChart::drawGrid() {
    auto drawNode = DrawNode::create();
    
    // 水平网格线
    int horizontalLines = 4;
    for (int i = 0; i <= horizontalLines; i++) {
        float y = (i * _chartSize.height) / horizontalLines;
        drawNode->drawLine(Vec2(0, y), Vec2(_chartSize.width, y), Color4F(1, 1, 1, 0.15f));
    }
    
    // 垂直网格线（如果数据点足够多）
    if (_dataPointCount > 1) {
        int verticalLines = std::min(_dataPointCount - 1, 8);
        for (int i = 0; i <= verticalLines; i++) {
            float x = (i * _chartSize.width) / verticalLines;
            drawNode->drawLine(Vec2(x, 0), Vec2(x, _chartSize.height), Color4F(1, 1, 1, 0.15f));
        }
    }
    
    this->addChild(drawNode);
}

void ScoreChart::drawLineChart() {
    if (_dataPointCount < 1) {
        // 没有数据时显示提示
        auto noDataLabel = Label::createWithSystemFont("No enough data for chart", "Arial", 14);
        noDataLabel->setPosition(Vec2(_chartSize.width/2, _chartSize.height/2));
        noDataLabel->setColor(Color3B(150, 150, 150));
        this->addChild(noDataLabel);
        return;
    }
    
    auto drawNode = DrawNode::create();
    
    if (_dataPointCount == 1) {
        // 只有一个数据点时显示点
        float x = _chartSize.width / 2;
        float y = ((_scoreHistory[0] - _minScore) / (_maxScore - _minScore)) * _chartSize.height;
        
        drawNode->drawDot(Vec2(x, y), 6, Color4F(0, 1, 0, 1.0f));
        
        // 显示数值
        char scoreText[32];
        sprintf(scoreText, "%.2f", _scoreHistory[0]);
        auto scoreLabel = Label::createWithSystemFont(scoreText, "Arial", 12);
        scoreLabel->setPosition(Vec2(x + 25, y));
        scoreLabel->setColor(Color3B::WHITE);
        this->addChild(scoreLabel);
        
    } else {
        // 多个数据点时绘制折线
        for (int i = 0; i < _dataPointCount - 1; i++) {
            float x1 = (i * _chartSize.width) / (_dataPointCount - 1);
            float y1 = ((_scoreHistory[i] - _minScore) / (_maxScore - _minScore)) * _chartSize.height;
            
            float x2 = ((i + 1) * _chartSize.width) / (_dataPointCount - 1);
            float y2 = ((_scoreHistory[i + 1] - _minScore) / (_maxScore - _minScore)) * _chartSize.height;
            
            // 使用渐变色：分数上升为绿色，下降为红色
            Color4F lineColor;
            float lineWidth = 2.5f;
            
            if (_scoreHistory[i + 1] >= _scoreHistory[i]) {
                lineColor = Color4F(0.2f, 0.8f, 0.2f, 1.0f); // 绿色 - 进步
            } else {
                lineColor = Color4F(0.8f, 0.2f, 0.2f, 1.0f); // 红色 - 退步
            }
            
            drawNode->drawLine(Vec2(x1, y1), Vec2(x2, y2), lineColor);
            
            // 绘制数据点
            drawNode->drawDot(Vec2(x1, y1), 4, Color4F(1, 1, 1, 1.0f));
            drawNode->drawDot(Vec2(x2, y2), 4, Color4F(1, 1, 1, 1.0f));
            
            // 在第一个点和最后一个点显示数值
            if (i == 0) {
                char scoreText[32];
                sprintf(scoreText, "Start: %.2f", _scoreHistory[i]);
                auto startLabel = Label::createWithSystemFont(scoreText, "Arial", 10);
                startLabel->setPosition(Vec2(x1 + 30, y1));
                startLabel->setColor(Color3B(200, 200, 200));
                this->addChild(startLabel);
            }
            
            if (i == _dataPointCount - 2) {
                char scoreText[32];
                sprintf(scoreText, "Now: %.2f", _scoreHistory[i + 1]);
                auto currentLabel = Label::createWithSystemFont(scoreText, "Arial", 10);
                currentLabel->setPosition(Vec2(x2 - 30, y2 + 15));
                currentLabel->setColor(Color3B::WHITE);
                this->addChild(currentLabel);
            }
        }
        
        // 绘制趋势线（线性回归）
        if (_dataPointCount >= 3) {
            drawTrendLine(drawNode);
        }
    }
    
    this->addChild(drawNode);
}

void ScoreChart::drawTrendLine(DrawNode* drawNode) {
    // 计算线性回归
    float sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    int n = _dataPointCount;
    
    for (int i = 0; i < n; i++) {
        float x = (float)i / (n - 1); // 归一化x值
        float y = (_scoreHistory[i] - _minScore) / (_maxScore - _minScore); // 归一化y值
        
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    float slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    float intercept = (sumY - slope * sumX) / n;
    
    // 绘制趋势线
    float x1 = 0;
    float y1 = intercept;
    float x2 = 1.0f;
    float y2 = slope + intercept;
    
    // 转换为实际坐标
    float actualX1 = x1 * _chartSize.width;
    float actualY1 = y1 * _chartSize.height;
    float actualX2 = x2 * _chartSize.width;
    float actualY2 = y2 * _chartSize.height;
    
    // 绘制虚线趋势线
    drawDashedLine(drawNode, Vec2(actualX1, actualY1), Vec2(actualX2, actualY2), Color4F(1, 1, 1, 0.6f), 2.0f);
}

void ScoreChart::drawLabels() {
    // Y轴标签（分数）
    int yLabels = 4;
    for (int i = 0; i <= yLabels; i++) {
        float scoreValue = _minScore + (i * (_maxScore - _minScore)) / yLabels;
        char scoreText[32];
        sprintf(scoreText, "%.1f", scoreValue);
        
        auto scoreLabel = Label::createWithSystemFont(scoreText, "Arial", 10);
        float y = (i * _chartSize.height) / yLabels;
        scoreLabel->setPosition(Vec2(25, y));
        scoreLabel->setColor(Color3B(200, 200, 200));
        this->addChild(scoreLabel);
        
        // Y轴刻度线
        auto tickDraw = DrawNode::create();
        tickDraw->drawLine(Vec2(0, y), Vec2(5, y), Color4F(1, 1, 1, 0.5f));
        this->addChild(tickDraw);
    }
    
    // X轴标签（游戏次数）
    if (_dataPointCount > 1) {
        int xLabels = std::min(_dataPointCount - 1, 5);
        for (int i = 0; i <= xLabels; i++) {
            int dataIndex = (i * (_dataPointCount - 1)) / xLabels;
            if (dataIndex < _timeLabels.size() && !_timeLabels[dataIndex].empty()) {
                auto timeLabel = Label::createWithSystemFont(_timeLabels[dataIndex], "Arial", 9);
                float x = (dataIndex * _chartSize.width) / (_dataPointCount - 1);
                timeLabel->setPosition(Vec2(x, -12));
                timeLabel->setColor(Color3B(150, 150, 150));
                this->addChild(timeLabel);
                
                // X轴刻度线
                auto tickDraw = DrawNode::create();
                tickDraw->drawLine(Vec2(x, 0), Vec2(x, 5), Color4F(1, 1, 1, 0.5f));
                this->addChild(tickDraw);
            }
        }
    }
    
    // 轴标题
    auto yAxisTitle = Label::createWithSystemFont("Score", "Arial", 10);
    yAxisTitle->setPosition(Vec2(15, _chartSize.height/2));
    yAxisTitle->setRotation(-90);
    yAxisTitle->setColor(Color3B(200, 200, 255));
    this->addChild(yAxisTitle);
    
    auto xAxisTitle = Label::createWithSystemFont("Game Session", "Arial", 10);
    xAxisTitle->setPosition(Vec2(_chartSize.width/2, -20));
    xAxisTitle->setColor(Color3B(200, 200, 255));
    this->addChild(xAxisTitle);
    
    // 图例说明
    if (_dataPointCount >= 2) {
        auto legend = Label::createWithSystemFont("Green=Progress  Red=Decline", "Arial", 9);
        legend->setPosition(Vec2(_chartSize.width - 80, _chartSize.height - 10));
        legend->setColor(Color3B(180, 180, 180));
        this->addChild(legend);
    }
}

