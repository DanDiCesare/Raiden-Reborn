#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

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
	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	auto closeItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
		origin.y + closeItem->getContentSize().height / 2));

	// create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	/////////////////////////////
	// 3. add your codes below...

	// add a label shows "Hello World"
	// create and initialize a label
	auto sprBlueGuy = Sprite::create("Blue_Front1.png");
	this->addChild(sprBlueGuy, 0);
	sprBlueGuy->setPosition(Vec2(200, 100));
	float width = sprBlueGuy->getContentSize().width;
	float height = sprBlueGuy->getContentSize().height;

	Vector<SpriteFrame*> sprBlueGuy_animFrames;
	sprBlueGuy_animFrames.reserve(4);
	sprBlueGuy_animFrames.pushBack(SpriteFrame::create("Blue_Front1.png", Rect(0, 0, width, height)));
	sprBlueGuy_animFrames.pushBack(SpriteFrame::create("Blue_Front2.png", Rect(0, 0, width, height)));
	sprBlueGuy_animFrames.pushBack(SpriteFrame::create("Blue_Front3.png", Rect(0, 0, width, height)));
	sprBlueGuy_animFrames.pushBack(SpriteFrame::create("Blue_Left1.png", Rect(0, 0, width, height)));
	sprBlueGuy_animFrames.pushBack(SpriteFrame::create("Blue_Left2.png", Rect(0, 0, width, height)));
	sprBlueGuy_animFrames.pushBack(SpriteFrame::create("Blue_Left3.png", Rect(0, 0, width, height)));
	sprBlueGuy_animFrames.pushBack(SpriteFrame::create("Blue_Back1.png", Rect(0, 0, width, height)));
	sprBlueGuy_animFrames.pushBack(SpriteFrame::create("Blue_Back2.png", Rect(0, 0, width, height)));
	sprBlueGuy_animFrames.pushBack(SpriteFrame::create("Blue_Back3.png", Rect(0, 0, width, height)));
	sprBlueGuy_animFrames.pushBack(SpriteFrame::create("Blue_Right1.png", Rect(0, 0, width, height)));
	sprBlueGuy_animFrames.pushBack(SpriteFrame::create("Blue_Right2.png", Rect(0, 0, width, height)));
	sprBlueGuy_animFrames.pushBack(SpriteFrame::create("Blue_Right3.png", Rect(0, 0, width, height)));

	auto sprBlueGuy_animation = Animation::createWithSpriteFrames(sprBlueGuy_animFrames, 0.1f);
	auto sprBlueGuy_animate = Animate::create(sprBlueGuy_animation);
	sprBlueGuy->runAction(RepeatForever::create(sprBlueGuy_animate));

	return true;
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
