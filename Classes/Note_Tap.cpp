#include "Note_Tap.h"
#include "GamePlay.h"
#include <cocos/editor-support/cocostudio/SimpleAudioEngine.h>
using namespace CocosDenshion;

extern int Note_strack[4];//4条轨道
extern bool Play_TimeStop;

Note::Note(int x, int y, float speed)
{
	Note_x = x;//Track轨道
	Note_y = y;//音符种类
	Note_speed = speed;//速度
}

void Note::Note_down()
{

	//auto moveTo = MoveTo::create(2.0f, Vec3(250, 0, 1));
	//Sequence动作按顺序执行，CallFunc回调函数：移动动作完成后结束
	//this->runAction(Sequence::create(moveTo, CallFunc::create(CC_CALLBACK_0(Note::Note_end, this)), NULL));
	//增加一个定时器，更新状态
	this->schedule(CC_SCHEDULE_SELECTOR(Note::Note_update));

}

void Note::Note_end()
{

	this->removeFromParentAndCleanup(true);
	//调用remove与parent分离，并且clear自己
	//如果直接delete的话，会报错要删除的对象当前还是正在运行状态
	delete this;

}

void Note::Note_update(float dt)
{	
	if (Play_TimeStop)return;

	int X = Note_x;
	int Speed = Note_speed;
	this->setPosition(this->getPosition() + Vec2(0, -Note_speed));
	
	//PERFECT
	//判定区间80+-ms，9.6帧
	if (this->getPositionY() <= (165 + 9.6 * Speed) && this->getPositionY() >= (165 - 9.6 * Speed))
	{
		if ((Note_strack[0] == 1 && Note_x == 1)|| (Note_strack[1] == 1 && Note_x == 2) || 
			(Note_strack[2] == 1 && Note_x == 3) || (Note_strack[3] == 1 && Note_x == 4))
		{
			//播放音效
			SimpleAudioEngine::getInstance()->playEffect("Music file/Tap_Perfect.mp3");
			//动作特效
			auto Note_layer = (LayerColor*)this->getParent();
			auto Icon = Sprite::create("Note icon/Note_Great.png");
			Icon->setPosition(Vec2(Director::getInstance()->
				getVisibleSize().width / 2 - 380 + X * 152, 148));
			Icon->setOpacity(0);
			Note_layer->addChild(Icon, 2);
			auto fade_1 = FadeTo::create(0.3f, 200);
			auto rota = RotateTo::create(0.3f, 90);
			auto spawn = Spawn::create(fade_1, rota, NULL);
			auto keep = FadeTo::create(0.5f, 200);
			auto fade_2 = FadeTo::create(0.8f, 0);
			//Sequence动作按顺序执行，CallFunc回调函数：移动动作完成后结束
			Icon->runAction(Sequence::create(spawn, keep, fade_2, CallFunc::create(CC_CALLBACK_0(
				Sprite::removeFromParent, Icon)), NULL));
			//1. 创建新图片精灵（替换为你的新图片路径）
			auto NewIcon = Sprite::create("Note icon/Note_Great1.png"); // 新图片路径
			//2. 设置位置
			NewIcon->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2, 900));
			//3. 初始状态：透明 + 轻微缩小（0.7倍，用于放大效果）
			NewIcon->setOpacity(0);
			NewIcon->setScale(0.7f);
			//4. 添加到图层（z轴设为3，确保在现有元素上方）
			Note_layer->addChild(NewIcon, 3);
			
			//5. 定义新图片的动画：淡入 + 放大
			auto newFadeIn = FadeTo::create(0.2f, 255); // 0.2秒淡入（比现有稍快）
			auto newScaleUp = ScaleTo::create(0.2f, 1.0f); // 0.2秒从0.7放大到1.0（微微放大）
			auto newSpawn = Spawn::create(newFadeIn, newScaleUp, NULL); // 同步执行
			//6. 保持显示一段时间后淡出
			auto newKeep = DelayTime::create(0.4f); // 保持0.4秒
			auto newFadeOut = FadeTo::create(0.6f, 0); // 0.6秒淡出
			//7. 执行动画后移除
			NewIcon->runAction(Sequence::create(
				newSpawn,
				newKeep,
				newFadeOut,
				CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, NewIcon)),
				NULL
			));
			//粒子特效
			auto Particle = ParticleSystemQuad::create("Particle/Note_Perfect.plist");
			Particle->setPosition(Point(Director::getInstance()->
				getVisibleSize().width / 2 - 380 + X * 152, this->getPositionY() - 8));
			Note_layer->addChild(Particle, 1);
			//加分！找到父节点的父节点，也就是GamePlay
			auto PlayFather = ((GamePlay*)(LayerColor*)this->getParent()->getParent());
			PlayFather->Play_Perfect++;
			PlayFather->Play_Combo++;
			PlayFather->Play_Score = PlayFather->Play_Score + PlayFather->Play_GetScore;
			this->removeFromParentAndCleanup(true);
			delete this;
		}
	}
	//GRAET
	//判定区间200+-ms，24帧
	else if (this->getPositionY() <= (165 + 24 * Speed) && this->getPositionY() >= (165 - 24 * Speed))
	{
		if ((Note_strack[0] == 1 && Note_x == 1) || (Note_strack[1] == 1 && Note_x == 2) ||
			(Note_strack[2] == 1 && Note_x == 3) || (Note_strack[3] == 1 && Note_x == 4))
		{
			SimpleAudioEngine::getInstance()->playEffect("Music file/Tap_Good.mp3");

			auto Note_layer = (LayerColor*)this->getParent();
			auto Icon = Sprite::create("Note icon/Note_Good.png");
			Icon->setPosition(Vec2(Director::getInstance()->
				getVisibleSize().width / 2 - 380 + X * 152, 148));
			Icon->setOpacity(0);
			Note_layer->addChild(Icon, 2);
			auto fade_1 = FadeTo::create(0.3f, 200);
			auto rota = RotateTo::create(0.3f, 90);
			auto spawn = Spawn::create(fade_1, rota, NULL);
			auto keep = FadeTo::create(0.5f, 200);
			auto fade_2 = FadeTo::create(0.8f, 0);
			Icon->runAction(Sequence::create(spawn, keep, fade_2, CallFunc::create(CC_CALLBACK_0(
				Sprite::removeFromParent, Icon)), NULL));

			//1. 创建新图片精灵（替换为你的新图片路径）
			auto NewIcon = Sprite::create("Note icon/Note_Good1.png"); // 新图片路径
			//2. 设置位置
			NewIcon->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2, 900));
			//3. 初始状态：透明 + 轻微缩小（0.7倍，用于放大效果）
			NewIcon->setOpacity(0);
			NewIcon->setScale(0.7f);
			//4. 添加到图层（z轴设为3，确保在现有元素上方）
			Note_layer->addChild(NewIcon, 3);

			//5. 定义新图片的动画：淡入 + 放大
			auto newFadeIn = FadeTo::create(0.2f, 255); // 0.2秒淡入（比现有稍快）
			auto newScaleUp = ScaleTo::create(0.2f, 1.0f); // 0.2秒从0.7放大到1.0（微微放大）
			auto newSpawn = Spawn::create(newFadeIn, newScaleUp, NULL); // 同步执行
			//6. 保持显示一段时间后淡出
			auto newKeep = DelayTime::create(0.4f); // 保持0.4秒
			auto newFadeOut = FadeTo::create(0.6f, 0); // 0.6秒淡出
			//7. 执行动画后移除
			NewIcon->runAction(Sequence::create(
				newSpawn,
				newKeep,
				newFadeOut,
				CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, NewIcon)),
				NULL
			));

			auto Particle = ParticleSystemQuad::create("Particle/Note_Good.plist");
			Particle->setPosition(Point(Director::getInstance()->
				getVisibleSize().width / 2 - 380 + X * 152, this->getPositionY() - 8));
			Note_layer->addChild(Particle, 1);
			auto PlayFather = ((GamePlay*)(LayerColor*)this->getParent()->getParent());
			PlayFather->Play_Good++;
			PlayFather->Play_Combo++;
			PlayFather->Play_Score = PlayFather->Play_Score + PlayFather->Play_GetScore * 0.65;
			this->removeFromParentAndCleanup(true);
			delete this;
		}
	}
	//BAD
	else if (this->getPositionY() < (165 - 24 * Speed))
	{
		((GamePlay*)(LayerColor*)this->getParent()->getParent())->Play_Pass++;
		((GamePlay*)(LayerColor*)this->getParent()->getParent())->Play_Combo = 0;
		this->removeFromParentAndCleanup(true);
		//调用remove与parent分离，并且clear自己
		//如果直接delete的话，会报错要删除的对象当前还是正在运行状态
		delete this;
	}		
	//在按下按键后锁定
	if (Note_strack[X - 1] == 1)
	{
		Note_strack[X - 1] = -1;
	}


}

