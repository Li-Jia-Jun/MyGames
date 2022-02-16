#pragma once
#include "cocos2d.h"
#include "SimpleAudioEngine.h"


USING_NS_CC;
using namespace CocosDenshion;


class AboutGame : public Layer {

public:
	virtual bool init();
	static Scene* createScene();
	CREATE_FUNC(AboutGame);

	void backToMenu(Ref *pSender);

	virtual void onEnter();
	virtual void onEnterTransitionDidFinish();


};