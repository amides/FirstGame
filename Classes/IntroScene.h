#pragma once

#include "cocos2d.h"

class IntroScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    void createAestroids(float time);
    
    // implement the "static create()" method manually
    CREATE_FUNC(IntroScene);
    
private:
    void setUpKeyboardEvents(cocos2d::Sprite* sprite, cocos2d::Size visibleSize,
                            cocos2d::Vec2 origin);
    
    void createBullets(const cocos2d::Vec2& shipPosition);
    
    bool onContactBegin(cocos2d::PhysicsContact& contact);
};
