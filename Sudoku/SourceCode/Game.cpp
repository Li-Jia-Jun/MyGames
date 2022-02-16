#include"Game.h"
#include"GameWin.h"
#include"GameLose.h"

using namespace CocosDenshion;

GameScene::GameScene()
{

}

GameScene::~GameScene()
{

}

Scene* GameScene::createScene()
{
	Scene* myScene = Scene::create();
	GameScene* myLayer = GameScene::create();
	myScene->addChild(myLayer,1,1);
	return myScene;
}

bool GameScene::init() {
	if (!Layer::init()) {
		return false;
	}
	auto size = Director::getInstance()->getVisibleSize();

	SimpleAudioEngine::getInstance()->playBackgroundMusic("Game/normal.wav");

	// now_quz[0][0] is selected at the first time 
	preSelected.x = 0;
	preSelected.y = 0;
	selected.x = 0;
	selected.y = 0;
	count = 0;

	//initialize time
	time = 0;


	//initialize timebg and countbg
	bg_time = Sprite::create("Game/bg_time.png");
	bg_count =Sprite::create("Game/bg_count.png");
	bg_time->setPosition(Vec2(105.5,115.5));
	bg_count->setPosition(Vec2(105.5,222));
	this->addChild(bg_time,1);
	this->addChild(bg_count, 1);

	time1 = Sprite::create();
	time1->setPosition(Vec2(160, 120));
	this->addChild(time1, 3);

	time2 = Sprite::create();
	time2->setPosition(Vec2(135, 120));
	this->addChild(time2, 3);

	time3 = Sprite::create();
	time3->setPosition(Vec2(100, 120));
	this->addChild(time3, 3);

	//initialize count numbers
	count_ge = Sprite::create();
	count_ge->setPosition(Vec2(105.5 + 15, 222));
	this->addChild(count_ge,3);

	count_shi = Sprite::create();
	count_shi->setPosition(Vec2(105.5 - 15, 222));
	this->addChild(count_shi,3);

	//initialize bgimage 
	bgImage = Sprite::create("Game/bgImageWithQuz.png");
	bgImage->setPosition(Vec2(size.width/2,size.height/2));
	this->addChild(bgImage,0);

	basePuzzle = new BasePuzzle();

	//get quzzle and answer from BasePuzzle
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			now_puz[i][j] = basePuzzle->arr_puz[i][j];
			now_ans[i][j] = basePuzzle->arr_ans[i][j];
		}
	}

	//initialize fillin、clean、delete 、pause and submit buttons
	auto bt_1 = MenuItemImage::create("Game/bt_1.png","Game/bt_1.png",this,menu_selector(GameScene::fillInCallBack));
	bt_1->setTag(1);
	bt_1->setPosition(Vec2(248.5,221.5));

	auto bt_2 = MenuItemImage::create("Game/bt_2.png", "Game/bt_2.png", this, menu_selector(GameScene::fillInCallBack));
	bt_2->setTag(2);
	bt_2->setPosition(Vec2(354.5,221.5));

	auto bt_3 = MenuItemImage::create("Game/bt_3.png", "Game/bt_3.png", this, menu_selector(GameScene::fillInCallBack));
	bt_3->setTag(3);
	bt_3->setPosition(Vec2(248.5,115.5));

	auto bt_4 = MenuItemImage::create("Game/bt_4.png", "Game/bt_4.png" , this, menu_selector(GameScene::fillInCallBack));
	bt_4->setTag(4);
	bt_4->setPosition(Vec2(354.5,116.5));

	auto bt_delete = MenuItemImage::create("Game/bt_delete.png","Game/bt_delete.png",this,menu_selector(GameScene::deleteCallBack));
	bt_delete->setPosition(Vec2(460.5,221.5));
	bt_delete->setTag(5);

	auto bt_clean = MenuItemImage::create("Game/bt_clean.png","Game/bt_clean.png",this,menu_selector(GameScene::cleanCallBack));
	bt_clean->setPosition(Vec2(460.5,116.5));
	bt_clean->setTag(6);

	auto bt_submit = MenuItemImage::create("Game/bt_submit1.png", "Game/bt_submit2.png", this, menu_selector(GameScene::checkCallBack));
	bt_submit->setPosition(Vec2(389,854));

	auto bt_pause = MenuItemImage::create("Game/bt_pause1.png", "Game/bt_pause2.png", this, menu_selector(GameScene::pauseCallBack));
	bt_pause->setPosition(Vec2(152.5, 854));

	auto menu = Menu::create(bt_1, bt_2, bt_3, bt_4,bt_clean,bt_delete,bt_pause,bt_submit,NULL);
	menu->setPosition(Point::ZERO);
	
	this->addChild(menu,2);

	//set position for puz

	int x = 87;
	int y = 724;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				now_puz[i][j]->sprite->setPosition(Vec2(x,y));
				now_puz[i][j]->sprite->setVisible(true);
				this->addChild(now_puz[i][j]->sprite,3);
				x += 122;
			}
			x = 87;
			y -= 121;
	}
	
	//initialize listener to listen SuKudo event
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = [=](Touch* touch, Event* event) {

		Vec2 touchPoint = touch->getLocation();
		bool withinCube = false;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				Rect cube = Rect(now_puz[i][j]->sprite->getPositionX() - 53, now_puz[i][j]->sprite->getPositionY() - 53, 106, 106);
				//if touchPoint is within Rect(not undo) then change selected value and cube's values
				if (cube.containsPoint(touchPoint)) {
					if (now_puz[i][j]->type == TYPE_UNDO) {
						break;
					}
					else {
						preSelected.x = selected.x;
						preSelected.y = selected.y;
						selected.x = i;
						selected.y = j;
						switch (now_puz[i][j]->value) {
						case 0:
							now_puz[i][j]->change(0, TYPE_SELECTED, "BaseCube/cube_selected_0.png");
							withinCube = true;
							break;
						case 1:
							now_puz[i][j]->change(1, TYPE_SELECTED, "BaseCube/cube_selected_1.png");
							withinCube = true;
							break;
						case 2:
							now_puz[i][j]->change(2, TYPE_SELECTED, "BaseCube/cube_selected_2.png");
							withinCube = true;
							break;
						case 3:
							now_puz[i][j]->change(3, TYPE_SELECTED, "BaseCube/cube_selected_3.png");
							withinCube = true;
							break;
						case 4:
							now_puz[i][j]->change(4, TYPE_SELECTED, "BaseCube/cube_selected_4.png");
							withinCube = true;
						}
						setDefalut();
					}
				}
			}
		}
		return withinCube;
	};



	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	schedule(CC_SCHEDULE_SELECTOR(GameScene::update), 1.0);

	return true;
}

void GameScene::fillInCallBack(Ref* pSender)
{
	MenuItem * clickedItem = (MenuItem*)pSender;
	auto *st = ScaleTo::create(0.05f, 0.9f);
	auto *st2 = ScaleTo::create(0.1f, 1.0f);
	Sequence *sq = Sequence::create(st, st2, NULL);
	clickedItem->runAction(sq);

	SimpleAudioEngine::getInstance()->playEffect("res/GameWin/music/click.wav");

	int tag = (static_cast<MenuItemImage*>(pSender))->getTag();
	
	//if something was selected then change paper according to  tag
	//set nothing is selected after the change
	if (tag == 1) {
		now_puz[selected.x][selected.y]->sprite->setVisible(false);
		now_puz[selected.x][selected.y]->change(1, TYPE_FILLED, "BaseCube/cube_filled_1");
		now_puz[selected.x][selected.y]->sprite->setVisible(true);
	}

	else if (tag == 2) {
		now_puz[selected.x][selected.y]->sprite->setVisible(false);
		now_puz[selected.x][selected.y]->change(2, TYPE_FILLED, "BaseCube/cube_filled_2");
		now_puz[selected.x][selected.y]->sprite->setVisible(true);
	}

	else if (tag == 3) {
		now_puz[selected.x][selected.y]->sprite->setVisible(false);
		now_puz[selected.x][selected.y]->change(3, TYPE_FILLED, "BaseCube/cube_filled_3");
		now_puz[selected.x][selected.y]->sprite->setVisible(true);
	}
	
	else {
		now_puz[selected.x][selected.y]->sprite->setVisible(false);
		now_puz[selected.x][selected.y]->change(4, TYPE_FILLED, "BaseCube/cube_filled_4");
		now_puz[selected.x][selected.y]->sprite->setVisible(true);
	}
	
	
	/*selected.x = 0;
	selected.y = 0;*/
	count++;

	static_cast<MenuItemImage*>(pSender)->setScale(1.00f);
}

void GameScene::deleteCallBack(Ref* pSender)
{
	SimpleAudioEngine::getInstance()->playEffect("res/GameWin/music/click.wav");

	
	//static_cast<MenuItemImage*>(pSender)->setScale(1.05f);
	/*(this->getChildByTag(i))->setScale(0.5f, 1.05f);
	(this->getChildByTag(i))->setScale(0.5f, 1.00f);*/

	MenuItem * clickedItem = (MenuItem*)pSender;
	auto *st = ScaleTo::create(0.05f, 0.9f);
	auto *st2 = ScaleTo::create(0.1f, 1.0f);
	Sequence *sq = Sequence::create(st, st2, NULL);
	clickedItem->runAction(sq);


	int i = ((MenuItemImage*)pSender)->getTag();
	if (selected.x<0 || selected.y<0) {
		return;
	}

	now_puz[selected.x][selected.y]->change(0, TYPE_BLANK, "BaseCube/cube_blank_0");
	now_puz[selected.x][selected.y]->sprite->setVisible(true);
	count++;
}

void GameScene::cleanCallBack(Ref* pSender)
{
	MenuItem * clickedItem = (MenuItem*)pSender;
	auto *st = ScaleTo::create(0.05f, 0.9f);
	auto *st2 = ScaleTo::create(0.1f, 1.0f);
	Sequence *sq = Sequence::create(st, st2, NULL);
	clickedItem->runAction(sq);

	SimpleAudioEngine::getInstance()->playEffect("res/GameWin/music/click.wav");

	int i = ((MenuItemImage*)pSender)->getTag();
	
	/*(this->getChildByTag(i))->setScale(0.5f, 1.05f);
	(this->getChildByTag(i))->setScale(0.5f, 1.00f);*/

	if (selected.x<0 || selected.y<0) {
		return;
	}

	//load the original quz again to clean the sub quz
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (now_puz[i][j]->type == TYPE_FILLED) {
				now_puz[i][j]->setBaseCube(0, TYPE_BLANK, "Game/cube_blank_0.png");
			}
		}
	}

	selected.x = 0;
	selected.y = 0;
	count++;
	//static_cast<MenuItemImage*>(pSender)->setScale(1.00f);
}

void GameScene::checkCallBack(Ref* pSender)
{

	SimpleAudioEngine::getInstance()->playEffect("res/GameWin/music/click.wav");

	int wrong_count = 0;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (now_puz[i][j]->value != basePuzzle->arr_ans[i][j]->value) {

				wrong_count++;

				int value = now_puz[i][j]->value;
				//if puz and ans are not the same then changed cube to wrong state
				switch (value) {
				case 0:
					now_puz[i][j]->change(0, TYPE_WRONG, "BaseCube/cube_wrong_0.png");
					break;
				case 1:
					now_puz[i][j]->change(1, TYPE_WRONG, "BaseCube/cube_wrong_1.png");
					break;
				case 2:
					now_puz[i][j]->change(2, TYPE_WRONG, "BaseCube/cube_wrong_2.png");
					break;
				case 3:
					now_puz[i][j]->change(3, TYPE_WRONG, "BaseCube/cube_wrong_3.png");
					break;
				case 4:
					now_puz[i][j]->change(4, TYPE_WRONG, "BaseCube/cube_wrong_4.png");
					break;
				}
			}
		}
	}
	selected.x = 0;
	selected.y = 0;

	//添加头文件 
	if (0 == wrong_count) {
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
		Scene* scene = GameWin::createScene(s,ss,m);
		auto* tt = TransitionFade::create(0.4f, scene);
		Director::getInstance()->replaceScene(tt);
		
	}
}

void GameScene::pauseCallBack(Ref* pSender)
{

}

void GameScene::setDefalut()
{
	int v = now_puz[preSelected.x][preSelected.y]->value;
	switch (v) {
	case 0:
		now_puz[preSelected.x][preSelected.y]->change(0, TYPE_BLANK, "BaseCube/cube_blank_0.png");
		break;
	case 1:
		now_puz[preSelected.x][preSelected.y]->change(1, TYPE_FILLED, "BaseCube/cube_filled_1.png");
		break;
	case 2:
		now_puz[preSelected.x][preSelected.y]->change(2, TYPE_FILLED, "BaseCube/cube_filled_2.png");
		break;
	case 3:
		now_puz[preSelected.x][preSelected.y]->change(3, TYPE_FILLED, "BaseCube/cube_filled_3.png");
		break;
	case 4:
		now_puz[preSelected.x][preSelected.y]->change(4, TYPE_WRONG, "BaseCube/cube_filled_4.png");
		break;
	}
	now_puz[preSelected.x][preSelected.y]->sprite->setVisible(true);
}

void GameScene::update(float dt)
{
		time += 1;
	if (time == 60) {
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
		SimpleAudioEngine::getInstance()->playBackgroundMusic("Game/normal.wav");
	}

	if (time == 120) {
		Scene* scene = GameLose::createScene();
		auto* tt = TransitionFade::create(0.4f, scene);
		Director::getInstance()->replaceScene(tt);
		}

		s = time % 10;
		String* str1 = String::createWithFormat("res/GameWin/num%d.png", s);
		time1->setTexture(str1->getCString());

		 ss = (time - s) / 10;

		if (ss >= 6) {
			ss -= 6;
			m += 1;
		}
		String* str2 = String::createWithFormat("res/GameWin/num%d.png", ss);
		time2->setTexture(str2->getCString());

		String* str3= String::createWithFormat("res/GameWin/num%d.png", m);
		time3->setTexture(str3->getCString());
		

		if (count < 10) {
			String* str4 = String::createWithFormat("res/GameWin/num%d.png", count);
			count_ge->setTexture(str4->getCString());
		}
		

		if (count >= 10 && count < 100) {
			
			String* str5 = String::createWithFormat("res/GameWin/num%d.png", count % 10);
			count_ge->setTexture(str5->getCString());

			String* str6 = String::createWithFormat("res/GameWin/num%d.png", count / 10);
			count_shi->setTexture(str6->getCString());
		}

		if (count >= 100) {
			SimpleAudioEngine::getInstance()->stopBackgroundMusic();
			Scene* scene = GameWin::createScene(s, ss, m);
			auto* tt = TransitionFade::create(0.4f, scene);
			Director::getInstance()->replaceScene(tt);
		}


}
