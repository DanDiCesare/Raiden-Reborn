#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "DPad.h"
#include "ui/CocosGUI.h"
#include <CCScheduler.h>
//#include "SneakyButton.h"
//#include "SneakyButtonSkinnedBase.h"
//#include "SneakyJoystickSkinnedBase.h"
//#include "SneakyJoystick.h"

using namespace cocos2d;

class HelloWorld : public cocos2d::Layer
{
public:

    static cocos2d::Scene* createScene();


    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
	//bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
	//void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
	void GetPositionE(float dt);
	void addEnemy(float dt);
	void addEnemyShot(Sprite* sprEnemy);
	void EnemyWideHitTime(Sprite* sprEnemyWide);
	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);

	unsigned int score;

private:

	cocos2d::Label *scoreLabel;
};


#endif // __HELLOWORLD_SCENE_H__
