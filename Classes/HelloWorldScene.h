#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    void menuCloseCallback(cocos2d::Ref* pSender);

    void gamepick();
    
    void gameset(Ref* pSender);//ÉèÖÃ

    void gamesta(Ref* pSender);//Í³¼Æ

    void gamemyself(Ref* pSender);//Í³¼Æ

    void keyconfig(cocos2d::Ref* pSender);//Create new KeyConfig button

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
};

#endif // __HELLOWORLD_SCENE_H__
