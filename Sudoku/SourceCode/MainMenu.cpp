#include"MainMenu.h"
#include"GameMenu.h"
#include"AboutGame.h"

using namespace CocosDenshion;

Scene *MainMenu::createScene() {
	Scene *scene = Scene::create();
	MainMenu *mainMenuLayer = MainMenu::create();
	scene->addChild(mainMenuLayer);

	return scene;
}

bool MainMenu::init() {
	if (!Layer::init()) {
		return false;
	}

	Size size = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(1);
	SimpleAudioEngine::getInstance()->playBackgroundMusic("res/MainMenu/music/game.wav", true);

	Sprite *bgImg = Sprite::create("res/MainMenu/MainMenubg.png");
	bgImg->setPosition(Vec2(size.width / 2, size.height / 2));
	this->addChild(bgImg);
	bgImg->setOpacity(0);

	
	//红色不是正常状态
	auto *button1 = MenuItemImage::create("res/MainMenu/enter2.png", "res/MainMenu/enter1.png",this, menu_selector(MainMenu::enterGameMenu));
	auto *button2 = MenuItemImage::create("res/MainMenu/about1.png", "res/MainMenu/about2.png", this, menu_selector(MainMenu::enterAboutGame));
	auto *button3 = MenuItemImage::create("res/MainMenu/exit1.png", "res/MainMenu/exit2.png", this, menu_selector(MainMenu::menuCloseCallback));
	button1->setPosition(Vec2(129+141,479+141));
	button2->setPosition(Vec2(206+65,335+37));
	button3->setPosition(Vec2(206+65,235+37));

	auto *menu = Menu::create(button1, button2, button3, NULL);
	menu->setPosition(Point::ZERO);
	this->addChild(menu);

	menu->setScale(0);
	auto *st = ScaleTo::create(0.4f, 1.2f);
	auto *dt = DelayTime::create(0.1f);
	auto *st2 = ScaleTo::create(0.2f, 1.0f);
	menu->runAction(Sequence::create(st, dt, st2, NULL));

	auto *ain = CCFadeIn::create(0.6f);
	bgImg->runAction(ain);

	//this->setKeypadEnabled(true);
	//isPause = false;

	return true;
}


void MainMenu::enterGameMenu(Ref *pSender) {

	MenuItem * clickedItem = (MenuItem*)pSender;
	auto *st = ScaleTo::create(0.05f, 0.9f);
	auto *st2 = ScaleTo::create(0.1f, 1.0f);
	Sequence *sq = Sequence::create(st, st2, NULL);
	clickedItem->runAction(sq);

	SimpleAudioEngine::getInstance()->playEffect("res/GameMenu/music/error.wav");
	
	Scene *gameMenu = GameMenu::createScene();
	auto *tt = TransitionFade::create(1.0f, gameMenu);
	Director::getInstance()->replaceScene(tt);
	
}

void MainMenu::enterAboutGame(Ref *pSender) {

	MenuItem * clickedItem = (MenuItem*)pSender;
	auto *st = ScaleTo::create(0.05f, 0.9f);
	auto *st2 = ScaleTo::create(0.1f, 1.0f);
	Sequence *sq = Sequence::create(st, st2, NULL);
	clickedItem->runAction(sq);

	SimpleAudioEngine::getInstance()->playEffect("res/GameMenu/music/click.wav");

	Scene* aboutGame = AboutGame::createScene();
	auto* tt = TransitionFade::create(0.4f, aboutGame);
	Director::getInstance()->replaceScene(tt);
}

void MainMenu::menuCloseCallback(Ref *pSender) {

	SimpleAudioEngine::getInstance()->playEffect("res/GameMenu/music/click.wav");

	MenuItem * clickedItem = (MenuItem*)pSender;
	auto *st = ScaleTo::create(0.05f, 0.9f);
	auto *st2 = ScaleTo::create(0.1f, 1.0f);
	Sequence *sq = Sequence::create(st, st2, NULL);
	clickedItem->runAction(sq);

	Director::getInstance()->end();

}



void MainMenu::onEnter() {
	Layer::onEnter();
}

void MainMenu::TransitionDidFinish() {

}
