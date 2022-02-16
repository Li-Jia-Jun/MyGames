#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include"MainMenu.h"

USING_NS_CC;
using namespace CocosDenshion;

#define MUSIC_LOGO "res/HelloWorld/music/logo.wav"
#define MUSIC_BG "res/HelloWorld/music/bg.wav"
#define MUSIC_GAME "res/HelloWorld/music/game.wav"
#define MUSIC_CLICK "res/HelloWorld/music/click.wav"
#define MUSIC_START "res/HelloWorld/music/start.wav"
#define MUSIC_HARRY "res/HelloWorld/music/harry.wav"
#define MUSIC_TIME "res/HelloWorld/music/time.wav"
#define MUSIC_ERROR "res/HelloWorld/music/error.wav"
#define MUSIC_PROPS "res/HelloWorld/music/props.wav"
#define MUSIC_WIN "res/HelloWorld/music/win.wav"
#define MUSIC_LOSE "res/HelloWorld/music/lose.wav"


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

    if ( !Layer::init() )
    {
        return false;
    }
    
    auto size = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


	SimpleAudioEngine::getInstance()->preloadBackgroundMusic(MUSIC_LOGO);
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic(MUSIC_BG);
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic(MUSIC_GAME);
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic(MUSIC_HARRY);

	SimpleAudioEngine::getInstance()->preloadEffect(MUSIC_CLICK);
	SimpleAudioEngine::getInstance()->preloadEffect(MUSIC_START);
	SimpleAudioEngine::getInstance()->preloadEffect(MUSIC_TIME);
	SimpleAudioEngine::getInstance()->preloadEffect(MUSIC_ERROR);
	SimpleAudioEngine::getInstance()->preloadEffect(MUSIC_PROPS);

	SimpleAudioEngine::getInstance()->preloadEffect(MUSIC_WIN);
	SimpleAudioEngine::getInstance()->preloadEffect(MUSIC_LOSE);

	SimpleAudioEngine::getInstance()->playBackgroundMusic(MUSIC_LOGO, false);

	SpriteFrameCache * spriteFrameCache = SpriteFrameCache::getInstance();
	int index = 1;
	SpriteFrame * frame = NULL;
	Vector<SpriteFrame *>frameArray;

	do {
	frame = spriteFrameCache->getSpriteFrameByName(__String::createWithFormat("%d.png", index)->getCString());
	if (0 == frame) {
	break;
	}
	frameArray.pushBack(frame);
	index++;
	} while (true);

	Animation * animation = Animation::createWithSpriteFrames(frameArray);
	animation->setLoops(1);
	animation->setRestoreOriginalFrame(false);
	animation->setDelayPerUnit(0.13f);
	auto* animate = Animate::create(animation);

	Sprite* beginImg = Sprite::create("res/HelloWorld/1.png");
	beginImg->setPosition(Vec2(origin.x + size.width / 2, origin.y + size.height / 2));
	this->addChild(beginImg,2);
	beginImg->setOpacity(0);
	
	auto* in = CCFadeIn::create(0.3f);
	auto* dt2 = DelayTime::create(0.2f);
	auto* dt = DelayTime::create(0.9f);
	auto* out = CCFadeOut::create(2.0f);
	CallFunc *enterMenu = CallFunc::create(this, callfunc_selector(HelloWorld::enterMainMenu));
	auto* sq = Sequence::create(in,dt2, animate,animate,dt,out, enterMenu,NULL);

	beginImg->runAction(sq);
    
    return true;
}

//
//void HelloWorld::menuCloseCallback(Ref* pSender)
//{
//    //Close the cocos2d-x game scene and quit the application
//    Director::getInstance()->end();
//
//    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//    exit(0);
//#endif
//    
//    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
//    
//    //EventCustom customEndEvent("game_scene_close_event");
//    //_eventDispatcher->dispatchEvent(&customEndEvent);
//    
//    
//}


void HelloWorld::enterMainMenu() {
	Scene *mainmenu = MainMenu::createScene();
	auto *tt = TransitionCrossFade::create(0.2f, mainmenu);
	Director::getInstance()->replaceScene(tt);
}


void HelloWorld::onEnter() {
	Layer::onEnter();
}

void HelloWorld::TransitionDidFinish() {
	Layer::onEnterTransitionDidFinish();
}

