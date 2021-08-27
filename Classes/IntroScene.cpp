//
//  IntroScene.cpp
//  FirstGame
//
//  Created by Amides Rodriguez on 2/18/21.
//

#include "IntroScene.h"
#include <cocos2d.h>
#include <CCScheduler.h>

#include <vector>
#include <string>


USING_NS_CC;

const int SPACESHIP_STEP = 30;
const int SPACESHIP_LENGHT = 60;

const std::string ASSETS_BASE_PATH = "../Resources/Spaceship_art_pack/";
std::string AESTROID_BASE_PATH = ASSETS_BASE_PATH + "Aestroids/";
const std::vector<std::string> asteroidsNames {"aestroidgrey.png", "aestroid_brown.png", "aestroid_dark.png", "aestroid_gray_2.png"};

Scene* IntroScene::createScene()
{
    return IntroScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool IntroScene::init()
{
    if ( !Scene::initWithPhysics())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
        
    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create("CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(IntroScene::menuCloseCallback,
                                                         this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }
    
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    
    //Background loading

    auto background = Sprite::create(ASSETS_BASE_PATH + "Background/background.jpg");
    if (!background)
    {
        problemLoading("'background.jpg'");
    }
    else
    {
        background->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
        background->setScale(2.0f, 1.5f);

        this->addChild(background, -1);
    }
    
    //spaceship loading
    auto spaceShip1 = Sprite::create(ASSETS_BASE_PATH + "/Red/Red_Plane_animation/1.png");
    if (!spaceShip1)
    {
        problemLoading("'1.png'");
    }
    else
    {
        spaceShip1->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/8 + origin.y));
        spaceShip1->setContentSize(Size(SPACESHIP_LENGHT, SPACESHIP_LENGHT));
        spaceShip1->setAnchorPoint(Vec2(0.5, 0.5));

        this->addChild(spaceShip1, 0);
    }
    
    auto physicsBody = PhysicsBody::createBox(Size(20.0f, 20.0f));
    physicsBody->setDynamic(false);
    
    spaceShip1->addComponent(physicsBody);
    
    spaceShip1->setTag(11);
    spaceShip1->getPhysicsBody()->setContactTestBitmask(0xffffffff);
    spaceShip1->getPhysicsBody()->setDynamic(false);
    
   // srand((unsigned int)time(nullptr));
    
    auto funPointer = static_cast<cocos2d::SEL_SCHEDULE>(&IntroScene::createAestroids);
    this->schedule(funPointer, 2.5f);
    
    setUpKeyboardEvents(spaceShip1, visibleSize, origin);

    return true;
}

void IntroScene::createAestroids(float time)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    //Aestroid loading
    int randomFileName = rand() % asteroidsNames.size();
    int randomStartPosition = rand() % (int)visibleSize.width;
    
    auto aestroid = Sprite::create(AESTROID_BASE_PATH + asteroidsNames[randomFileName].c_str());
    if (!aestroid)
    {
        problemLoading(asteroidsNames[randomFileName].c_str());
    }
    else
    {
        aestroid->setPosition(Vec2(randomStartPosition + origin.x, visibleSize.height * 0.9 + origin.y));
        aestroid->setScale(0.2f);

        this->addChild(aestroid, 0);
        
        int duration = 4.0;
        auto actionMove = MoveTo::create(duration, Vec2(randomStartPosition + origin.x, origin.y));
        auto actionRemove = RemoveSelf::create();
          
        aestroid->runAction(Sequence::create(actionMove,actionRemove, nullptr));
        
        auto physicsBody = PhysicsBody::createBox(Size(50.0f, 50.0f));
        physicsBody->setDynamic(false);
        
        aestroid->addComponent(physicsBody);
        
        aestroid->setTag(12);
        aestroid->getPhysicsBody()->setContactTestBitmask(0xffffffff);
        aestroid->getPhysicsBody()->setDynamic(false);

        //adds contact event listener
        auto contactListener = EventListenerPhysicsContact::create();
        contactListener->onContactBegin = CC_CALLBACK_1(IntroScene::onContactBegin, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    }
}

bool IntroScene::onContactBegin(cocos2d::PhysicsContact& contact)
{
    //space ship 11
    //aesteroid 12
    //bullets 10
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();

    if (nodeA && nodeB)
    {
        if (nodeA->getTag() == 11 && nodeB->getTag() == 12)
        {
            nodeA->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/8 + origin.y));
            nodeB->removeFromParentAndCleanup(true);
        }
        else if (nodeA->getTag() == 12 && nodeB->getTag() == 11)
        {
            nodeB->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/8 + origin.y));
            nodeA->removeFromParentAndCleanup(true);
        }
        else if ((nodeA->getTag() == 10 && nodeB->getTag() == 12) || (nodeA->getTag() == 12 && nodeB->getTag() == 10))
        {
            nodeB->removeFromParentAndCleanup(true);
            nodeA->removeFromParentAndCleanup(true);
        }
        else {
            return false;
        }
        
        return true;
    }

    return false;
}


void IntroScene::setUpKeyboardEvents(Sprite* sprite, Size visibleSize, Vec2 origin)
{
    int halfWidth = SPACESHIP_LENGHT / 2;
    
    auto eventListener = EventListenerKeyboard::create();
    eventListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        
        Vec2 loc = event->getCurrentTarget()->getPosition();
        
        switch(keyCode){
            case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            case EventKeyboard::KeyCode::KEY_A:
            {
                if (loc.x - SPACESHIP_STEP <= origin.x + halfWidth)
                {
                    loc.x = origin.x + halfWidth;
                }
                else
                {
                    loc.x = loc.x - SPACESHIP_STEP;
                }
                
                event->getCurrentTarget()->setPosition(loc.x , loc.y);
            }
            break;
            case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            case EventKeyboard::KeyCode::KEY_D:
            {
                if (loc.x + SPACESHIP_STEP >= visibleSize.width - halfWidth)
                {
                    loc.x = visibleSize.width;
                }
                else
                {
                    loc.x = loc.x + SPACESHIP_STEP;
                }
                
                event->getCurrentTarget()->setPosition(loc.x , loc.y);
            }
            break;
            case EventKeyboard::KeyCode::KEY_SPACE:
            {
                auto spaceShipPos = event->getCurrentTarget()->getPosition();
                createBullets(spaceShipPos);
            }
            break;
            default:
                break;
        }
    };
        
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener,sprite);
}

void IntroScene::createBullets(const cocos2d::Vec2& shipPosition)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto fireBullet = Sprite::create(ASSETS_BASE_PATH + "/Red/bullet_red.png");
    if (!fireBullet)
    {
        problemLoading("'bullet_red.png'");
    }
    else
    {
        fireBullet->setPosition(Vec2(shipPosition.x, shipPosition.y + 1.0f));
        fireBullet->setContentSize(Size(SPACESHIP_LENGHT / 2.0, SPACESHIP_LENGHT / 2.0));
        fireBullet->setAnchorPoint(Vec2(0.5, 0.5));

        this->addChild(fireBullet, 0);
        
        int duration = 2.5;
        auto actionMove = MoveTo::create(duration, Vec2(shipPosition.x, visibleSize.height));
        auto actionRemove = RemoveSelf::create();
          
        fireBullet->runAction(Sequence::create(actionMove,actionRemove, nullptr));
        
        auto physicsBody = PhysicsBody::createBox(Size(20.0f, 20.0f));
        physicsBody->setDynamic(false);
        
        fireBullet->addComponent(physicsBody);
        
        fireBullet->setTag(10);
        fireBullet->getPhysicsBody()->setContactTestBitmask(0xffffffff);
        fireBullet->getPhysicsBody()->setDynamic(false);
    }
}

void IntroScene::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();
}
