#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include <iostream>
using namespace std;

USING_NS_CC;

void menuItemClicked();

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	////Basic Label
	//auto myLabel = Label::createWithTTF("Your Text", "fonts/Marker Felt.ttf", 24);
	//myLabel->setPosition(Vec2(200, 100));
	//this->addChild(myLabel);

	////System label
	//auto myLabel2 = Label::createWithSystemFont("My Label Text", "Arial", 16);
	//myLabel2->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	//this->addChild(myLabel2);

	//label with font TTFConfig

	/*TTFConfig labelConfig;
	labelConfig.fontFilePath = "/fonts/Marker Felt.ttf";
	labelConfig.fontSize = 16;
	labelConfig.glyphs = GlyphCollection::DYNAMIC;
	labelConfig.outlineSize = 0;
	labelConfig.customGlyphs = nullptr;
	labelConfig.distanceFieldEnabled = false;

	auto myLabel = Label::createWithTTF(labelConfig, "My Label Text");
	myLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	myLabel->enableOutline(Color4B::BLUE, 1);
	this->addChild(myLabel);

	auto myLabel2 = Label::createWithTTF(labelConfig, "My Label Text 2");
	myLabel2->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 100));
	myLabel2->enableOutline(Color4B::BLUE, 1);
	this->addChild(myLabel2);*/

	//********MENUS*********
	Vector<MenuItem*> menuItems;

	auto menuItemLabel1 = Label::createWithTTF("Menu Item 1", "fonts/Marker Felt.ttf", 24);
	auto menuItem1 = MenuItemLabel::create(menuItemLabel1,
		[&](Ref* sender) {
		//Code here
			menuItemClicked();
		}
	);
	menuItem1->setPosition(Vec2(menuItem1->getPosition().x, menuItem1->getPosition().y + 30));

	auto menuItemLabel2 = Label::createWithTTF("Menu Item 2", "fonts/Marker Felt.ttf", 24);
	auto menuItem2 = MenuItemLabel::create(menuItemLabel2);
	menuItem2->setPosition(Vec2(menuItem2->getPosition().x, menuItem1->getPosition().y - 30));

	auto menuItemLabel3 = Label::createWithTTF("Menu Item 3", "fonts/Marker Felt.ttf", 24);
	auto menuItem3 = MenuItemLabel::create(menuItemLabel3);
	menuItem3->setPosition(Vec2(menuItem2->getPosition().x, menuItem2->getPosition().y - 30));
	//close button
	auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
	closeItem->setAnchorPoint(Vec2(1, 1));
	closeItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

	menuItems.pushBack(menuItem1);
	menuItems.pushBack(menuItem2);
	menuItems.pushBack(menuItem3);
	menuItems.pushBack(closeItem);

	auto menu = Menu::createWithArray(menuItems);
	this->addChild(menu, 1);


    return true;
}

void menuItemClicked()
{
	log("Menu Item 1 clocked");
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
    
    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
    
    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
    
    
}
