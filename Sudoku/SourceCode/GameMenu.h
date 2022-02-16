#pragma once
#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "SimpleAudioEngine.h"
//#include"cocos-ext.h"

USING_NS_CC;
using namespace CocosDenshion;
//using namespace cocos2d::extension;
using namespace std;


class GameMenu : public Layer {

public:
	virtual bool init();

	static Scene *createScene();

	void menuCallback(Ref * psender);

	CREATE_FUNC(GameMenu);

	virtual bool onTouchBegan(Touch * touch, Event *event);
	virtual void onTouchMoved(Touch *touch, Event *event);
	virtual void onTouchEnded(Touch *touch, Event*event);

	int getMode(Touch *touch);

	void backToMenu(Ref *pSender);
	void enterGame(Ref *pSender);
	void notEnterGame(Ref *pSender);

	virtual void onEnter();
	virtual void onEnterTransitionDidFinish();


public:
	int mode;


};