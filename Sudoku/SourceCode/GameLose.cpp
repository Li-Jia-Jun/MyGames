#include"GameLose.h"
#include"GameWin.h"
#include"MainMenu.h"
//还需要添加重新开始的场景类头文件

bool GameLose::init() {
	if (!Layer::init()) {
		return false;
	}
	SimpleAudioEngine::getInstance()->playBackgroundMusic("res/GameWin/music/lose.wav");
	Size size = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Sprite* bgImg = Sprite::create("res/GameLose/GameLosebg.png");
	bgImg->setPosition(Vec2(origin.x + size.width / 2, origin.y + size.height/2));
	this->addChild(bgImg);

	MenuItem* button1 = MenuItemImage::create("res/GameLose/reTry1.png", "res/GameLose/reTry2.png", this, menu_selector(GameLose::reTry));
	MenuItem* button2 = MenuItemImage::create("res/GameLose/back1.png", "res/GameLose/back2.png", this, menu_selector(GameLose::backToMenu));

	button1->setPosition(Vec2(270,333));
	button2->setPosition(Vec2(270,217));

	auto* menu = Menu::create(button1, button2, NULL);
	menu->setPosition(Point::ZERO);
	this->addChild(menu,1);


	return true;
}

Scene* GameLose::createScene() {
	Scene* scene = Scene::create();
	GameLose* layer = GameLose::create();
	scene->addChild(layer);

	return scene;
}

void GameLose::reTry(Ref *pSender) {

	MenuItem * clickedItem = (MenuItem*)pSender;
	auto *st = ScaleTo::create(0.05f, 0.9f);
	auto *st2 = ScaleTo::create(0.1f, 1.0f);
	Sequence *sq = Sequence::create(st, st2, NULL);
	clickedItem->runAction(sq);

	SimpleAudioEngine::getInstance()->playEffect("res/GameMenu/music/click.wav");

	//重新创建一个游戏场景并导入当前失败的关卡
	//level

	//Scene* scene = GameWin::createScene();
	//auto* tt = TransitionFade::create(0.4f, scene);
	//Director::getInstance()->replaceScene(tt);

}

void GameLose::backToMenu(Ref *pSender) {

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

void GameLose::onEnter() {
	Layer::onEnter();
}

void GameLose::onEnterTransitionDidFinish() {
	Layer::onEnterTransitionDidFinish();
}