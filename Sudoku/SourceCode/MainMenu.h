#pragma once

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

class MainMenu : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();


	// implement the "static create()" method manually
	CREATE_FUNC(MainMenu);

	bool isPause;
	Animate* animate;

public:
	//跳到主菜单
	void enterGameMenu(Ref *pSender);
	void enterAboutGame(Ref *pSender);
	void menuCloseCallback(Ref *pSender);

	//生命周期
	virtual void onEnter();
	virtual void TransitionDidFinish();
	//virutal void onExit();
};