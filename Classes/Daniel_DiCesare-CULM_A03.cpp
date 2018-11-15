#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include <iostream>
using namespace std;

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

	auto sprBlueGuy = Sprite::create("Blue_Front1.png");
	this->addChild(sprBlueGuy, 0);
	sprBlueGuy->setPosition(Vec2(200, 100));

	auto sprBlueGuy_moveBy = MoveBy::create(5, Vec2(200, 200));
	auto sprBlueGuy_moveEase = EaseElasticOut::create(sprBlueGuy_moveBy);
	sprBlueGuy->runAction(sprBlueGuy_moveEase);
	sprBlueGuy->runAction(sprBlueGuy_moveEase->reverse());

	//Print to console
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	cout << "Origin x: " << origin.x << endl;
	cout << "Origin y: " << origin.y << endl;
	cout << "Visible width: " << visibleSize.width << endl;
	cout << "Visible height: " << visibleSize.height << endl;

	//Sequence
	auto sprBlueGuy2 = Sprite::create("Blue_Front1.png");
	this->addChild(sprBlueGuy2, 0);
	sprBlueGuy2->setPosition(Vec2(200, 100));

	auto sprBlueGuy2_jump = JumpBy::create(0.5, Vec2(0, 0), 100, 1);
	auto sprBlueGuy2_rotate = RotateTo::create(2.0f, 10);

	auto sprBlueGuy2_callbackJump = CallFunc::create([]() {
		cout << "Jumped!" << endl;
		cout << "Waiting for next action..." << endl;
	});

	auto sprBlueGuy2_callbackRotate = CallFunc::create([]() {
		cout << "Rotated!" << endl;
		cout << "No more actions!" << endl;
	});

	auto sprBlueGuy2_seq = Sequence::create(sprBlueGuy2_jump, sprBlueGuy2_rotate, sprBlueGuy2_callbackJump, sprBlueGuy2_callbackRotate, nullptr);
	sprBlueGuy2->runAction(sprBlueGuy2_seq);

	//Object 3
	auto sprBlueGuy3 = Sprite::create("Blue_Front1.png");
	this->addChild(sprBlueGuy3, 0);
	sprBlueGuy3->setPosition(Vec2(50, 50));

	auto sprBlueGuy3_jump = JumpBy::create(4.0f, Vec2(0, 0), 100, 1);
	auto sprBlueGuy3_moveBy = MoveBy::create(4.0f, Vec2(100, 0));
	auto sprBlueGuy3_rotate = RotateBy::create(4.0f, 10);
	auto sprBlueGuy3_fadeTo = FadeTo::create(4.0f, 0);
	auto sprBlueGuy3_tintBy = TintBy::create(2.0f, 120.0f, 232.0f, 254.0f);


	/*sprBlueGuy3->runAction(RepeatForever::create(sprBlueGuy3_jump));
	sprBlueGuy3->runAction(RepeatForever::create(sprBlueGuy3_moveBy));
	sprBlueGuy3->runAction(RepeatForever::create(sprBlueGuy3_rotate));
	sprBlueGuy3->runAction(RepeatForever::create(sprBlueGuy3_fadeTo));*/

	//Spawn
	auto sprBlueGuy3_spawn = Spawn::createWithTwoActions(sprBlueGuy3_jump, sprBlueGuy3_moveBy);
	auto sprBlueGuy3_spawn2 = Spawn::createWithTwoActions(sprBlueGuy3_spawn, sprBlueGuy3_rotate);
	auto sprBlueGuy3_spawn3 = Spawn::createWithTwoActions(sprBlueGuy3_spawn2, sprBlueGuy3_tintBy);

	//sprBlueGuy3->runAction(sprBlueGuy3_spawn2);

	auto sprBlueGuy3_seq = Sequence::create(sprBlueGuy3_spawn3, sprBlueGuy3_fadeTo, nullptr);
	sprBlueGuy3->runAction(sprBlueGuy3_seq);

	//Cloning
	auto sprBlueGuy4 = Sprite::create("Blue_Front1.png");
	this->addChild(sprBlueGuy4, 0);
	sprBlueGuy4->setPosition(Vec2(50, 50));

	auto sprBlueGuy5 = Sprite::create("Blue_Front1.png");
	this->addChild(sprBlueGuy5, 0);
	sprBlueGuy5->setPosition(Vec2(50, 50));

	auto sprBlueGuy4_moveBy = MoveBy::create(5, Vec2(200, 100));
	sprBlueGuy4->runAction(sprBlueGuy4_moveBy);
	sprBlueGuy5->runAction(sprBlueGuy4_moveBy->clone());




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
