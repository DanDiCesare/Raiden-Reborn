#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include <iostream>
using namespace std;

USING_NS_CC;

Scene* GetScene1();
Scene* GetScene2();
Scene* GetScene3();
MenuItemImage* GetCloseItem();

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
			Scene* scene = GetScene1();
			Director::getInstance()->pushScene(scene);
		}
	);
	menuItem1->setPosition(Vec2(menuItem1->getPosition().x, menuItem1->getPosition().y + 30));

	auto menuItemLabel2 = Label::createWithTTF("Menu Item 2", "fonts/Marker Felt.ttf", 24);
	auto menuItem2 = MenuItemLabel::create(menuItemLabel2,
		[&](Ref* sender) {
		//Code here
			Director::getInstance()->pushScene(GetScene2());		
		}
	);
	menuItem2->setPosition(Vec2(menuItem2->getPosition().x, menuItem1->getPosition().y - 30));

	auto menuItemLabel3 = Label::createWithTTF("Menu Item 3", "fonts/Marker Felt.ttf", 24);
	auto menuItem3 = MenuItemLabel::create(menuItemLabel3,
		[&](Ref* sender) {
		//Code here
			Director::getInstance()->pushScene(GetScene3());
		}
	);
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

Scene* GetScene1()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();

	auto scene = Scene::create();

	auto sprBlueGuy = Sprite::create("Blue_Front1.png");
	scene->addChild(sprBlueGuy, 0);
	sprBlueGuy->setPosition(Vec2(0, 100));
	auto sprBlueGuy_move1 = MoveBy::create(2, Vec2(400, 0));
	auto sprBlueGuy_move2 = MoveBy::create(2, Vec2(-400, 0));
	auto sprBlueGuy_sequence = Sequence::create(sprBlueGuy_move1, sprBlueGuy_move2, nullptr);
	sprBlueGuy->runAction(sprBlueGuy_sequence);
	
	//Create listener for sprite
	auto sprBlueGuy_listener = EventListenerTouchOneByOne::create();
	//Trigger forr lsitener of sprite
	sprBlueGuy_listener->onTouchBegan = [=](Touch* touch, Event* event) {
		auto touchPoint = touch->getLocation();
		auto sprBlueGuy_box = sprBlueGuy->getBoundingBox();

		if (sprBlueGuy_box.containsPoint(touchPoint))
		{
			log("sprite is touched!");
		}
		return true;
	};
	eventDispatcher->addEventListenerWithSceneGraphPriority(sprBlueGuy_listener, scene);


	auto sprStar = Sprite::create("Star.png");
	sprStar->setScale(0.2);
	scene->addChild(sprStar);
	sprStar->setPosition(Vec2(visibleSize.width / 2, 200));

	//Create listener for sprite
	auto sprStar_listener = EventListenerTouchOneByOne::create();
	//Trigger forr lsitener of sprite
	sprStar_listener->onTouchBegan = [=](Touch* touch, Event* event) {
		auto touchPoint = touch->getLocation();
		auto sprStar_box = sprStar->getBoundingBox();

		if (touchPoint.x < sprStar_box.getMinX())
		{
			auto sprStar_move = MoveBy::create(1, Vec2(-20, 0));
			sprStar->runAction(sprStar_move);
		}

		else if (touchPoint.x > sprStar_box.getMinX())
		{
			auto sprStar_move = MoveBy::create(1, Vec2(20, 0));
			sprStar->runAction(sprStar_move);
		}
		return true;
	};
	eventDispatcher->addEventListenerWithSceneGraphPriority(sprStar_listener, scene);


	auto menu = Menu::create(GetCloseItem(), NULL);
	scene->addChild(menu);
	return scene;
}

Scene* GetScene2()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();

	auto scene = Scene::create();

	Vector<Sprite*> sprites;
	int currentY = 40;
	auto sprBlueGuy_move1 = MoveBy::create(2, Vec2(400, 0));
	auto sprBlueGuy_move2 = MoveBy::create(2, Vec2(-400, 0));
	auto sprBlueGuy_sequence = Sequence::create(sprBlueGuy_move1, sprBlueGuy_move2, nullptr);

	for (int x = 0; x < 5; x++)
	{
		auto sprBlueGuy = Sprite::create("Blue_Front1.png");
		sprBlueGuy->setScale(.5);
		sprBlueGuy->setTag(x);
		scene->addChild(sprBlueGuy);
		sprBlueGuy->setPosition(Vec2(0, currentY));
		if(x == 0)
			sprBlueGuy->runAction(RepeatForever::create(sprBlueGuy_sequence));

		else
			sprBlueGuy->runAction(RepeatForever::create(sprBlueGuy_sequence->clone()));
		sprites.pushBack(sprBlueGuy);
		currentY += 40;

	}

	//Create listener for sprite
	auto sprBlueGuy_listener = EventListenerTouchOneByOne::create();
	//Trigger forr lsitener of sprite
	sprBlueGuy_listener->onTouchBegan = [=](Touch* touch, Event* event) {
		auto touchPoint = touch->getLocation();

		for each (Sprite* spr in sprites)
		{
			auto spr_box = spr->getBoundingBox();
			if (spr_box.containsPoint(touchPoint))
			{
				log("Sprite %i touched!!", spr->getTag());
			}
		}
		return true;
	};
	eventDispatcher->addEventListenerWithSceneGraphPriority(sprBlueGuy_listener, scene);


	auto menu = Menu::create(GetCloseItem(), NULL);
	scene->addChild(menu);

	return scene;
}

Scene* GetScene3()
{
	const int BOUNDARY_TAG = 5;
	const int SPRITE_TAG = 10;
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
	
	auto scene = Scene::createWithPhysics();

	//boundary nde
	Node *boundaryNode = Node::create();
	boundaryNode->setAnchorPoint(Point(0.5, 0.5));
	boundaryNode->setPosition(Point(visibleSize.width / 2.0, visibleSize.height / 2.0));

	//Physics body for boundary node
	auto boundaryBody = PhysicsBody::createEdgeBox(visibleSize);;
	boundaryBody->setDynamic(false);
	boundaryBody->setContactTestBitmask(0x01);

	//Apply Physics body to the boundary
	boundaryNode->setPhysicsBody(boundaryBody);
	scene->addChild(boundaryNode);

	for (int x = 0; x < 10; x++)
	{
		//sprite
		auto sprite = Sprite::create("Blue_Front1.png");
		sprite->setPosition(Vec2(cocos2d::random(40, (int)visibleSize.width - 40), cocos2d::random(40, (int)visibleSize.height - 40)));
		scene->addChild(sprite);
		sprite->setTag(10);

		//Create a PhysicsBody for sprite
		auto physicsBody = PhysicsBody::createBox(sprite->getContentSize(), PhysicsMaterial(0.1f, 1.0, 0.0f));
		physicsBody->setVelocity(Vec2(cocos2d::random(-500, 500), cocos2d::random(-500, 500)));
		physicsBody->setGravityEnable(false);
		physicsBody->setContactTestBitmask(0x01);

		//Apply Physics body to sprite
		sprite->setPhysicsBody(physicsBody);
	}

	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = [=](PhysicsContact& contact) {
		log("contact happened");

		auto nodeA = contact.getShapeA()->getBody()->getNode();
		auto nodeB = contact.getShapeB()->getBody()->getNode();

		if (nodeA && nodeB)
		{
			if (nodeA->getTag() == 10)//if node a is the sprite and b is the boundary
			{
				nodeA->removeFromParentAndCleanup(true);
			}
			else if (nodeB->getTag() == 10)//if node b is the sprite
			{
				nodeB->removeFromParentAndCleanup(true);
			}
			if (nodeA->getTag() == 10 && nodeB->getTag == 10)//two sprites hit each oter
			{
				nodeA->removeFromParentAndCleanup(true);
				//nodeA->getPhysicsBody()->setVelocity;
			}
		}
		return true;
	};
	eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, scene); 


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
