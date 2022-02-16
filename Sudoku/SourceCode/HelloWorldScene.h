#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

//#include"cocos-ext.h"

USING_NS_CC;
//USING_NS_CC_EXT;

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    //void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

public:
	//跳到主菜单
	void enterMainMenu();

	//生命周期
	virtual void onEnter();
	virtual void TransitionDidFinish();
	//virutal void onExit();
};

#endif // __HELLOWORLD_SCENE_H__
