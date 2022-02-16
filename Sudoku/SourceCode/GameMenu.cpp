#include"GameMenu.h"
#include"MainMenu.h"
#include"GameLose.h"
#include"GameWin.h"
#include"Game.h"

bool GameMenu::init() {

	if (!Layer::init()) {
		return true;
	}

	Size size = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	SimpleAudioEngine::getInstance()->playBackgroundMusic("res/GameMenu/music/game.wav");

	Sprite *bgImg = Sprite::create("res/GameMenu/GameMenubg.png");
	bgImg->setPosition(Vec2(origin.x + size.width / 2, origin.y + size.height / 2));
	this->addChild(bgImg,0);

	Sprite *modeIcon = Sprite::create("res/GameMenu/modeIcon.png");
	modeIcon->setAnchorPoint(Vec2(0, 0));
	modeIcon->setPosition(Vec2(175, 654));
	this->addChild(modeIcon, 1);

	auto *button1 = MenuItemImage::create("res/GameMenu/mode4_1.png", "res/GameMenu/mode4_2.png", this, menu_selector(GameMenu::enterGame));
	auto *button2 = MenuItemImage::create("res/GameMenu/mode6_3.png", "res/GameMenu/mode6_3.png", this, menu_selector(GameMenu::notEnterGame));
	auto *button3 = MenuItemImage::create("res/GameMenu/mode9_3.png", "res/GameMenu/mode9_3.png", this, menu_selector(GameMenu::notEnterGame));
	auto *button4 = MenuItemImage::create("res/GameMenu/back1.png", "res/GameMenu/back2.png", this, menu_selector(GameMenu::backToMenu));

	button1->setPosition(Vec2(177+93,283+79+79+36+36+40));
	button2->setPosition(Vec2(177+93,283+79+36+40));
	button3->setPosition(Vec2(177+93,283+40));
	button4->setPosition(Vec2(202+68,168+40));

	auto * menu = Menu::create(button1, button2, button3, button4, NULL);
	menu->setPosition(Point::ZERO);

	this->addChild(menu);


	return true;
}

Scene* GameMenu::createScene() {

	Scene *scene = Scene::create();
	GameMenu *GameMenuLayer = GameMenu::create();
	scene->addChild(GameMenuLayer);

	return scene;
}




bool GameMenu::onTouchBegan(Touch * touch, Event *event) {

	return true; //!
}

void GameMenu::onTouchMoved(Touch *touch, Event *event) {

}

void GameMenu::onTouchEnded(Touch *touch, Event*event) {

}

int GameMenu::getMode(Touch *touch) {

	return 0;  //!
}


void GameMenu::backToMenu(Ref *pSender) {

	MenuItem * clickedItem = (MenuItem*)pSender;
	auto *st = ScaleTo::create(0.05f, 0.9f);
	auto *st2 = ScaleTo::create(0.1f, 1.0f);
	Sequence *sq = Sequence::create(st, st2, NULL);
	clickedItem->runAction(sq);

	SimpleAudioEngine::getInstance()->playEffect("res/GameMenu/music/click.wav");

	Scene *mainMenu = MainMenu::createScene();
	auto *tt = TransitionFade::create(0.4f, mainMenu);
	Director::getInstance()->replaceScene(tt);
	
}



//跳转到游戏界面，没有包含头文件，需补充
void GameMenu::enterGame(Ref *pSender) {

	MenuItem * clickedItem = (MenuItem*)pSender;
	auto *st = ScaleTo::create(0.05f, 0.9f);
	auto *st2 = ScaleTo::create(0.1f, 1.0f);
	Sequence *sq = Sequence::create(st, st2, NULL);
	clickedItem->runAction(sq);

	SimpleAudioEngine::getInstance()->playEffect("res/GameMenu/music/error.wav");

	/*if (4 == mode) {
		Scene *gameScene = GameScene::createScene();
		Global::mode = this.mode;
		Director::getInstance()->replaceScene(gameScene);
	}*/

	SimpleAudioEngine::getInstance()->stopBackgroundMusic(false);

	Scene* scene = GameScene::createScene();
	auto* tt = TransitionFade::create(0.4f, scene);
	Director::getInstance()->replaceScene(tt);
		
		
	
}

void GameMenu::notEnterGame(Ref *pSender) {

	MenuItem * clickedItem = (MenuItem*)pSender;
	auto *st = ScaleTo::create(0.05f, 0.9f);
	auto *st2 = ScaleTo::create(0.1f, 1.0f);
	Sequence *sq = Sequence::create(st, st2, NULL);
	clickedItem->runAction(sq);

	SimpleAudioEngine::getInstance()->playEffect("res/GameMenu/music/error.wav");


}

void GameMenu::onEnter() {
	Layer::onEnter();
}

void GameMenu::onEnterTransitionDidFinish() {
	Layer::onEnterTransitionDidFinish();
}