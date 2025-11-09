#include "GamePick.h"
#include "MenuPick_1.h"
#include "MenuPick_2.h"
#include "MenuPick_3.h"
#include "MenuPick_Custom.h"
#include <cocos/editor-support/cocostudio/SimpleAudioEngine.h>
using namespace CocosDenshion;

USING_NS_CC;

std::string Diff;
std::string Filename;

Scene* GamePick::createScene()
{
	return GamePick::create();
}

bool GamePick::init()
{
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();

	//背景
	auto Backlayer = LayerColor::create(Color4B::WHITE);
	this->addChild(Backlayer, 0, 0);
	auto Main_interface = Sprite::create("Picture/Pickbackground.png");
	Main_interface->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	Backlayer->addChild(Main_interface, 1);


	//菜单,layer的继承类
	auto Menuitem = Sprite::create("Picture/TextMeun 1.png");
	auto MenuItem1 = MenuItemSprite::create(Menuitem, Menuitem, Menuitem);
	MenuItem1->initWithCallback(CC_CALLBACK_1(GamePick::menupick_1, this));
	Menuitem = Sprite::create("Picture/TextMeun 2.png");
	auto MenuItem2 = MenuItemSprite::create(Menuitem, Menuitem, Menuitem,
		CC_CALLBACK_1(GamePick::menupick_2, this));
	Menuitem = Sprite::create("Picture/TextMeun 3.png");
	auto MenuItem3 = MenuItemSprite::create(Menuitem, Menuitem, Menuitem,
		CC_CALLBACK_1(GamePick::menupick_3, this));
	Menuitem = Sprite::create("Picture/TextMeun_Custom.png");
	auto MenuItem4 = MenuItemSprite::create(Menuitem, Menuitem, Menuitem,
		CC_CALLBACK_1(GamePick::menupick_custom, this));
	
	//改的时候记得在这里加上Menuitem4
	auto Menu = Menu::create(MenuItem1, MenuItem2, MenuItem3, MenuItem4, NULL);
	

	//菜单初始设为Zero，菜单项设置的是相对位置
	Menu->setPosition(Vec2(0, 0));
	MenuItem1->setAnchorPoint(Vec2(0.5, 0.5));
	MenuItem2->setAnchorPoint(Vec2(0.5, 0.5));
	MenuItem3->setAnchorPoint(Vec2(0.5, 0.5));

	float spacing = 600.0f; // 按钮之间的间距
	MenuItem2->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	MenuItem1->setPosition(MenuItem2->getPosition() + Vec2(-spacing, 0));
	MenuItem3->setPosition(MenuItem2->getPosition() + Vec2(spacing, 0));
	MenuItem4->setPosition(MenuItem2->getPosition() + Vec2(2 * spacing, 0)); // 第四个按钮在右边
	this->addChild(Menu, 1, 1);

	// creating a keyboard event listener
	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {

		if (keyCode == EventKeyboard::KeyCode::KEY_ENTER)
		{
			SimpleAudioEngine::getInstance()->playEffect("Music file/Pick.mp3");
		}

	};
	listener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {

		if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
		{
			SimpleAudioEngine::getInstance()->playEffect("Music file/Pick.mp3"); 
			backmeun();
		}
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	// 鼠标滚轮监听
	auto mouseListener = EventListenerMouse::create();
	mouseListener->onMouseScroll = [=](Event* event) {
		EventMouse* e = (EventMouse*)event;
		float scrollY = e->getScrollY();  // 鼠标滚轮方向（向上为正，向下为负）

		// 获取三个菜单按钮
		auto menu = this->getChildByTag(1);
		if (!menu) return;

		auto items = menu->getChildren();

		// 定义移动步长
		float moveStep = 100.0f * scrollY; // 每次滚轮滚动移动的像素
		float minX = visibleSize.width / 2 - 600; // 左边界
		float maxX = visibleSize.width / 2 + 1200; // 右边界

		// 计算中间按钮（MenuItem2）新位置
		auto centerItem = items.at(1); // MenuItem2
		Vec2 newPos = centerItem->getPosition() + Vec2(moveStep, 0);

		// 限制范围
		if (newPos.x < visibleSize.width / 2 - 600) newPos.x = visibleSize.width / 2 - 600;
		if (newPos.x > visibleSize.width / 2 ) newPos.x = visibleSize.width / 2 ;

		// 计算偏移量（用于同步其他按钮）
		float delta = newPos.x - centerItem->getPositionX();

		// 同步移动三个按钮
		for (auto child : items)
		{
			child->setPosition(child->getPosition() + Vec2(delta, 0));
		}
		};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

	return true;
}

void GamePick::menuCloseCallback(Ref* pSender)
{
	Director::getInstance()->end();
}

void GamePick::backmeun()
{
	Director::getInstance()->popScene();
}

void GamePick::menupick_1(Ref* pSender)
{
	Scene* scene = MenuPick_1::createScene();
	Director::getInstance()->pushScene(TransitionFadeBL::create(1.5, scene));
}

void GamePick::menupick_2(Ref* pSender)
{
	Scene* scene = MenuPick_2::createScene();
	Director::getInstance()->pushScene(TransitionFadeBL::create(1.5, scene));
}

void GamePick::menupick_3(Ref* pSender)
{
	Scene* scene = MenuPick_3::createScene();
	Director::getInstance()->pushScene(TransitionFadeBL::create(1.5, scene));
}


void GamePick::menupick_custom(Ref* pSender)
{
	Scene* scene = MenuPick_Custom::createScene();
	Director::getInstance()->pushScene(TransitionFadeBL::create(1.5, scene));
}
