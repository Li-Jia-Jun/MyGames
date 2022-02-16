#include"AboutGame.h"
#include"MainMenu.h"

bool AboutGame::init() {
	if (!Layer::init()) {
		return false;
	}

	Size size = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Sprite* bgImg = Sprite::create("res/AboutGame/AboutGamebg.png");
	bgImg->setPosition(Vec2(origin.x + size.width / 2, origin.y + size.height / 2));
	this->addChild(bgImg);

	auto* button = MenuItemImage::create("res/AboutGame/back1.png", "res/AboutGame/back2.png", this, menu_selector(AboutGame::backToMenu));
	button->setPosition(Vec2(48+223,95));
	auto menu = Menu::create(button, NULL);
	menu->setPosition(Point::ZERO);
	this->addChild(menu);
	

	return true;
}

Scene* AboutGame::createScene() {
	Scene* scene = Scene::create();
	AboutGame* layer = AboutGame::create();
	scene->addChild(layer);
	return scene;
}

void AboutGame::backToMenu(Ref *pSender) {

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


void AboutGame::onEnter() {
	Layer::onEnter();
}

void AboutGame::onEnterTransitionDidFinish() {
	Layer::onEnterTransitionDidFinish();
}