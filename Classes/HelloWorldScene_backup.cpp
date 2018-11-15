#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include <iostream>
#include "ui/CocosGUI.h"
#include <ui/UIButton.h>
#include <Windows.h>
#include "SimpleAudioEngine.h"
#include <ctime>
#include <CCScheduler.h>
#include <chrono>
#include <conio.h>
#include <array>
//#include "SneakyButton.h"
//#include "SneakyButtonSkinnedBase.h"
//#include "SneakyJoystickSkinnedBase.h"
//#include "SneakyJoystick.h"

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;
using namespace ui;
USING_NS_CC;

int ScoreUpdate(int &score, int);
Sprite* Missle(int, int, float, char);
Sprite* BlowUp(int, int);
Sprite* SmallBlowUp(int, int);
Sprite* SmallBlowUpWide(int, int);
Sprite* SmallBlowUpBoss();
Sprite* EnemyShip(int, float);
Sprite* EnemyShot1(Sprite*);
Sprite* OneUp(float);
Sprite* EnemyWide(int, float, int, int);
Sprite* EnemyShotWide(Sprite*);
Sprite* Boss(float, float);
Sprite* BossShotPattern1(int, float, float, int);
Sprite* BossIdleR();
Sprite* BossIdleL();
Sprite* BossIdleM();
Sprite* BossCharge();
Sprite* BossLaser();

Scene* GetScene1();
Scene* GameOver(int);
Scene* GameWin(int);
MenuItemImage* GetCloseItem();

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////            TO-DO-LIST										///////////////////
/////////////////////           1. Move Enemies									///////////////////
/////////////////////           2. Time Enemy spawn								///////////////////
/////////////////////           3. Change Controls								///////////////////
/////////////////////           4. background									///////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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


	//********MENUS*********
	Vector<MenuItem*> menuItems;

	auto menuItemLabel1 = Label::createWithTTF("RAIDEN REBORN", "fonts/Marker Felt.ttf", 24);
	auto menuItem1 = MenuItemLabel::create(menuItemLabel1,
		[&](Ref* sender) {
		//Code here
		Scene* scene = GetScene1();
		Director::getInstance()->pushScene(scene);
	}
	);


	//close button
	auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
	closeItem->setAnchorPoint(Vec2(1, 1));
	closeItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	menuItems.pushBack(menuItem1);
	menuItems.pushBack(closeItem);

	auto menu = Menu::createWithArray(menuItems);
	this->addChild(menu, 1);
	this->scheduleUpdate();


	return true;
}


Scene* GetScene1()
{
	//PLAY AUDIO
	auto audio = SimpleAudioEngine::getInstance();
	audio->playBackgroundMusic("soundtrack.mp3", true);

	audio->preloadEffect("shotsound.mp3");
	audio->preloadEffect("explosion.mp3");
	audio->preloadEffect("1up.wav");



	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();

	//auto scene = Scene::create();
	auto scene = Scene::createWithPhysics();

	//add background
	auto sprBackground = Sprite::create("spaceBackgroundV2.jpg");
	sprBackground->setPosition(Vec2((visibleSize.width / 2) - 600, (visibleSize.height / 2) + 200));
	sprBackground->setScale(1);
	//sprBackground->setRotation(180);
	sprBackground->setColor(Color3B(0, 128, 255));
	scene->addChild(sprBackground, -1);
	sprBackground->runAction(MoveBy::create(45, Vec2(0, -1950)));

	////////////////////////////////////////////////////////SPACESHIP//////////////////////////////////////
	auto sprShip = Sprite::create("RaidenShipBase.png");
	//sprShip->setScale(1);
	scene->addChild(sprShip);
	sprShip->setPosition(Vec2(visibleSize.width / 2, 300));
	sprShip->setTag(20);


	///////////////////////////////////////////////////////SHOOTING////////////////////////////////////////
	auto basicShot = Sprite::create("BasicShot.png");
	scene->addChild(basicShot, 0);
	basicShot->setTag(10);


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////COLLISIONS///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Shot Physics found in button


	//Ship Physics
	auto sprShip_physBody = PhysicsBody::createBox(sprShip->getContentSize(), PhysicsMaterial(200.f, 20.0f, 0.0f));
	sprShip_physBody->setGravityEnable(false);
	sprShip_physBody->setContactTestBitmask(0x01);
	sprShip->setPhysicsBody(sprShip_physBody);

	//boundary node
	Node *boundaryNode = Node::create();
	boundaryNode->setTag(1);/////////////////////////////
	boundaryNode->setAnchorPoint(Point(0.5, 0.5));
	boundaryNode->setPosition(Point(visibleSize.width / 2.0, visibleSize.height / 2.0));

	//Physics body for boundary node
	auto boundaryBody = PhysicsBody::createEdgeBox(visibleSize);;
	boundaryBody->setDynamic(false);
	boundaryBody->setContactTestBitmask(0x01);

	//Apply Physics body to the boundary
	boundaryNode->setPhysicsBody(boundaryBody);
	scene->addChild(boundaryNode);

	//Line above boundary
	auto sprBarrier = Sprite::create("lineBoarder.png");
	scene->addChild(sprBarrier);
	sprBarrier->setTag(50);
	sprBarrier->setScale(.372f);
	sprBarrier->setPosition(Vec2(visibleSize.width / 2, 767));
	auto sprBarrier_physBody = PhysicsBody::createBox(sprBarrier->getContentSize(), PhysicsMaterial(7.f, 0.0, 0.0f));
	sprBarrier_physBody->setGravityEnable(false);
	sprBarrier_physBody->setContactTestBitmask(0x01);
	sprBarrier->setPhysicsBody(sprBarrier_physBody);

	//Line below boundary
	auto sprBarrier2 = Sprite::create("lineBoarder.png");
	scene->addChild(sprBarrier2);
	sprBarrier2->setTag(50);
	sprBarrier2->setScale(.372f);
	sprBarrier2->setOpacity(0);
	sprBarrier2->setPosition(Vec2(visibleSize.width / 2, 1));
	auto sprBarrier2_physBody = PhysicsBody::createBox(sprBarrier2->getContentSize(), PhysicsMaterial(7.f, 0.0, 0.0f));
	sprBarrier2_physBody->setGravityEnable(false);
	sprBarrier2_physBody->setContactTestBitmask(0x01);
	sprBarrier2->setPhysicsBody(sprBarrier2_physBody);

	//Line right boundary
	auto sprBarrierR = Sprite::create("lineBoarderUp.png");
	scene->addChild(sprBarrierR);
	sprBarrierR->setOpacity(0);
	sprBarrierR->setTag(51);
	sprBarrierR->setScale(.2f);
	sprBarrierR->setScaleY(2);
	//sprBarrierR->setOpacity(0);
	sprBarrierR->setPosition(Vec2(400, visibleSize.height / 2));
	auto sprBarrierR_physBody = PhysicsBody::createBox(sprBarrierR->getContentSize(), PhysicsMaterial(7.f, 0.0, 0.0f));
	sprBarrierR_physBody->setGravityEnable(false);
	sprBarrierR_physBody->setContactTestBitmask(0x01);
	sprBarrierR->setPhysicsBody(sprBarrierR_physBody);
	//Line left boundary
	auto sprBarrierL = Sprite::create("lineBoarderUp.png");
	scene->addChild(sprBarrierL);
	sprBarrierL->setTag(51);
	sprBarrierL->setScale(.2f);
	sprBarrierL->setScaleY(2);
	sprBarrierL->setOpacity(0);
	sprBarrierL->setPosition(Vec2(32, visibleSize.height / 2));
	auto sprBarrierL_physBody = PhysicsBody::createBox(sprBarrierL->getContentSize(), PhysicsMaterial(7.f, 0.0, 0.0f));
	sprBarrierL_physBody->setGravityEnable(false);
	sprBarrierL_physBody->setContactTestBitmask(0x01);
	sprBarrierL->setPhysicsBody(sprBarrierL_physBody);

	//Score Panel
	auto sprScoreboard = Sprite::create("scoreboard.png");
	scene->addChild(sprScoreboard, 2);
	sprScoreboard->setPosition(300, 753);
	sprScoreboard->setColor(Color3B(96, 96, 96));
	sprScoreboard->setScaleY(0.5);
	//Score
	auto sprScore = Sprite::create("BasicShot.png");
	scene->addChild(sprScore, -2);
	sprScore->setTag(300);

	cocos2d::Label *scoreLabel;
	scoreLabel = Label::createWithTTF(to_string(sprScore->getTag()), "fonts/Marker Felt.ttf", 24);
	scoreLabel->setColor(Color3B::WHITE);
	scoreLabel->setPosition(Point(330, 752));
	scene->addChild(scoreLabel, 10000);

	////Lives
	auto sprLives = Sprite::create("BasicShot.png");
	scene->addChild(sprLives, -2);
	sprLives->setTag(3);

	cocos2d::Label *livesLabel;
	livesLabel = Label::createWithTTF(to_string(sprLives->getTag()), "fonts/Marker Felt.ttf", 24);
	livesLabel->setColor(Color3B::WHITE);
	livesLabel->setPosition(Point(20, 752));
	scene->addChild(livesLabel, 10000);

	//SHOT COUNTER TIME
	auto sprTD = Sprite::create("BasicShot.png");
	scene->addChild(sprTD, -2);
	sprTD->setTag(1000);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///DIRECTION WIHT TOUCH
	EventListenerTouchOneByOne *listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch* touch, Event* event) {
		Point touchLoc = touch->getLocation();

		if (sprShip->getBoundingBox().containsPoint(touchLoc))
		{
			return true;
		}
		return true; // if you are consuming it
	};

	// trigger when moving touch
	listener->onTouchMoved = [=](Touch* touch, Event* event) {
		if (touch->getDelta().x < visibleSize.width - 10)
			sprShip->setPosition(sprShip->getPosition() + touch->getDelta());
	};

	// trigger when you let up
	listener->onTouchEnded = [=](Touch* touch, Event* event) {
		// your code
	};
	eventDispatcher->addEventListenerWithSceneGraphPriority(listener, scene);



	//Left
	auto buttonLeft = Button::create("Button_Normal.png", "Button_Press.png", "Button_Disable.png");
	buttonLeft->setOpacity(70);
	//button->setTitleText("Button Text");
	buttonLeft->setPosition(Vec2(visibleSize.width / 14, visibleSize.height / 6.68f));

	buttonLeft->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
		{
			auto sprShip_moveL = MoveBy::create(1, Vec2(-100, 0));
			sprShip->runAction(RepeatForever::create(sprShip_moveL));
		}
		break;
		case ui::Widget::TouchEventType::ENDED:
		{
			log("Button Left clicked");
			auto sprShip_moveL = MoveBy::create(1, Vec2(100, 0));
			sprShip->runAction(RepeatForever::create(sprShip_moveL));
		}
		break;
		case ui::Widget::TouchEventType::CANCELED:
		{
			auto sprShip_moveL = MoveBy::create(1, Vec2(100, 0));
			sprShip->runAction(RepeatForever::create(sprShip_moveL));
		}
		default:
			break;
		}
	});
	scene->addChild(buttonLeft);
	//Right
	auto buttonRight = Button::create("Button_Normal.png", "Button_Press.png", "Button_Disable.png");
	buttonRight->setOpacity(70);
	//button->setTitleText("Button Text");
	buttonRight->setPosition(Vec2(visibleSize.width / 5, visibleSize.height / 6.68f));

	buttonRight->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
		{
			auto sprShip_moveR = MoveBy::create(1, Vec2(100, 0));
			sprShip->runAction(RepeatForever::create(sprShip_moveR));
		}
		break;
		case ui::Widget::TouchEventType::ENDED:
		{
			log("Button Right clicked");
			auto sprShip_moveR = MoveBy::create(1, Vec2(-100, 0));
			sprShip->runAction(RepeatForever::create(sprShip_moveR));
		}
		break;
		case ui::Widget::TouchEventType::CANCELED:
		{
			auto sprShip_moveR = MoveBy::create(1, Vec2(-100, 0));
			sprShip->runAction(RepeatForever::create(sprShip_moveR));
		}
		default:
			break;
		}
	});
	scene->addChild(buttonRight);
	//UP
	auto buttonUp = Button::create("Button_Normal.png", "Button_Press.png", "Button_Disable.png");
	buttonUp->setOpacity(70);
	//button->setTitleText("Button Text");
	buttonUp->setPosition(Vec2(visibleSize.width / 7.3f, visibleSize.height / 5.f));

	buttonUp->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
		{
			auto sprShip_moveU = MoveBy::create(1, Vec2(0, 100));
			sprShip->runAction(RepeatForever::create(sprShip_moveU));
		}
		break;
		case ui::Widget::TouchEventType::ENDED:
		{
			log("Button Up clicked");
			auto sprShip_moveU = MoveBy::create(1, Vec2(0, -100));
			sprShip->runAction(RepeatForever::create(sprShip_moveU));
		}
		break;
		case ui::Widget::TouchEventType::CANCELED:
		{
			auto sprShip_moveU = MoveBy::create(1, Vec2(0, -100));
			sprShip->runAction(RepeatForever::create(sprShip_moveU));
		}
		default:
			break;
		}
	});
	scene->addChild(buttonUp);
	//DOWN
	auto buttonDown = Button::create("Button_Normal.png", "Button_Press.png", "Button_Disable.png");
	buttonDown->setOpacity(70);
	//button->setTitleText("Button Text");
	buttonDown->setPosition(Vec2(visibleSize.width / 7.3f, visibleSize.height / 10));

	buttonDown->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
		{
			auto sprShip_moveD = MoveBy::create(1, Vec2(0, -100));
			sprShip->runAction(RepeatForever::create(sprShip_moveD));
		}
		break;
		case ui::Widget::TouchEventType::ENDED:
		{
			log("Button Down clicked");
			auto sprShip_moveD = MoveBy::create(1, Vec2(0, 100));
			sprShip->runAction(RepeatForever::create(sprShip_moveD));
		}
		break;
		case ui::Widget::TouchEventType::CANCELED:
		{
			auto sprShip_moveD = MoveBy::create(1, Vec2(0, 100));
			sprShip->runAction(RepeatForever::create(sprShip_moveD));
		}
		default:
			break;
		}
	});
	scene->addChild(buttonDown);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////SHoot stuff/////////////////////////////////////////////////////////////////////////////
	auto buttonShoot = Button::create("Button_Normal.png", "Button_Press.png", "Button_Disable.png");

	//button->setTitleText("Button Text");
	buttonShoot->setPosition(Vec2(visibleSize.width / 1.25f, visibleSize.height / 6.68f));
	buttonShoot->setOpacity(70);
	buttonShoot->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
		{
			auto basicShot = Sprite::create("BasicShot.png");
			scene->addChild(basicShot, 0);
			basicShot->setTag(10);
			basicShot->setPosition(Vec2(sprShip->getPositionX(), sprShip->getPositionY() + 30));
			auto basicShot_velocity = MoveBy::create(1.8f, Vec2(0, 780));
			auto basicShot_acc = EaseIn::create(basicShot_velocity, 2);
			basicShot->runAction(basicShot_acc);
			//SHOT PHYSICS
			auto basicShot_physBody = PhysicsBody::createBox(basicShot->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
			basicShot_physBody->setGravityEnable(false);
			basicShot_physBody->setContactTestBitmask(0x01);
			basicShot->setPhysicsBody(basicShot_physBody);
			audio->playEffect("shotsound.mp3");
		}
		break;
		case ui::Widget::TouchEventType::ENDED:
			log("Button Shoot clicked");
			break;
		default:
			break;
		}
	});
	scene->addChild(buttonShoot);

	//SPACE FOR SHOOT
	auto spaceListener = EventListenerKeyboard::create();
	spaceListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {

		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_SPACE:
		{
			auto basicShot = Sprite::create("BasicShot.png");
			scene->addChild(basicShot, 0);
			basicShot->setTag(10);
			basicShot->setPosition(Vec2(sprShip->getPositionX(), sprShip->getPositionY() + 30));
			auto basicShot_velocity = MoveBy::create(1.8f, Vec2(0, 780));
			auto basicShot_acc = EaseIn::create(basicShot_velocity, 2);
			basicShot->runAction(basicShot_acc);
			//SHOT PHYSICS
			auto basicShot_physBody = PhysicsBody::createBox(basicShot->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
			basicShot_physBody->setGravityEnable(false);
			basicShot_physBody->setContactTestBitmask(0x01);
			basicShot->setPhysicsBody(basicShot_physBody);
			audio->playEffect("shotsound.mp3");
		}
		break;
		}
	};
	eventDispatcher->addEventListenerWithSceneGraphPriority(spaceListener, scene);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////PHYSICS LISTENER//////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//TAGS
	//SHOT = 10
	//ME = 20
	//EnemyUnkillable = 30
	//EnemyNormal = 31
	//EnemyWide = 35 - 39
	//EnemyShot = 40
	//Boundary = 50 (top/bot) 51 (left/right)
	//Extra Life = 70
	//Missle = 60
	//BOSS = 80 - 280
	//Score = 200+
	//Lives <10 starts at 3
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = [=](PhysicsContact& contact) {
		//log("contact happened");
		bool aliveCheck = true;
		auto nodeA = contact.getShapeA()->getBody()->getNode();
		auto nodeB = contact.getShapeB()->getBody()->getNode();
		scoreLabel->setString(to_string(50 * (sprScore->getTag() - 300)));
		livesLabel->setString(to_string((sprLives->getTag())));
		//contact.getContactData()->points[0];


		if (nodeA == nullptr || nodeB == nullptr) //this somehow fixes chrashes idk why
			return false;
		if (nodeA != nullptr) nodeA->setRotation(0);
		if (nodeB != nullptr) nodeB->setRotation(0);

		//////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////
		//COllisons with player
		if (sprLives->getTag() == 0) // game over
		{
			audio->stopBackgroundMusic();
			Director::getInstance()->replaceScene(GameOver(sprScore->getTag()));

			return false;
		}
		if (nodeA->getTag() == 20 && nodeB->getTag() == 30 || nodeB->getTag() == 20 && nodeA->getTag() == 30)//me and enemyunkillable
		{
			sprLives->setTag(sprLives->getTag() - 1);
			//BLOW UP SMALL
			audio->playEffect("explosion.mp3");
			if (nodeA->getTag() == 20) { auto sprSmallBlowUp = SmallBlowUp(nodeA->getPositionX(), nodeA->getPositionY() + 28); scene->addChild(sprSmallBlowUp); }
			else { auto sprSmallBlowUp = SmallBlowUp(nodeB->getPositionX(), nodeB->getPositionY() + 28); scene->addChild(sprSmallBlowUp); }
			return false;
		}
		if (nodeA->getTag() == 20 && nodeB->getTag() == 31 || nodeB->getTag() == 20 && nodeA->getTag() == 31)//me and enemynormal
		{
			sprLives->setTag(sprLives->getTag() - 1);
			//BLOW UP SMALL
			audio->playEffect("explosion.mp3");
			if (nodeA->getTag() == 20) { auto sprSmallBlowUp = SmallBlowUp(nodeA->getPositionX(), nodeA->getPositionY() + 28); scene->addChild(sprSmallBlowUp); }
			else { auto sprSmallBlowUp = SmallBlowUp(nodeB->getPositionX(), nodeB->getPositionY() + 28); scene->addChild(sprSmallBlowUp); }
			return false;
		}
		if ((nodeA->getTag() > 34 && nodeA->getTag() < 40) && (nodeB->getTag() == 20) || ((nodeB->getTag() > 34 && nodeB->getTag() < 40) && nodeA->getTag() == 20)) //WIDE BOI with collision
		{
			sprLives->setTag(sprLives->getTag() - 1);
			//BLOW UP SMALL
			audio->playEffect("explosion.mp3");
			if (nodeA->getTag() == 20) { auto sprSmallBlowUp = SmallBlowUp(nodeA->getPositionX(), nodeA->getPositionY() + 28); scene->addChild(sprSmallBlowUp); }
			else { auto sprSmallBlowUp = SmallBlowUp(nodeB->getPositionX(), nodeB->getPositionY() + 28); scene->addChild(sprSmallBlowUp); }
			return false;
		}
		if (nodeA->getTag() == 20 && nodeB->getTag() == 40 || nodeB->getTag() == 20 && nodeA->getTag() == 40)//me and enemyshot
		{
			sprLives->setTag(sprLives->getTag() - 1);
			//BLOW UP SMALL
			audio->playEffect("explosion.mp3");
			if (nodeA->getTag() == 20) { auto sprSmallBlowUp = SmallBlowUp(nodeA->getPositionX(), nodeA->getPositionY() + 28); scene->addChild(sprSmallBlowUp); }
			else { auto sprSmallBlowUp = SmallBlowUp(nodeB->getPositionX(), nodeB->getPositionY() + 28); scene->addChild(sprSmallBlowUp); }
			if (nodeA->getTag() == 20) nodeB->removeFromParentAndCleanup(false); else nodeA->removeFromParentAndCleanup(false);
			return false;
		}
		if (nodeA->getTag() == 20 && nodeB->getTag() == 60 || nodeB->getTag() == 20 && nodeA->getTag() == 60)//me and missle
		{
			sprLives->setTag(sprLives->getTag() - 1);
			//BLOW UP
			audio->playEffect("explosion.mp3");
			auto sprBlowUp = BlowUp(nodeA->getPositionX(), nodeA->getPositionY());
			scene->addChild(sprBlowUp);
			if (nodeA->getTag() == 20) nodeB->removeFromParentAndCleanup(false); else nodeA->removeFromParentAndCleanup(false);
			return false;
		}

		if (nodeA->getTag() == 20 && nodeB->getTag() == 60 || nodeB->getTag() == 20 && nodeA->getTag() == 60)//me and enemynormal
		{
			sprLives->setTag(sprLives->getTag() - 1);
			//BLOW UP SMALL
			audio->playEffect("explosion.mp3");
			if (nodeA->getTag() == 20) { auto sprSmallBlowUp = SmallBlowUp(nodeA->getPositionX(), nodeA->getPositionY() + 28); scene->addChild(sprSmallBlowUp); }
			else { auto sprSmallBlowUp = SmallBlowUp(nodeB->getPositionX(), nodeB->getPositionY() + 28); scene->addChild(sprSmallBlowUp); }
			return false;
		}

		if (nodeA->getTag() == 60 && nodeB->getTag() == 60)
			return false;//missle with themselves

		else if ((nodeA->getTag() > 79 && nodeA->getTag() < 200) && (nodeB->getTag() == 30) || ((nodeB->getTag() > 79 && nodeB->getTag() < 200) && nodeA->getTag() == 30)) //WIDE BOI with collision
			return false;
		//////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////
		else if (nodeA->getTag() == 20 && nodeB->getTag() == 70 || nodeB->getTag() == 20 && nodeA->getTag() == 70)
		{
			sprLives->setTag(sprLives->getTag() + 1);
			audio->playEffect("1up.wav");
			if (nodeA->getTag() == 20) nodeB->removeFromParentAndCleanup(false); else nodeA->removeFromParentAndCleanup(false);
			return false;
		}

		else if (nodeA->getTag() == 70 && nodeB->getTag() != 20 || nodeB->getTag() == 70 && nodeA->getTag() != 20) //ONE UP AND EVERYTHING ELSE
			return false;

		else if (nodeA->getTag() == 31 && nodeB->getTag() == 31) //NORMAL ENEMY AND OTHER OF THE SAME
			return false;

		else if ((nodeA->getTag() > 34 && nodeA->getTag() < 40) && (nodeB->getTag() == 30) || ((nodeB->getTag() > 34 && nodeB->getTag() < 40) && nodeA->getTag() == 30)) //WIDE BOI with unkillable
			return false;
		else if ((nodeA->getTag() > 34 && nodeA->getTag() < 40) && (nodeB->getTag() == 31) || ((nodeB->getTag() > 34 && nodeB->getTag() < 40) && nodeA->getTag() == 31)) //WIDE BOI with killable
			return false;

		else if ((nodeA->getTag() > 79 && nodeA->getTag() < 279) && (nodeB->getTag() == 40) || ((nodeB->getTag() > 79 && nodeB->getTag() < 279) && nodeA->getTag() == 40)) //boss with shots
			return false;


		//else if (nodeA->getTag() == 20 && nodeB->getTag() == 50 || nodeB->getTag() == 20 && nodeA->getTag() == 50)//me and the boundary at top and bottom
		//{
		//	if (sprShip->getPositionY() > 600)
		//		sprShip->setPosition(Vec2(sprShip->getPositionX(), sprShip->getPositionY() - 15));
		//	if (sprShip->getPositionY() < 100)
		//		sprShip->setPosition(Vec2(sprShip->getPositionX(), sprShip->getPositionY() + 15));
		//	return false;
		//}
		//else if (nodeA->getTag() == 20 && nodeB->getTag() == 51 || nodeB->getTag() == 20 && nodeA->getTag() == 51)//me and the boundary at right and left
		//{
		//	if (sprShip->getPositionY() > 400)
		//		sprShip->setPosition(Vec2(sprShip->getPositionX() - 15, sprShip->getPositionY()));
		//	if (sprShip->getPositionY() < 50)
		//		sprShip->setPosition(Vec2(sprShip->getPositionX() + 15, sprShip->getPositionY()));
		//	return false;
		//}
		else if (nodeA->getTag() == 20 && nodeB->getTag() == 10 || nodeB->getTag() == 20 && nodeA->getTag() == 10)//My Ship and My shots
			return false;

		else if ((nodeA->getTag() == 40 && nodeB->getTag() == 10) && (nodeB->getTag() == 40 && nodeA->getTag() == 10))//my shots with enemy shots
		{
			/*nodeA->removeFromParentAndCleanup(false);
			nodeB->removeFromParentAndCleanup(false);*/
			return false;
		}

		else if (nodeA->getTag() == 40 && nodeB->getTag() == 30) //enemy shot to enemy ship
			return false;

		else if (nodeA->getTag() == 10 && (nodeB->getTag() == 50 || nodeB->getTag() == 51)) //if my shots hit the back
			nodeA->removeFromParentAndCleanup(true);
		else if (nodeA->getTag() == 40 && (nodeB->getTag() == 50 || nodeB->getTag() == 51)) //if their shots hit the back
			nodeA->removeFromParentAndCleanup(true);

		else if ((nodeA->getTag() == 40 && nodeB->getTag() != 40) && (nodeA->getTag() == 40 && nodeB->getTag() != 30)) //enemy shot collision
		{
			if ((nodeA->getTag() == 40))
				nodeA->removeFromParentAndCleanup(true);
			else if ((nodeB->getTag() == 40))
				nodeB->removeFromParentAndCleanup(true);
			return true;
		}

		else if ((nodeA->getTag() == 30 && nodeB->getTag() == 30) || (nodeA->getTag() == 30 && (nodeB->getTag() == 50 || nodeB->getTag() == 51)) || (nodeA->getTag() == 30 && nodeB->getTag() == 31) || (nodeA->getTag() == 31 && (nodeB->getTag() == 50 || nodeB->getTag() == 51))) //Enemy with themself and wall
		{
			return false;
		}

		else if ((nodeA->getTag() == 60 && (nodeB->getTag() == 50 || nodeB->getTag() == 51)) || ((nodeA->getTag() == 50 || nodeA->getTag() == 51) && nodeB->getTag() == 60) || (nodeA->getTag() == 30 && nodeB->getTag() == 60) || (nodeA->getTag() == 60 && nodeB->getTag() == 30) || (nodeA->getTag() == 31 && nodeB->getTag() == 60) || (nodeA->getTag() == 60 && nodeB->getTag() == 31)) //Missle with themself, others and wall
		{
			return false;
		}

		else if ((nodeA->getTag() == 60 && nodeB->getTag() == 40) || (nodeA->getTag() == 40 && nodeB->getTag() == 60)) //Missle withh enemy shots
		{
			return false;
		}

		else if (nodeA->getTag() == 20 && nodeB->getTag() == 40 || nodeA->getTag() == 40 && nodeB->getTag() == 20)
		{
			/*nodeB->removeFromParentAndCleanup(true);
			nodeA->removeFromParentAndCleanup(true);*/
		}
		else if ((nodeA->getTag() == 30 && nodeB->getTag() == 10) || (nodeA->getTag() == 10 && nodeB->getTag() == 30)) //unkillable enemy
		{
			//Explosion
			audio->playEffect("explosion.mp3");
			auto sprBlowUp = BlowUp(nodeA->getPositionX(), nodeA->getPositionY());
			scene->addChild(sprBlowUp);
			//score
			sprScore->setTag(sprScore->getTag() + 1);

			nodeA->setPosition(visibleSize.width / 2, 1500);
			nodeB->setPosition(visibleSize.width / 2, 1500);
			if (nodeA->getTag() == 10)
			{
				nodeA->removeFromParentAndCleanup(true);
				if (sprScore->getTag() > 330)
					nodeB->setTag(31);
			}
			else
			{
				nodeB->removeFromParentAndCleanup(true);
				if (sprScore->getTag() > 330)
					nodeA->setTag(31);
			}
			return false;

		}
		else if ((nodeA->getTag() == 31 && nodeB->getTag() == 10) || (nodeA->getTag() == 10 && nodeB->getTag() == 31)) //killable enemy
		{
			//Explosion
			audio->playEffect("explosion.mp3");
			auto sprBlowUp = BlowUp(nodeA->getPositionX(), nodeA->getPositionY());
			scene->addChild(sprBlowUp);
			//score
			sprScore->setTag(sprScore->getTag() + 1);

			nodeA->setPosition(visibleSize.width / 2, 1500);
			nodeB->setPosition(visibleSize.width / 2, 1500);
			nodeA->CCNode::stopAllActions();
			nodeB->CCNode::stopAllActions();
			if (nodeA->getTag() == 10)
				nodeA->removeFromParentAndCleanup(true);
			else
				nodeB->removeFromParentAndCleanup(true);
			return false;
		}
		else if ((nodeA->getTag() > 34 && nodeA->getTag() < 40) && (nodeB->getTag() != 10) || ((nodeB->getTag() > 34 && nodeB->getTag() < 40) && nodeA->getTag() == 31)) //WIDE BOI with collision
			return false;
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if (((nodeA->getTag() > 34 && nodeA->getTag() < 39) && nodeB->getTag() == 10) || ((nodeB->getTag() > 34 && nodeB->getTag() < 39) && nodeA->getTag() == 10)) //WIDE BOI state 1
		{
			audio->playEffect("explosion.mp3");
			if (nodeA->getTag() > 34 && nodeA->getTag() < 39) { auto sprSmallBlowUp = SmallBlowUpWide(nodeA->getPositionX(), nodeA->getPositionY() - 43); scene->addChild(sprSmallBlowUp); }
			else { auto sprSmallBlowUp = SmallBlowUpWide(nodeB->getPositionX(), nodeB->getPositionY() - 43); scene->addChild(sprSmallBlowUp); }
			if (nodeA->getTag() != 10) { nodeB->removeFromParentAndCleanup(true); nodeA->setTag(nodeA->getTag() + 1); }
			else if (nodeB->getTag() != 10) { nodeA->removeFromParentAndCleanup(true); nodeB->setTag(nodeB->getTag() + 1); }
			return false;
		}
		else if ((nodeA->getTag() == 39 && nodeB->getTag() == 10) || (nodeA->getTag() == 10 && nodeB->getTag() == 39)) //WIDE BOI state 5
		{
			//Explosion
			audio->playEffect("explosion.mp3");
			auto sprBlowUp = BlowUp(nodeA->getPositionX(), nodeA->getPositionY());
			scene->addChild(sprBlowUp);
			//score
			sprScore->setTag(sprScore->getTag() + 5);

			nodeA->setPosition(visibleSize.width / 2, 1500);
			nodeB->setPosition(visibleSize.width / 2, 1500);
			nodeA->CCNode::stopAllActions();
			nodeB->CCNode::stopAllActions();
			if (nodeA->getTag() == 10)
				nodeA->removeFromParentAndCleanup(true);
			else
				nodeB->removeFromParentAndCleanup(true);
			return false;
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		else if ((nodeA->getTag() == 60 && nodeB->getTag() == 10) || (nodeA->getTag() == 10 && nodeB->getTag() == 60)) //MISSLE
		{
			audio->playEffect("explosion.mp3");
			auto sprBlowUp = BlowUp(nodeA->getPositionX(), nodeA->getPositionY());
			scene->addChild(sprBlowUp);
			//score
			sprScore->setTag(sprScore->getTag() + 1);
			nodeA->removeFromParentAndCleanup(true);
			nodeB->removeFromParentAndCleanup(true);
			return false;
		}



		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if (((nodeA->getTag() > 79 && nodeA->getTag() < 200) && nodeB->getTag() == 10) || ((nodeB->getTag() > 79 && nodeB->getTag() < 200) && nodeA->getTag() == 10)) //BOSS state 1
		{
			//////////////////////////////////////////
			////////////////////////////////////////// BOSS SHOT STAGES

			if (nodeA->getTag() == 81 || nodeB->getTag() == 81) //X ATTACK
			{
				for (int j = 0; j < 5; j++)
				{
					log("YES IT WORKS");
					for (int i = 1; i < 8; i++) //left -> right
					{
						Sprite* bossShot1 = BossShotPattern1(i, visibleSize.width, visibleSize.height, j);
						scene->addChild(bossShot1);
					}
					for (int i = 15; i > 8; i--) //right -> left
					{
						Sprite* bossShot1 = BossShotPattern1(i, visibleSize.width, visibleSize.height, j);
						scene->addChild(bossShot1);
					}
				}
			}

			if (nodeA->getTag() == 100 || nodeB->getTag() == 100) //X ATTACK
			{
				for (int j = 0; j < 5; j++)
				{
					for (int i = 1; i < 8; i++) //left -> right
					{
						Sprite* bossShot1 = BossShotPattern1(i, visibleSize.width, visibleSize.height, j);
						scene->addChild(bossShot1);
					}
					for (int i = 15; i > 8; i--) //right -> left
					{
						Sprite* bossShot1 = BossShotPattern1(i, visibleSize.width, visibleSize.height, j);
						scene->addChild(bossShot1);
					}
				}
			}
			if (nodeA->getTag() == 120 || nodeB->getTag() == 120) //CHARGE ATTACK
			{
				Sprite* bossCharge = BossCharge();
				scene->addChild(bossCharge);
				auto bossLaser = Sprite::create("EnemyShotW.png");
				bossLaser->setTag(40);
				bossLaser->setRotation(90);
				bossLaser->setColor(Color3B(0, 0, 255));
				bossLaser->setPosition(Vec2(216, 400));
				bossLaser->setScale(10);
				bossLaser->setScaleY(5);
				bossLaser->setOpacity(0);
				scene->addChild(bossLaser);

				auto move = MoveBy::create(2, Vec2(0, -800));
				auto delay = DelayTime::create(1.8f);
				auto fade = FadeIn::create(0.2f);
				auto sequence = Sequence::create(delay, fade, move, nullptr);
				bossLaser->runAction(sequence);
			}

			if (nodeA->getTag() == 130 || nodeB->getTag() == 130) //X ATTACK
			{
				for (int j = 0; j < 5; j++)
				{
					log("YES IT WORKS");
					for (int i = 1; i < 8; i++) //left -> right
					{
						Sprite* bossShot1 = BossShotPattern1(i, visibleSize.width, visibleSize.height, j);
						scene->addChild(bossShot1);
					}
					for (int i = 15; i > 8; i--) //right -> left
					{
						Sprite* bossShot1 = BossShotPattern1(i, visibleSize.width, visibleSize.height, j);
						scene->addChild(bossShot1);
					}
				}
			}

			if (nodeA->getTag() == 170 || nodeB->getTag() == 170) //CHARGE ATTACK
			{
				Sprite* bossCharge = BossCharge();
				scene->addChild(bossCharge);
				auto bossLaser = Sprite::create("EnemyShotW.png");
				bossLaser->setTag(40);
				bossLaser->setRotation(90);
				bossLaser->setColor(Color3B(0, 0, 255));
				bossLaser->setPosition(Vec2(216, 400));
				bossLaser->setScale(10);
				bossLaser->setScaleY(5);
				bossLaser->setOpacity(0);
				scene->addChild(bossLaser);

				auto move = MoveBy::create(2, Vec2(0, -800));
				auto delay = DelayTime::create(1.8f);
				auto fade = FadeIn::create(0.2f);
				auto sequence = Sequence::create(delay, fade, move, nullptr);
				bossLaser->runAction(sequence);
			}

			if (nodeA->getTag() == 170 || nodeB->getTag() == 170) //X ATTACK
			{
				for (int j = 0; j < 5; j++)
				{
					log("YES IT WORKS");
					for (int i = 1; i < 8; i++) //left -> right
					{
						Sprite* bossShot1 = BossShotPattern1(i, visibleSize.width, visibleSize.height, j);
						scene->addChild(bossShot1);
					}
					for (int i = 15; i > 8; i--) //right -> left
					{
						Sprite* bossShot1 = BossShotPattern1(i, visibleSize.width, visibleSize.height, j);
						scene->addChild(bossShot1);
					}
				}
			}


			////////////////////////////////////////////
			sprScore->setTag(sprScore->getTag() + 1);
			audio->playEffect("explosion.mp3");
			auto sprSmallBlowUp = SmallBlowUpBoss(); scene->addChild(sprSmallBlowUp);
			if (nodeA->getTag() != 10) { nodeB->removeFromParentAndCleanup(true); nodeA->setTag(nodeA->getTag() + 1); }
			else if (nodeB->getTag() != 10) { nodeA->removeFromParentAndCleanup(true); nodeB->setTag(nodeB->getTag() + 1); }

			return false;
		}
		else if ((nodeA->getTag() == 200 && nodeB->getTag() == 10) || (nodeA->getTag() == 10 && nodeB->getTag() == 200)) //BOSS final state
		{
			//Explosion
			audio->playEffect("explosion.mp3");
			auto sprBlowUp = BlowUp(nodeA->getPositionX(), nodeA->getPositionY());
			scene->addChild(sprBlowUp);
			//score
			sprScore->setTag(sprScore->getTag() + 25);


			nodeA->setPosition(visibleSize.width / 2, 1500);
			nodeB->setPosition(visibleSize.width / 2, 1500);
			nodeA->CCNode::stopAllActions();
			nodeB->CCNode::stopAllActions();
			if (nodeA->getTag() == 10)
				nodeA->removeFromParentAndCleanup(true);
			else
				nodeB->removeFromParentAndCleanup(true);

			//WIN::
			audio->stopBackgroundMusic();
			Director::getInstance()->replaceScene(GameWin(sprScore->getTag()));
			return false;
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





		else if (nodeA && nodeB)
		{
			if ((nodeA->getTag() == 10 && nodeB->getTag() != 10) && (nodeA->getTag() == 10 && (nodeB->getTag() != 50 || nodeB->getTag() != 51)))//if node a is the sprite and b is the boundary, while not hitting back line
			{
				nodeB->removeFromParentAndCleanup(true);
				nodeA->removeFromParentAndCleanup(true);
			}
		}

		return true;
	};
	eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, scene);
	scene->removeChild(basicShot, 0);


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////                      ////////////////////////////////////////////////////
	//////////////////////////////////////////////////        LEVEL 1       ////////////////////////////////////////////////////
	//////////////////////////////////////////////////                      ////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	srand((unsigned int)time(nullptr));
	//Size selfContentSize = scene->getContentSize();

	//UNKILLABLES (TAG, TIME)
	Sprite* sprEnemy = EnemyShip(30, 6);
	sprEnemy->setColor(Color3B(255, 50, 100));
	scene->addChild(sprEnemy);
	Sprite* sprEnemy2 = EnemyShip(30, 6);
	sprEnemy2->setColor(Color3B(255, 50, 100));
	scene->addChild(sprEnemy2);

	//KILLABLES (TAG, TIME)
	Sprite* sprEnemyK = EnemyShip(31, 2.f);
	scene->addChild(sprEnemyK);
	Sprite* sprEnemyK1 = EnemyShip(31, 2.f);
	scene->addChild(sprEnemyK1);

	Sprite* sprEnemyK2 = EnemyShip(31, 21.f);
	scene->addChild(sprEnemyK2);
	Sprite* sprEnemyK3 = EnemyShip(31, 22.f);
	scene->addChild(sprEnemyK3);
	Sprite* sprEnemyK4 = EnemyShip(31, 23.f);
	scene->addChild(sprEnemyK4);
	Sprite* sprEnemyK5 = EnemyShip(31, 24.f);
	scene->addChild(sprEnemyK5);
	Sprite* sprEnemyK6 = EnemyShip(31, 25.f);
	scene->addChild(sprEnemyK6);

	//MISSLES (xPOS, yPOS, TIME, SIDE l/r)
	Sprite* sprMissle = Missle(visibleSize.width / 2 - 50, 550, 8, 'r');
	scene->addChild(sprMissle);
	Sprite* sprMissle2 = Missle(visibleSize.width / 2 + 50, 550, 8, 'l');
	scene->addChild(sprMissle2);
	Sprite* sprMissle3 = Missle(visibleSize.width / 2 - 25, 550, 8, 'r');
	scene->addChild(sprMissle3);
	Sprite* sprMissle4 = Missle(visibleSize.width / 2 + 25, 550, 8, 'l');
	scene->addChild(sprMissle4);

	Sprite* sprMissle5 = Missle(visibleSize.width / 2 - 25, 550, 20, 'r');
	scene->addChild(sprMissle5);
	Sprite* sprMissle6 = Missle(visibleSize.width / 2 + 25, 550, 20, 'l');
	scene->addChild(sprMissle6);

	Sprite* sprMissle7 = Missle(visibleSize.width / 2 - 50, 650, 32, 'r');
	scene->addChild(sprMissle7);
	Sprite* sprMissle8 = Missle(visibleSize.width / 2 + 50, 650, 32, 'l');
	scene->addChild(sprMissle8);
	Sprite* sprMissle9 = Missle(visibleSize.width / 2 - 25, 650, 32, 'r');
	scene->addChild(sprMissle9);
	Sprite* sprMissle10 = Missle(visibleSize.width / 2 + 25, 650, 32, 'l');
	scene->addChild(sprMissle10);

	Sprite* sprMissle11 = Missle(visibleSize.width / 2 - 75, 650, 35, 'r');
	scene->addChild(sprMissle11);
	Sprite* sprMissle12 = Missle(visibleSize.width / 2 + 75, 650, 35, 'l');
	scene->addChild(sprMissle12);

	//WIDE ENEMY (TAG, TIME, xCOORD, yCOORD)
	Sprite* sprEnemyWide = EnemyWide(35, 12, 200, 500);
	scene->addChild(sprEnemyWide);

	Sprite* sprEnemyWide2 = EnemyWide(35, 15, 100, 600);
	scene->addChild(sprEnemyWide2);
	Sprite* sprEnemyWide3 = EnemyWide(35, 15, 668, 600);
	scene->addChild(sprEnemyWide3);

	Sprite* sprEnemyWide4 = EnemyWide(35, 28, 668, 600);
	scene->addChild(sprEnemyWide4);
	Sprite* sprEnemyWide5 = EnemyWide(35, 28, 100, 600);
	scene->addChild(sprEnemyWide5);
	Sprite* sprEnemyWide6 = EnemyWide(35, 28, 216, 500);
	scene->addChild(sprEnemyWide6);

	Sprite* sprEnemyWide7 = EnemyWide(35, 38, 166, 500);
	scene->addChild(sprEnemyWide7);
	Sprite* sprEnemyWide8 = EnemyWide(35, 38, 266, 500);
	scene->addChild(sprEnemyWide8);


	//ONE UP (TIME)
	Sprite* sprOneUp = OneUp(23);
	scene->addChild(sprOneUp);

	//BOSS (TIME, WIDTH OF SCREEN) //starts at 45s
	Sprite* sprBoss = Boss(47, visibleSize.width); //47
	scene->addChild(sprBoss);

	//RUNTIME
	Director::getInstance()->getScheduler()->schedule([=](float dt) {
		{
			////SHOOTING
			sprTD->setTag(sprTD->getTag() + 1);
			//UNKILLABLE
			Sprite* basicShotE = EnemyShot1(sprEnemy);
			scene->addChild(basicShotE);
			sprEnemy->setRotation(0);
			Sprite* basicShotE2 = EnemyShot1(sprEnemy2);
			scene->addChild(basicShotE2);
			sprEnemy2->setRotation(0);

			//KILLABLE
			Sprite* basicShotEK = EnemyShot1(sprEnemyK);
			scene->addChild(basicShotEK);
			sprEnemyK->setRotation(0);
			Sprite* basicShotEK1 = EnemyShot1(sprEnemyK1);
			scene->addChild(basicShotEK1);
			sprEnemyK1->setRotation(0);
			Sprite* basicShotEK2 = EnemyShot1(sprEnemyK2);
			scene->addChild(basicShotEK2);
			sprEnemyK2->setRotation(0);
			Sprite* basicShotEK3 = EnemyShot1(sprEnemyK3);
			scene->addChild(basicShotEK3);
			sprEnemyK3->setRotation(0);
			Sprite* basicShotEK4 = EnemyShot1(sprEnemyK4);
			scene->addChild(basicShotEK4);
			sprEnemyK4->setRotation(0);
			Sprite* basicShotEK5 = EnemyShot1(sprEnemyK5);
			scene->addChild(basicShotEK5);
			sprEnemyK5->setRotation(0);
			Sprite* basicShotEK6 = EnemyShot1(sprEnemyK6);
			scene->addChild(basicShotEK6);
			sprEnemyK6->setRotation(0);

			//WIDE ENEMY
			Sprite* basicShotW = EnemyShotWide(sprEnemyWide);
			scene->addChild(basicShotW);
			sprEnemyWide->setRotation(0);
			Sprite* basicShotW2 = EnemyShotWide(sprEnemyWide2);
			scene->addChild(basicShotW2);
			sprEnemyWide2->setRotation(0);
			Sprite* basicShotW3 = EnemyShotWide(sprEnemyWide3);
			scene->addChild(basicShotW3);
			sprEnemyWide3->setRotation(0);
			Sprite* basicShotW4 = EnemyShotWide(sprEnemyWide4);
			scene->addChild(basicShotW4);
			sprEnemyWide4->setRotation(0);
			Sprite* basicShotW5 = EnemyShotWide(sprEnemyWide5);
			scene->addChild(basicShotW5);
			sprEnemyWide5->setRotation(0);
			Sprite* basicShotW6 = EnemyShotWide(sprEnemyWide6);
			scene->addChild(basicShotW6);
			sprEnemyWide6->setRotation(0);
			Sprite* basicShotW7 = EnemyShotWide(sprEnemyWide7);
			scene->addChild(basicShotW7);
			sprEnemyWide6->setRotation(0);
			Sprite* basicShotW8 = EnemyShotWide(sprEnemyWide8);
			scene->addChild(basicShotW8);
			sprEnemyWide8->setRotation(0);


			//BOSS IDLE
			if (sprTD->getTag() - 1000 > 48)
			{
				Sprite* sprMissleR = BossIdleR();
				scene->addChild(sprMissleR);
				Sprite* sprMissleL = BossIdleL();
				scene->addChild(sprMissleL);
				Sprite* sprMissleM = BossIdleM();
				scene->addChild(sprMissleM);
			}

		}
	}, scene, 1.f, false, "schedulerKey");


	auto menu = Menu::create(GetCloseItem(), NULL);
	scene->addChild(menu);
	return scene;


}

void HelloWorld::addEnemy(float dt)
{
	this->unschedule(schedule_selector(HelloWorld::addEnemy));
	auto sprEnemy = Sprite::create("EnemyShip.png");
	sprEnemy->setTag(30);

	auto sprEnemyContentSize = sprEnemy->getContentSize();
	auto selfContentSize = this->getContentSize();
	int minY = 500;
	int maxY = 700;
	int minX = 50;
	int maxX = 380;
	int rangeX = maxX - minX;
	int rangeY = maxY - minY;
	int randomY = (rand() % rangeY) + minY;
	int randomX = (rand() % rangeX) + minX;

	sprEnemy->setPosition(Vec2(randomX, randomY));
	//sprEnemy->setPosition(Vec2(selfContentSize.width + sprEnemyContentSize.width / 2, randomY));
	this->addChild(sprEnemy);

	// 2
	int minDuration = 2.0;
	int maxDuration = 4.0;
	int rangeDuration = maxDuration - minDuration;
	int randomDuration = (rand() % rangeDuration) + minDuration;

	// 3
	auto actionMove1 = MoveTo::create(randomDuration, Vec2(50, randomY));
	auto actionMove2 = MoveTo::create(randomDuration, Vec2(selfContentSize.width - 50, randomY));
	sprEnemy->runAction(RepeatForever::create(Sequence::create(actionMove1, actionMove2, nullptr)));

	//Enemy Physics
	auto sprEnemy_physBody = PhysicsBody::createBox(sprEnemy->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
	sprEnemy_physBody->setGravityEnable(false);
	sprEnemy_physBody->setContactTestBitmask(0x01);
	sprEnemy->setPhysicsBody(sprEnemy_physBody);

	auto xPos = sprEnemy->getPositionX();
	auto yPos = sprEnemy->getPositionY();
	log("%f     %f", xPos, yPos);





}


Sprite* EnemyShip(int enemyTag, float whenToSpawn)
{
	auto sprEnemy = Sprite::create("EnemyShip.png");
	sprEnemy->setTag(enemyTag);

	auto sprEnemyContentSize = sprEnemy->getContentSize();
	int minY = 500;
	int maxY = 700;
	int minX = 50;
	int maxX = 380;
	int rangeX = maxX - minX;
	int rangeY = maxY - minY;
	int randomY = (rand() % rangeY) + minY;
	int randomX = (rand() % rangeX) + minX;
	int randomInt = (rand() % 2) + 1;
	if (randomInt == 1)
		sprEnemy->setPosition(Vec2(500, 700));
	if (randomInt == 2)
		sprEnemy->setPosition(Vec2(-500, 700));

	//sprEnemy->setPosition(Vec2(selfContentSize.width + sprEnemyContentSize.width / 2, randomY));
	// 2
	int minDuration = 2.0;
	int maxDuration = 4.0;
	int rangeDuration = maxDuration - minDuration;
	int randomDuration = (rand() % rangeDuration) + minDuration;

	// 3
	auto actionWait = DelayTime::create(whenToSpawn);
	auto actionMove1 = MoveTo::create(randomDuration, Vec2(50, randomY));
	randomY = (rand() % rangeY) + minY;
	auto actionMove2 = MoveTo::create(randomDuration, Vec2(400, randomY));
	//sprEnemy->runAction(RepeatForever::create(Sequence::create(actionMove1, actionMove2, nullptr)));
	auto repeater = Sequence::create(actionMove1, actionMove2, nullptr);
	auto sequence = Sequence::create(actionWait, Repeat::create(repeater, 99999), nullptr);
	sprEnemy->runAction(sequence);

	//Enemy Physics
	auto sprEnemy_physBody = PhysicsBody::createBox(sprEnemy->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
	sprEnemy_physBody->setGravityEnable(false);
	sprEnemy_physBody->setContactTestBitmask(0x01);
	sprEnemy->setPhysicsBody(sprEnemy_physBody);

	auto xPos = sprEnemy->getPositionX();
	auto yPos = sprEnemy->getPositionY();
	log("%f     %f", xPos, yPos);

	return sprEnemy;
}

Sprite* EnemyWide(int enemyTag, float whenToSpawn, int moveX, int moveY)
{
	auto sprEnemyWide = Sprite::create("EnemyShip.png");
	sprEnemyWide->setTag(enemyTag);
	sprEnemyWide->setScale(2);
	auto sprEnemyWideContentSize = sprEnemyWide->getContentSize();
	int minY = 500;
	int maxY = 700;
	int minX = 50;
	int maxX = 380;
	int rangeX = maxX - minX;
	int rangeY = maxY - minY;
	int randomY = (rand() % rangeY) + minY;
	int randomX = (rand() % rangeX) + minX;
	int randomInt = (rand() % 2) + 1;
	if (randomInt == 1)
		sprEnemyWide->setPosition(Vec2(500, 700));
	if (randomInt == 2)
		sprEnemyWide->setPosition(Vec2(-500, 700));

	// 3
	auto actionWait = DelayTime::create(whenToSpawn);
	auto actionMove1 = MoveTo::create(0.5, Vec2(moveX, moveY));
	auto repeater = Sequence::create(actionMove1, nullptr);
	auto sequence = Sequence::create(actionWait, Repeat::create(repeater, 99999), nullptr);
	sprEnemyWide->runAction(sequence);

	//Enemy Physics
	auto sprEnemyWide_physBody = PhysicsBody::createBox(sprEnemyWide->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
	sprEnemyWide_physBody->setGravityEnable(false);
	sprEnemyWide_physBody->setContactTestBitmask(0x01);
	sprEnemyWide->setPhysicsBody(sprEnemyWide_physBody);

	auto xPos = sprEnemyWide->getPositionX();
	auto yPos = sprEnemyWide->getPositionY();
	log("%f     %f", xPos, yPos);

	return sprEnemyWide;
}

Sprite* Missle(int xPos, int yPos, float time, char side)
{
	auto sprMissle = Sprite::create("missle.png");
	sprMissle->setTag(60);
	sprMissle->setScale(0.07f);
	sprMissle->setRotation(180);
	int randomInt = (rand() % 2) + 1;
	if (side == 'r')
		sprMissle->setPosition(Vec2(1000, 720));
	if (side == 'l')
		sprMissle->setPosition(Vec2(-1000, 720));

	auto moveIn = MoveTo::create(4, Vec2(xPos, yPos));
	auto shoot = MoveBy::create(2, Vec2(0, -900));
	auto delay = DelayTime::create(time);
	sprMissle->runAction(Sequence::create(delay, moveIn, shoot, nullptr));

	//Physics
	auto sprMissle_physBody = PhysicsBody::createBox(sprMissle->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
	sprMissle_physBody->setGravityEnable(false);
	sprMissle_physBody->setContactTestBitmask(0x01);
	sprMissle->setPhysicsBody(sprMissle_physBody);

	return sprMissle;

}

Sprite* EnemyShot1(Sprite* sprEnemy)
{
	auto xPos = sprEnemy->getPositionX();
	auto yPos = sprEnemy->getPositionY();
	auto basicShotE = Sprite::create("BasicShot.png");
	basicShotE->setTag(40);
	basicShotE->setRotation(180);
	basicShotE->setColor(Color3B(0, 255, 0));
	basicShotE->setPosition(Vec2(xPos, yPos - 34));

	int randInt = rand() % 10 + 1;
	randInt = randInt / 10;
	auto basicShotE_velocity = MoveBy::create(1.8f + randInt, Vec2(0, -780));
	auto basicShotE_acc = EaseIn::create(basicShotE_velocity, 2);
	basicShotE->runAction(basicShotE_acc);

	//SHOT PHYSICS
	auto basicShotE_physBody = PhysicsBody::createBox(basicShotE->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
	basicShotE_physBody->setGravityEnable(false);
	basicShotE_physBody->setContactTestBitmask(0x01);
	basicShotE->setPhysicsBody(basicShotE_physBody);

	return basicShotE;
}

Sprite* EnemyShotWide(Sprite* sprEnemyWide)
{
	auto xPos = sprEnemyWide->getPositionX();
	auto yPos = sprEnemyWide->getPositionY();

	int chanse = rand() % 4 + 1;

	auto basicShotW = Sprite::create("EnemyShotW.png");
	basicShotW->setTag(40);
	basicShotW->setRotation(90);
	basicShotW->setScaleX(4);
	basicShotW->setScaleY(1.25);
	basicShotW->setColor(Color3B(255, 100, 100));
	if (chanse != 1)
		basicShotW->setPosition(Vec2(xPos, yPos - 63));
	int randInt = rand() % 10 + 1;
	randInt = randInt / 10;
	auto basicShotW_velocity = MoveBy::create(1.8f + randInt, Vec2(0, -780));
	auto basicShotW_acc = EaseIn::create(basicShotW_velocity, 2);
	basicShotW->runAction(basicShotW_acc);

	//SHOT PHYSICS
	auto basicShotW_physBody = PhysicsBody::createBox(basicShotW->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
	basicShotW_physBody->setGravityEnable(false);
	basicShotW_physBody->setContactTestBitmask(0x01);
	basicShotW->setPhysicsBody(basicShotW_physBody);

	return basicShotW;
}

Sprite* BlowUp(int xPos, int yPos)
{
	//Explosion Animation
	auto sprBlowUp = Sprite::create("BlowUp_1.png");
	sprBlowUp->setPosition(Vec2(xPos, yPos));
	float width = sprBlowUp->getContentSize().width;
	float height = sprBlowUp->getContentSize().height;
	Vector<SpriteFrame*> blowUp_animFrames;
	blowUp_animFrames.reserve(4);
	blowUp_animFrames.pushBack(SpriteFrame::create("BlowUp_1.png", Rect(0, 0, 96, 96)));
	blowUp_animFrames.pushBack(SpriteFrame::create("BlowUp_2.png", Rect(0, 0, 96, 96)));
	blowUp_animFrames.pushBack(SpriteFrame::create("BlowUp_3.png", Rect(0, 0, 96, 96)));
	blowUp_animFrames.pushBack(SpriteFrame::create("BlowUp_4.png", Rect(0, 0, 96, 96)));
	blowUp_animFrames.pushBack(SpriteFrame::create("BlowUp_5.png", Rect(0, 0, 96, 96)));
	blowUp_animFrames.pushBack(SpriteFrame::create("BlowUp_6.png", Rect(0, 0, 96, 96)));
	blowUp_animFrames.pushBack(SpriteFrame::create("BlowUp_7.png", Rect(0, 0, 96, 96)));
	blowUp_animFrames.pushBack(SpriteFrame::create("BlowUp_8.png", Rect(0, 0, 96, 96)));
	blowUp_animFrames.pushBack(SpriteFrame::create("BlowUp_9.png", Rect(0, 0, 96, 96)));
	blowUp_animFrames.pushBack(SpriteFrame::create("BlowUp_10.png", Rect(0, 0, 96, 96)));
	blowUp_animFrames.pushBack(SpriteFrame::create("BlowUp_11.png", Rect(0, 0, 96, 96)));
	blowUp_animFrames.pushBack(SpriteFrame::create("BlowUp_12.png", Rect(0, 0, 96, 96)));
	blowUp_animFrames.pushBack(SpriteFrame::create("BlowUp_13.png", Rect(0, 0, 96, 96)));

	auto sprBlowUp_animation = Animation::createWithSpriteFrames(blowUp_animFrames, 0.055f);
	auto sprBlowUp_animate = Animate::create(sprBlowUp_animation);
	sprBlowUp->runAction(sprBlowUp_animate);

	return sprBlowUp;
}

Sprite* SmallBlowUp(int xPos, int yPos)
{
	//Explosion Animation
	auto sprSBlowUp = Sprite::create("SBlowUp_1.png");
	sprSBlowUp->setPosition(Vec2(xPos, yPos));
	float width = sprSBlowUp->getContentSize().width;
	float height = sprSBlowUp->getContentSize().height;
	Vector<SpriteFrame*> sblowUp_animFrames;
	sblowUp_animFrames.reserve(4);
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_1.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_2.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_3.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_4.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_5.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_6.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_7.png", Rect(0, 0, 18, 18)));

	auto sprSBlowUp_animation = Animation::createWithSpriteFrames(sblowUp_animFrames, 0.035f);
	auto sprSBlowUp_animate = Animate::create(sprSBlowUp_animation);
	sprSBlowUp->runAction(sprSBlowUp_animate);
	return sprSBlowUp;
}

Sprite* SmallBlowUpWide(int xPos, int yPos)
{
	//Explosion Animation
	auto sprSBlowUp = Sprite::create("SBlowUp_1.png");
	sprSBlowUp->setPosition(Vec2(xPos, yPos));
	float width = sprSBlowUp->getContentSize().width;
	float height = sprSBlowUp->getContentSize().height;
	Vector<SpriteFrame*> sblowUp_animFrames;
	sblowUp_animFrames.reserve(4);
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_1.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_2.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_3.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_4.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_5.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_6.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_7.png", Rect(0, 0, 18, 18)));

	auto sprSBlowUp_animation = Animation::createWithSpriteFrames(sblowUp_animFrames, 0.035f);
	auto sprSBlowUp_animate = Animate::create(sprSBlowUp_animation);
	sprSBlowUp->runAction(sprSBlowUp_animate);
	sprSBlowUp->setScale(2);
	return sprSBlowUp;
}

Sprite* SmallBlowUpBoss()
{
	//Explosion Animation
	auto sprSBlowUp = Sprite::create("SBlowUp_1.png");
	float width = sprSBlowUp->getContentSize().width;
	float height = sprSBlowUp->getContentSize().height;
	Vector<SpriteFrame*> sblowUp_animFrames;
	sblowUp_animFrames.reserve(4);
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_1.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_2.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_3.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_4.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_5.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_6.png", Rect(0, 0, 18, 18)));
	sblowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_7.png", Rect(0, 0, 18, 18)));

	auto sprSBlowUp_animation = Animation::createWithSpriteFrames(sblowUp_animFrames, 0.035f);
	auto sprSBlowUp_animate = Animate::create(sprSBlowUp_animation);
	sprSBlowUp->runAction(sprSBlowUp_animate);

	int minY = 500;
	int maxY = 700;
	int minX = 50;
	int maxX = 380;
	int rangeX = maxX - minX;
	int rangeY = maxY - minY;
	int randomY = (rand() % 150) + 585;
	int randomX = (rand() % 50) + 190;
	int randomInt = (rand() % 2) + 1;
	sprSBlowUp->setPosition(randomX, randomY);
	sprSBlowUp->setScale(2);

	return sprSBlowUp;
}

Sprite* OneUp(float whenToSpawn)
{
	auto sprOneUp = Sprite::create("1Up.png");
	sprOneUp->setTag(70);

	auto sprEnemyContentSize = sprOneUp->getContentSize();
	int minY = 500;
	int maxY = 700;
	int minX = 50;
	int maxX = 380;
	int rangeX = maxX - minX;
	int rangeY = maxY - minY;
	int randomY = (rand() % rangeY) + minY;
	int randomX = (rand() % rangeX) + minX;
	int randomInt = (rand() % 2) + 1;
	if (randomInt == 1)
		sprOneUp->setPosition(Vec2(500, 700));
	if (randomInt == 2)
		sprOneUp->setPosition(Vec2(-500, 700));

	//sprOneUp->setPosition(Vec2(sprEnemyContentSize.width + sprEnemyContentSize.width / 2, randomY));
	// 2
	int minDuration = 2.0;
	int maxDuration = 4.0;
	int rangeDuration = maxDuration - minDuration;
	int randomDuration = (rand() % rangeDuration) + minDuration;

	// 3
	auto actionWait = DelayTime::create(whenToSpawn);
	auto actionMove1 = MoveTo::create(randomDuration, Vec2(50, randomY));
	randomY = (rand() % rangeY) + minY;
	auto actionMove2 = MoveTo::create(randomDuration, Vec2(400, randomY));
	//sprEnemy->runAction(RepeatForever::create(Sequence::create(actionMove1, actionMove2, nullptr)));
	auto repeater = Sequence::create(actionMove1, actionMove2, nullptr);
	auto sequence = Sequence::create(actionWait, Repeat::create(repeater, 99999), nullptr);
	sprOneUp->runAction(sequence);

	//Enemy Physics
	auto sprOneUp_physBody = PhysicsBody::createBox(sprOneUp->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
	sprOneUp_physBody->setGravityEnable(false);
	sprOneUp_physBody->setContactTestBitmask(0x01);
	sprOneUp->setPhysicsBody(sprOneUp_physBody);

	auto xPos = sprOneUp->getPositionX();
	auto yPos = sprOneUp->getPositionY();
	log("%f     %f", xPos, yPos);

	return sprOneUp;
}

Sprite* Boss(float whenToSpawn, float w)
{
	auto sprBoss = Sprite::create("boss.png");
	sprBoss->setTag(80);
	sprBoss->setPosition(800, 1200);
	sprBoss->setOpacity(0);
	// 2
	int minDuration = 2.0;
	int maxDuration = 4.0;
	int rangeDuration = maxDuration - minDuration;
	int randomDuration = (rand() % rangeDuration) + minDuration;

	// 3
	auto actionWait = DelayTime::create(whenToSpawn);
	auto fadeIn = FadeIn::create(1);
	auto MoveTo = MoveTo::create(0, Vec2(w / 2, 650));
	//sprEnemy->runAction(RepeatForever::create(Sequence::create(actionMove1, actionMove2, nullptr)));
	auto sequence = Sequence::create(actionWait, MoveTo, fadeIn, nullptr);
	sprBoss->runAction(sequence);

	//Enemy Physics
	auto sprBoss_physBody = PhysicsBody::createBox(sprBoss->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
	sprBoss_physBody->setGravityEnable(false);
	sprBoss_physBody->setContactTestBitmask(0x01);
	sprBoss->setPhysicsBody(sprBoss_physBody);

	auto xPos = sprBoss->getPositionX();
	auto yPos = sprBoss->getPositionY();
	log("%f     %f", xPos, yPos);

	return sprBoss;
}

Sprite* BossShotPattern1(int counter, float width, float height, int loop)
{
	auto bossShot1 = Sprite::create("bossshotdot.png");
	bossShot1->setTag(40);
	bossShot1->setOpacity(0);
	if (counter > 7)
		bossShot1->setPosition(Vec2(((counter - 8) * width / 7) - 35, 530));
	else
		bossShot1->setPosition(Vec2((counter * width / 7) - 35, 530));

	auto fadeIn = FadeIn::create(0.2f);
	auto fullDelay = DelayTime::create(loop * 1.5f);
	if (counter > 7)
	{
		auto delay = DelayTime::create(abs(counter - 15) / 8.f);
		auto bossShot1_velocity = MoveBy::create(1.8f, Vec2(0, -780));
		auto bossShot1_acc = EaseIn::create(bossShot1_velocity, 2);
		auto sequence = Sequence::create(fullDelay, delay, fadeIn, bossShot1_acc, nullptr);
		bossShot1->runAction(sequence);
	}
	else
	{
		auto delay = DelayTime::create(abs(counter - 1) / 8.f);
		auto bossShot1_velocity = MoveBy::create(1.8f, Vec2(0, -780));
		auto bossShot1_acc = EaseIn::create(bossShot1_velocity, 2);
		auto sequence = Sequence::create(fullDelay, delay, fadeIn, bossShot1_acc, nullptr);
		bossShot1->runAction(sequence);
	}

	////SHOT PHYSICS
	//auto bossShot1_physBody = PhysicsBody::createBox(bossShot1->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
	//bossShot1_physBody->setGravityEnable(false);
	//bossShot1_physBody->setContactTestBitmask(0x01);
	//bossShot1->setPhysicsBody(bossShot1_physBody);

	return bossShot1;
}

Sprite* BossIdleR()
{
	auto sprMissleR = Sprite::create("missleright.png");
	sprMissleR->setTag(40);
	sprMissleR->setPosition(320, 530);
	sprMissleR->setScale(1.7f);
	auto shoot = MoveBy::create(2, Vec2(400, -400));
	sprMissleR->runAction(shoot);
	//Physics
	auto sprMissleR_physBody = PhysicsBody::createBox(sprMissleR->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
	sprMissleR_physBody->setGravityEnable(false);
	sprMissleR_physBody->setContactTestBitmask(0x01);
	sprMissleR->setPhysicsBody(sprMissleR_physBody);

	return sprMissleR;
}

Sprite* BossIdleL()
{
	auto sprMissleR = Sprite::create("missleleft.png");
	sprMissleR->setTag(40);
	sprMissleR->setPosition(112, 530);
	sprMissleR->setScale(1.7f);
	auto shoot = MoveBy::create(2, Vec2(-400, -400));
	sprMissleR->runAction(shoot);
	//Physics
	auto sprMissleR_physBody = PhysicsBody::createBox(sprMissleR->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
	sprMissleR_physBody->setGravityEnable(false);
	sprMissleR_physBody->setContactTestBitmask(0x01);
	sprMissleR->setPhysicsBody(sprMissleR_physBody);

	return sprMissleR;
}

Sprite* BossIdleM()
{
	auto sprMissleR = Sprite::create("misslemiddle.png");
	sprMissleR->setTag(40);
	sprMissleR->setPosition(216, 530);
	sprMissleR->setScale(1.7f);
	auto shoot = MoveBy::create(4.4f, Vec2(0, -800));
	sprMissleR->runAction(shoot);
	//Physics
	auto sprMissleR_physBody = PhysicsBody::createBox(sprMissleR->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
	sprMissleR_physBody->setGravityEnable(false);
	sprMissleR_physBody->setContactTestBitmask(0x01);
	sprMissleR->setPhysicsBody(sprMissleR_physBody);

	return sprMissleR;
}

Sprite* BossCharge()
{
	//Explosion Animation
	auto sprBlowUp = Sprite::create("BlowUp_1.png");
	sprBlowUp->setPosition(Vec2(216, 500));
	float width = sprBlowUp->getContentSize().width;
	float height = sprBlowUp->getContentSize().height;
	Vector<SpriteFrame*> blowUp_animFrames;
	blowUp_animFrames.reserve(4);
	blowUp_animFrames.pushBack(SpriteFrame::create("charge1.png", Rect(0, 0, 30, 30)));
	blowUp_animFrames.pushBack(SpriteFrame::create("charge2.png", Rect(0, 0, 30, 30)));
	blowUp_animFrames.pushBack(SpriteFrame::create("charge3.png", Rect(0, 0, 30, 30)));
	blowUp_animFrames.pushBack(SpriteFrame::create("charge4.png", Rect(0, 0, 30, 30)));
	blowUp_animFrames.pushBack(SpriteFrame::create("charge5.png", Rect(0, 0, 30, 30)));
	blowUp_animFrames.pushBack(SpriteFrame::create("charge6.png", Rect(0, 0, 30, 30)));
	blowUp_animFrames.pushBack(SpriteFrame::create("charge7.png", Rect(0, 0, 30, 30)));
	blowUp_animFrames.pushBack(SpriteFrame::create("SBlowUp_7.png", Rect(0, 0, 30, 30)));

	auto sprBlowUp_animation = Animation::createWithSpriteFrames(blowUp_animFrames, 0.2f);
	auto sprBlowUp_animate = Animate::create(sprBlowUp_animation);
	sprBlowUp->runAction(sprBlowUp_animate);

	return sprBlowUp;
}

Sprite* BossLaser()
{
	auto bossLaser = Sprite::create("bosslaser.png");
	bossLaser->setTag(40);
	bossLaser->setPosition(Vec2(216, 100));
	bossLaser->setScale(5);
	//bossLaser->setOpacity(0);

	auto delay = DelayTime::create(2);
	auto fade = FadeIn::create(0.2f);
	auto sequence = Sequence::create(delay, fade, nullptr);
	bossLaser->runAction(sequence);

	return bossLaser;
}



int ScoreUpdate(int &score, int add)
{
	score = score + add;
	log("||||||||||||%i", score);
	return score;
}


Scene* GameOver(int score)
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();

	auto scene = Scene::createWithPhysics();

	//Game Over Message
	auto message = Label::createWithTTF("GAME OVER", "fonts/Marker Felt.ttf", 36);
	message->setColor(Color3B(255, 0, 0));
	message->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	scene->addChild(message);

	auto message2 = Label::createWithTTF("Score: " + to_string(50 * (score - 300)), "fonts/Marker Felt.ttf", 28);
	message2->setColor(Color3B(255, 100, 100));
	message2->setPosition(visibleSize.width / 2, 200);
	scene->addChild(message2);

	auto menu = Menu::create(GetCloseItem(), NULL);
	scene->addChild(menu);

	return scene;
}

Scene* GameWin(int score)
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();

	auto scene = Scene::createWithPhysics();

	//Game WIN Message
	auto message = Label::createWithTTF("GAME COMPLETE!", "fonts/Marker Felt.ttf", 36);
	message->setColor(Color3B(100, 255, 100));
	message->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	scene->addChild(message);

	auto message2 = Label::createWithTTF("Score: " + to_string(50 * (score - 300)), "fonts/Marker Felt.ttf", 28);
	message2->setColor(Color3B(0, 255, 0));
	message2->setPosition(visibleSize.width / 2, 200);
	scene->addChild(message2);

	auto menu = Menu::create(GetCloseItem(), NULL);
	scene->addChild(menu);

	return scene;
}

MenuItemImage* GetCloseItem()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();

	auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",
		[&](Ref* sender) {
		//go back to menu
		Director::getInstance()->popScene();
	}
	);
	closeItem->setAnchorPoint(Vec2(1, 1));
	closeItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

	return closeItem;
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