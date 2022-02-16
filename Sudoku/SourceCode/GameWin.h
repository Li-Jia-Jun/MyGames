#pragma once
#pragma once
#include "cocos2d.h"
#include "SimpleAudioEngine.h"


USING_NS_CC;
using namespace CocosDenshion;


class GameWin : public Layer {

public:
	virtual bool init();
	static Scene* createScene(int,int,int);
	CREATE_FUNC(GameWin);

	void backToMenu(Ref *pSender);
	void goToNext(Ref *pSender);

	virtual void onEnter();
	virtual void onEnterTransitionDidFinish();

public:
	int level;
	int time1=0;
	int time2=0;
	int time3=0;
	int time4=0;
	int step1=0;
	int step2=0;
	int step3=0;
};