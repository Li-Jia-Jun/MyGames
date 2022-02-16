#pragma once
#include "cocos2d.h"
#include "SimpleAudioEngine.h"


USING_NS_CC;
using namespace CocosDenshion;


class GameLose : public Layer {

public:
	virtual bool init();
	static Scene* createScene();
	CREATE_FUNC(GameLose);

	void backToMenu(Ref *pSender);
	void reTry(Ref *pSender);

	virtual void onEnter();
	virtual void onEnterTransitionDidFinish();

public:
	int level;
};