#include"GameWin.h"
#include"GameLose.h"
#include"MainMenu.h"
//还需要添加重新开始的场景类头文件

bool GameWin::init() {
	if (!Layer::init()) {
		return false;
	}
	SimpleAudioEngine::getInstance()->playBackgroundMusic("res/GameWin/music/win.wav");

	Size size = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Sprite* bgImg = Sprite::create("res/GameWin/GameWinbg.png");
	bgImg->setPosition(Vec2(origin.x + size.width / 2, origin.y + size.height / 2));
	this->addChild(bgImg);

	MenuItem* button1 = MenuItemImage::create("res/GameWin/next1.png", "res/GameWin/next2.png", this, menu_selector(GameWin::goToNext));
	MenuItem* button2 = MenuItemImage::create("res/GameWin/back1.png", "res/GameWin/back2.png", this, menu_selector(GameWin::backToMenu));

	button1->setPosition(Vec2(270, 275));
	button2->setPosition(Vec2(270, 178));

	auto* menu = Menu::create(button1, button2, NULL);
	menu->setPosition(Point::ZERO);
	this->addChild(menu, 1);

	//下面是用时和步数label
	//用时
	String* Str1 = String::createWithFormat("res/GameWin/num%d.png", time1);
	auto* timeImg1 = Sprite::create(Str1->getCString());
	String* Str2 = String::createWithFormat("res/GameWin/num%d.png", time2);
	auto* timeImg2 = Sprite::create(Str2->getCString());
	String* Str3 = String::createWithFormat("res/GameWin/num%d.png", time3);
	auto* timeImg3 = Sprite::create(Str3->getCString());
	String* Str4 = String::createWithFormat("res/GameWin/num%d.png", time4);
	auto* timeImg4 = Sprite::create(Str4->getCString());

	timeImg1->setPosition(Vec2(132, 403));
	timeImg2->setPosition(Vec2(161, 403));
	timeImg3->setPosition(Vec2(201, 403));
	timeImg4->setPosition(Vec2(229, 403));

	this->addChild(timeImg1);
	this->addChild(timeImg2);
	this->addChild(timeImg3);
	this->addChild(timeImg4);

	String* Strp1 = String::createWithFormat("res/GameWin/num%d.png", step1);
	auto* stepImg1 = Sprite::create(Strp1->getCString());
	String* Strp2= String::createWithFormat("res/GameWin/num%d.png", step2);
	auto* stepImg2 = Sprite::create(Strp2->getCString());
	String* Strp3 = String::createWithFormat("res/GameWin/num%d.png", step3);
	auto* stepImg3 = Sprite::create(Strp3->getCString());

	stepImg1->setPosition(Vec2(329, 403));
	stepImg2->setPosition(Vec2(357, 403));
	stepImg3->setPosition(Vec2(385, 403));

	this->addChild(stepImg1);
	this->addChild(stepImg2);
	this->addChild(stepImg3);


	return true;
}

Scene* GameWin::createScene(int a, int b, int c) {
	Scene* scene = Scene::create();
	GameWin* layer = GameWin::create();
	layer->time4 = a;
	layer->time3 = b;
	layer->time2 = c;
	scene->addChild(layer);

	

	return scene;
}

void GameWin::goToNext(Ref *pSender) {

	MenuItem * clickedItem = (MenuItem*)pSender;
	auto *st = ScaleTo::create(0.05f, 0.9f);
	auto *st2 = ScaleTo::create(0.1f, 1.0f);
	Sequence *sq = Sequence::create(st, st2, NULL);
	clickedItem->runAction(sq);

	SimpleAudioEngine::getInstance()->playEffect("res/GameMenu/music/click.wav");
	SimpleAudioEngine::getInstance()->stopBackgroundMusic();

	//创建下一关的游戏场景并导入当前失败的关卡，暂时先跳转到失败界面
	//level

	Scene* scene = GameLose::createScene();
	auto* tt = TransitionFade::create(0.4f, scene);
	Director::getInstance()->replaceScene(tt);
}

void GameWin::backToMenu(Ref *pSender) {

	MenuItem * clickedItem = (MenuItem*)pSender;
	auto *st = ScaleTo::create(0.05f, 0.9f);
	auto *st2 = ScaleTo::create(0.1f, 1.0f);
	Sequence *sq = Sequence::create(st, st2, NULL);
	clickedItem->runAction(sq);

	SimpleAudioEngine::getInstance()->playEffect("res/GameMenu/music/click.wav");
	SimpleAudioEngine::getInstance()->stopBackgroundMusic();

	Scene *mainMenu = MainMenu::createScene();
	auto *tt = TransitionFade::create(0.4f, mainMenu);
	Director::getInstance()->replaceScene(tt);

}


void GameWin::onEnter() {
	Layer::onEnter();
}

void GameWin::onEnterTransitionDidFinish() {
	Layer::onEnterTransitionDidFinish();
}