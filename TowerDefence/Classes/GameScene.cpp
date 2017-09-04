#include<fstream>
#include<algorithm>
#include "json/rapidjson.h"  
#include "json/document.h" 
#include"GameScene.h"
#include"Monster.h"
#include"Tower.h"
#include"Bullet.h"

USING_NS_CC;
using namespace std;

#define Pi (3.14159)

#define MAP_ZO (0)
#define CUBE_ZO (1)
#define CUBE_SELECTED_ZO (2)
#define MONS_SELECTED_ZO (5)
#define TOWER_ZO (8)
#define MONS1_ZO (3)		//��һ���
#define MONS2_ZO (4)		//�ڶ����
#define MONS3_ZO (5)		//�������
#define MONS4_ZO (6)		//�������
#define BULLET_ZO (7)
#define OPTION_BG_ZO (9) //�˵��㱳��
#define OPTION_ZO (10)		//�˵���
#define BAR_ZO (20)
#define NUM_ZO (21)

#define OPTION_RADIUS (60)

extern int valueTable[][3];

Scene* GameScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool GameScene::init()
{
	if (!Layer::init()) {
		return false;
	}

	level = UserDefault::getInstance()->getIntegerForKey("nowLevel");
	
	//Ԥ���ر�������
	switch (level)
	{
	case 1:
		SimpleAudioEngine::getInstance()->preloadBackgroundMusic("Music/bg_Game_normal.wav");
		break;
	case 2:
	case 5:
		SimpleAudioEngine::getInstance()->preloadBackgroundMusic("Music/bg_ice.mp3");
		break;
	case 3:
	case 4:
		SimpleAudioEngine::getInstance()->preloadBackgroundMusic("Music/bg_fire.mp3");
		break;
	case 6:
		SimpleAudioEngine::getInstance()->preloadBackgroundMusic("Music/bg_light.mp3");
		break;

	default:
		break;
	}


	//���ﱬը��Ч
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("GameScene/blow.plist");


	isGameOver = false;
	isWon = false;
	isShowingOption = false;
	isDoubleSpeed = false;
	isFinalWave = false;

	Director::getInstance()->getScheduler()->setTimeScale(1.0f);

	towerBuilt = 0;
	killNum = 0;

	cubeSelectedPoint = nullptr;
	targetSelected = nullptr;
	towerSelected = nullptr;

	menuItem_T1 = nullptr;
	menuItem_T2 = nullptr;
	menuItem_T3 = nullptr;
	menuItem_Sell = nullptr;
	menuItem_Up = nullptr;

	money = 0;
	wave = 0;

	visibleSize = Director::getInstance()->getVisibleSize();
	visibleOrigin = Director::getInstance()->getVisibleOrigin();


	//��Ƭ��ͼ
	initMap();


	//׼���˵���
	initOptionMenu();


	//��ȡ��ͼ·��
	if (!initPath()) {
		return false;
	}


	//�հ�ѡ��ͼ��
	{
		cubeSelectedSp = Sprite::create("GameScene/cubeSelected.png");
		cubeSelectedSp->setVisible(false);
		cubeSelectedSp->setPosition(Point::ZERO);
		this->addChild(cubeSelectedSp, CUBE_SELECTED_ZO);
	}

	//үү~
	{
		Yeye = Sprite::create("GameScene/Yeye2.png");
		Yeye->setPosition(path.front());
		Yeye->setScaleX(0.8f);
		Yeye->setFlippedX(true);
		this->addChild(Yeye, TOWER_ZO);
	}

	//�Ϸ��˵���
	auto barSp = Sprite::create("GameScene/dock.png");
	barSp->setPosition(Point(550,758));
	this->addChild(barSp, BAR_ZO);

	//��ǮLabel
	{
		auto moneySp = Sprite::create("GameScene/money_icon.png");
		moneySp->setPosition(Point(725, 758));
		this->addChild(moneySp, NUM_ZO);
		moneyLabel = LabelTTF::create();
		moneyLabel->setString(String::createWithFormat("%d", money)->getCString());
		moneyLabel->setFontSize(30);
		moneyLabel->setPosition(Point(775, 758));
		this->addChild(moneyLabel, NUM_ZO);
	}

	//���ﲨ
	{
		nowWave = 1;
		getWaveInfo();
		waveLabel = LabelTTF::create();
		waveLabel->setString(String::createWithFormat("wave %d/%d",nowWave,wave)->getCString());
		waveLabel->setPosition(Point(450, 758));
		waveLabel->setFontSize(23);
		this->addChild(waveLabel, NUM_ZO);
	}


	//��ǰ�ؿ���ʾ
	{
		levelLabel = LabelTTF::create();
		levelLabel->setString(String::createWithFormat("Level %d", level)->getCString());
		levelLabel->setPosition(Point(600, 758));
		levelLabel->setFontSize(23);
		this->addChild(levelLabel, NUM_ZO);
	}


	//������
	{
		heart = 5;
		auto heartSp = Sprite::create("GameScene/heart_icon.png");
		heartSp->setPosition(Point(280,758));
		this->addChild(heartSp, NUM_ZO);
		heartLabel = LabelTTF::create();
		heartLabel->setString(String::createWithFormat("%d",heart)->getCString());
		heartLabel->setPosition(Point(330, 758));
		heartLabel->setFontSize(30);
		this->addChild(heartLabel, NUM_ZO);
	}

	//���㴥������
	auto touchlistener = EventListenerTouchOneByOne::create();
	touchlistener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan,this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchlistener, this);
	
	//��ʱ��
	canCreateMonster = true;
	this->schedule(schedule_selector(GameScene::Myupdate));


	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	//�˴���ȡ�Ƿ�����������

	if (UserDefault::getInstance()->getBoolForKey("isbgMusicOpen")) {
		musicRight = Sprite::create("GameScene/bt_bgMusicOpen.png");
		SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
	}
	else {
		musicRight = Sprite::create("GameScene/bt_bgMusicClose.png");

	}
	musicRight->setPosition(Vec2(1060, 755));
	this->addChild(musicRight, 2);

	speed = Sprite::create("GameScene/speed_icon_1.png");
	speed->setPosition(Vec2(120, 755));
	this->addChild(speed, 2);

	auto speedItem = MenuItemImage::create("GameScene/bt_speed.png", "GameScene/bt_speed.png", CC_CALLBACK_1(GameScene::menuSpeedCallback, this));
	speedItem->setPosition(Vec2(120, 755));

	auto pauseItem = MenuItemImage::create("GameScene/bt_pause.png", "GameScene/bt_pause.png", CC_CALLBACK_1(GameScene::menuPauseCallback,this));
	pauseItem->setPosition(Vec2(944, 755));
	auto musicItem = MenuItemImage::create("GameScene/bt_bgMusic.png", "GameScene/bt_bgMusic.png", CC_CALLBACK_1(GameScene::menuMusicCallback, this));
	musicItem->setPosition(Vec2(1032, 755));

	
	//��ʱ��Ӽ�ֱ�ӵ�����������İ�ť
	
	auto winItem = MenuItemImage::create("GameScene/bt_win.png", "GameScene/bt_win.png", CC_CALLBACK_1(GameScene::menuWinCallback, this));
	winItem->setPosition(Vec2(50,400));
	auto failItem = MenuItemImage::create("GameScene/bt_fail.png", "GameScene/bt_fail.png", CC_CALLBACK_1(GameScene::menuFailCallback, this));
	failItem->setPosition(Vec2(50, 320));

	//winItem->setVisible(false);		winItem->setEnabled(false);
	//failItem->setVisible(false);		failItem->setEnabled(false);

	auto menu = Menu::create(speedItem, pauseItem, musicItem, winItem, failItem, NULL);
	menu->setPosition(Point::ZERO);
	this->addChild(menu, BAR_ZO);

	

	
	this->countDown();
	auto delay = DelayTime::create(4.0f);
	this->runAction(Sequence::create(delay, CallFuncN::create(CC_CALLBACK_0(GameScene::startGame, this)), NULL));

	return true;
}

void GameScene::initMap()
{
	auto tmxMap = TMXTiledMap::create(String::createWithFormat("GameScene/Map/Level_%d.tmx",level)->getCString());
	tmxMap->setPosition(visibleOrigin);
	this->addChild(tmxMap, MAP_ZO);

	blankLayer = tmxMap->getLayer("blank");

	//��ͼ��Ϊ11x8��ʽ
	for (int i = 0; i < 11; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (blankLayer->getTileAt(Point(i, j)) != nullptr)
			{
				auto sp = blankLayer->getTileAt(Point(i, j));
				blankVec.pushBack(sp);
			}
		}
	}
}

void GameScene::getWaveInfo()
{
	ifstream in(String::createWithFormat("GameScene/Info/Levelinfo_%d.txt", level)->getCString());
	
	if (in.is_open())
	{
		//��һ�У�������
		string temp;
		getline(in, temp);
		waveInfo.push_back(temp);
		wave = atoi(temp.c_str());


		//�ڶ��У���Ǯ��
		string temp2;
		getline(in,temp2);
		money = atoi(temp2.c_str());
		moneyLabel->setString(String::createWithFormat("%d", money)->getCString());


		//�����У�ÿ��������Ϣ��
		while (!in.eof())
		{
			string temp3;
			getline(in,temp3);
			waveInfo.push_back(temp3);
		}
	}
}

bool GameScene::initPath()
{
	string filename = String::createWithFormat("GameScene/Info/path_%d.json", level)->getCString();
	rapidjson::Document doc;

	//�ж��ļ��Ƿ����  
	if (!FileUtils::getInstance()->isFileExist(filename))
	{
		CCLOG("json file is not find [%s]", filename);
		return false;
	}

	std::string data = FileUtils::getInstance()->getStringFromFile(filename);
	doc.Parse<rapidjson::kParseDefaultFlags>(data.c_str());

	//�ж��Ƿ��ȡ�ɹ�  �Ƿ�Ϊ��������  
	if (doc.HasParseError() || !doc.IsArray())
	{
		CCLOG("get json data err!");
		return false;
	}

	//�ڶ��п�ʼ����һ�������� X,Y 
	for (unsigned int i = 1; i<doc.Size(); i++)
	{
		rapidjson::Value &v = doc[i];
		path.push_back(Point(v[0].GetInt(), v[1].GetInt()));
	}

	return true;
}

void GameScene::initOptionMenu()
{
	isShowingOption = false;

	menu = Menu::create();

	menu->setName("menu");

	menu->setSwallowsTouches(true);

	menu->setPosition(Point::ZERO);
	
	this->addChild(menu, OPTION_ZO);
}

void GameScene::monsterCreate(float)
{
	if (!waveInfo.at

	(nowWave).empty())
	{
		int id = waveInfo.at

		(nowWave).back() - '0';

		auto m = Monster::create(id, path.size() - 1);

		m->setPosition(path.back());

		monsterList.push_back(m);

		if (m->zo == 1)
			addChild(m, MONS1_ZO);
		else if (m->zo == 2)
			addChild(m, MONS2_ZO);
		else if (m->zo == 3)
			addChild(m, MONS3_ZO);
		else
			addChild(m, MONS4_ZO);

		//��������Ч
		auto in = FadeIn::create(0.3f);
		auto out = FadeOut::create(0.8f);

		auto blow = Blow::create();
		blow->setTexture("GameScene/door.png");
		blow->setOpacity(0);
		blow->setScale(0.8f);
		blow->setPosition(path.back().x - 30, path.back().y);
		addChild(blow, CUBE_SELECTED_ZO);

		blow->runAction(Sequence::create(in, out, CallFuncN::create(CC_CALLBACK_0(Blow::stopBlow, blow)), NULL));

		waveInfo.at

		(nowWave).pop_back();
	}
	else
	{
		canCreateMonster = false;
	}
}

void GameScene::monsterMove()
{
	auto iter  = monsterList.begin(); 

	while(iter != monsterList.end())
	{
		if (((*iter)->way == 2))//�й����ƶ����յ�
		{
			if (--heart > 0)
			{
				heartLabel->setString(String::createWithFormat("%d", heart)->getCString());

				if (*iter == targetSelected)
				{
					targetSelected = nullptr;
				}

				//�������Ըù�������target��Ҫ��Ϊnullptr
				for (auto t : towerList)
				{
					if (t->target == *iter)
					{
						t->target = nullptr;
					}
				}

				(*iter)->removeFromParentAndCleanup(true);
				monsterList.erase(iter);
				iter = monsterList.begin();
			}
			else//����������0
			{
				if (towerBuilt == 0) {
					UserDefault::getInstance()->setBoolForKey("specialAchieve1", true);
					UserDefault::getInstance()->flush();
				}
				Director::getInstance()->getScheduler()->setTimeScale(1.0f);
				MySceneManager::myGetInstance()->toEndMenu(this);
				this->pauseSchedulerAndActions();
				break;
			}
		}
		else
		{
			if (!(*iter)->isMoving)
			(*iter)->move(path.at((*iter)->way));
			iter++;
		}
	}
}

void GameScene::showOptionInBlank()
{
	cubeSelectedSp->setPosition(*cubeSelectedPoint);
	cubeSelectedSp->setVisible(true);

	MenuItemImage* item1;
	MenuItemImage* item2;
	MenuItemImage* item3;
	

	//���ݽ�Ǯ�Ƿ��㹻�������˵���
	if (money >= valueTable[0][0])
	{
		item1 = MenuItemImage::create(
			"GameScene/Option/tower1_can_build.png", 
			"GameScene/Option/tower1_can_build.png",
			this, 
			menu_selector(GameScene::createTowerCallBack)
		);
	}
	else
	{
		item1 = MenuItemImage::create(
			"GameScene/Option/tower1_not_build.png", 
			"GameScene/Option/tower1_not_build.png",
			this,
			menu_selector(GameScene::createTowerCallBack)
		);
		item1->setEnabled(false);
	}

	if (money >= valueTable[0][1])
	{
		item2 = MenuItemImage::create(
			"GameScene/Option/tower2_can_build.png", 
			"GameScene/Option/tower2_can_build.png",
			this,
			menu_selector(GameScene::createTowerCallBack)
		);
	}
	else
	{
		item2 = MenuItemImage::create(
			"GameScene/Option/tower2_not_build.png", 
			"GameScene/Option/tower2_not_build.png", 
			this,
			menu_selector(GameScene::createTowerCallBack)
		);
		item2->setEnabled(false);
	}

	if (money >= valueTable[0][2])
	{
		item3 = MenuItemImage::create(
			"GameScene/Option/tower3_can_build.png",
			"GameScene/Option/tower3_can_build.png",
			this,
			menu_selector(GameScene::createTowerCallBack)
		);
	}
	else
	{
		item3 = MenuItemImage::create(
			"GameScene/Option/tower3_not_build.png",
			"GameScene/Option/tower3_not_build.png", 
			this,
			menu_selector(GameScene::createTowerCallBack)
		);
		item3->setEnabled(false);
	}

	item1->setTag(1);
	item2->setTag(2);
	item3->setTag(3);

	item1->setName("tower1");
	item2->setName("tower2");
	item3->setName("tower3");

	menuItem_T1 = item1;
	menuItem_T2 = item2;
	menuItem_T3 = item3;

	item1->setAnchorPoint(Point(0.5, 0.5));
	item2->setAnchorPoint(Point(0.5, 0.5));
	item3->setAnchorPoint(Point(0.5, 0.5));

	item1->setPosition(
		Point(cubeSelectedPoint->x - 50,
			cubeSelectedPoint->y-50-item1->getContentSize().height/2)
	);
	item2->setPosition(
		Point(cubeSelectedPoint->x,
			cubeSelectedPoint->y - 50 - item1->getContentSize().height / 2)
	);
	item3->setPosition(
		Point(cubeSelectedPoint->x + 50,
			cubeSelectedPoint->y - 50 - item3->getContentSize().height / 2)
	);

	menu->addChild(item1);
	menu->addChild(item2);
	menu->addChild(item3);


	//todo
	item1->setScale(1.2f);
	item2->setScale(1.2f);
	item3->setScale(1.2f);

}

void GameScene::showOptionWithTower()
{
	//Բ�α���
	auto circle = Sprite::create("GameScene/circle.png");
	circle->setName("circle");
	circle->setPosition(towerSelected->getPosition());
	circle->setScale(2*towerSelected->range / circle->getContentSize().width);
	this->addChild(circle,OPTION_BG_ZO);

	MenuItemImage* upgradeItem;
	MenuItemImage* soldItem;

	int id = towerSelected->id;
	int rank = towerSelected->rank;

	if (!towerSelected->isMax)//����������ʱ
	{
		String* upgradeImg;

		if (money >= towerSelected->upValue)//��Ǯ����
		{
			upgradeImg = String::createWithFormat("GameScene/Option/tower%d_can_toR%d.png", id, rank+1);

			upgradeItem = MenuItemImage::create(
				upgradeImg->getCString(),
				upgradeImg->getCString(),
				CC_CALLBACK_1(GameScene::upgradeTowerCallBack, this)
			);

		}
		else//����Ǯ����
		{
			upgradeImg = String::createWithFormat("GameScene/Option/tower%d_not_toR%d.png", id, rank+1);

			upgradeItem = MenuItemImage::create(
				upgradeImg->getCString(),
				upgradeImg->getCString(),
				CC_CALLBACK_1(GameScene::upgradeTowerCallBack, this)
			);

			upgradeItem->setEnabled(false);
		}

		soldItem = MenuItemImage::create(
			"GameScene/Option/tower_sold.png",
			"GameScene/Option/tower_sold.png",
			CC_CALLBACK_1(GameScene::upgradeTowerCallBack, this)
		);

		upgradeItem->setTag(1);
		soldItem->setTag(2);

		menuItem_Up = upgradeItem;
		menuItem_Sell = soldItem;

		upgradeItem->setPosition(Point(towerSelected->getPositionX()-40, towerSelected->getPositionY()));
		soldItem->setPosition(Point(towerSelected->getPositionX() + 40, towerSelected->getPositionY()));

		menu->addChild(upgradeItem);
		menu->addChild(soldItem);
	}
	else//������ʱ
	{
		soldItem = MenuItemImage::create(
			"GameScene/Option/tower_sold.png",
			"GameScene/Option/tower_sold.png",
			CC_CALLBACK_1(GameScene::upgradeTowerCallBack, this)
		);

		soldItem->setPosition(Point(towerSelected->getPositionX(), towerSelected->getPositionY() + 50));
		
		soldItem->setScale(1.2f);
		soldItem->setTag(2);
		menuItem_Sell = soldItem;

		menu->addChild(soldItem);
	}
}

bool GameScene::onTouchBegan(Touch* touch, Event* e)
{
	cubeSelectedSp->setVisible(false);

	Point touchPoint = touch->getLocation();

	if (!isShowingOption)//û����ʾ�˵�
	{
		//�ȼ���Ƿ�㵽����
		for (auto iter : monsterList)
		{
			auto rect = Rect(
				Point((*iter).getPositionX() - 50, ((*iter).getPositionY() - 50)),
				Size(100, 100)
			);
			if (rect.containsPoint(touchPoint))
			{
				//����Ŀ��
				if (targetSelected != nullptr)
				{
					targetSelected->icon->setVisible(false);
				}

				targetSelected = iter;
			}
		}


		//�ټ�������Ƿ�����
		for (auto iter : towerList)
		{
			auto rect = Rect(
				Point((*iter).getPositionX() - 50, ((*iter).getPositionY() - 50)),
				Size(100, 100)
			);
			if (rect.containsPoint(touchPoint))
			{
				towerSelected = iter;
				isShowingOption = true;
				showOptionWithTower();
				return true;
			}
		}

			//�����������Ƿ��ǿհ�
			int count = blankVec.size();

			for (int i = 0; i < count; i++)
			{
				auto sp = blankVec.at(i);
				auto cubePoint = Point(sp->getPositionX() + 50, sp->getPositionY() + 50);
				auto rect = Rect(sp->getPosition(), Size(100, 100));
				if (rect.containsPoint(touchPoint))
				{
					cubeSelectedPoint = new Point(cubePoint);

					isShowingOption = true;
					showOptionInBlank();
				}
			}
		return true;
	}
	else//������ʾ�˵�
	{
		Point point;
		if (towerSelected != nullptr)
			point = towerSelected->getPosition();
		else
			point = *cubeSelectedPoint;


		//������ʾ�˵�ʱ�㵽�����
		for (auto iter : towerList)
		{
			auto rect = Rect(
				Point((*iter).getPositionX() - 50, ((*iter).getPositionY() - 50)),
				Size(100, 100)
			);
			if (rect.containsPoint(touchPoint))
			{
				towerSelected = iter;
				isShowingOption = true;
				menu->removeAllChildrenWithCleanup(true);

				cleanOption();

				showOptionWithTower();
				return true;
			}
		}

		//������ʾ�˵�ʱ�㵽������
		int count = blankVec.size();
		for (int i = 0; i < count; i++)
		{
			auto sp = blankVec.at(i);
			auto cubePoint = Point(sp->getPositionX() + 50, sp->getPositionY() + 50);

			auto rect = Rect(sp->getPosition(), Size(100, 100));
			if (rect.containsPoint(touchPoint))
			{
				cubeSelectedPoint = new Point(cubePoint);

				cleanOption();

				isShowingOption = true;
				showOptionInBlank();
				return true;
			}
		}


		int x = abs(point.x - touchPoint.x);
		int y = abs(point.y - touchPoint.y);

		if (sqrt(x*x + y*y) >= OPTION_RADIUS)//�㵽�˵�����հ״�
		{
			cubeSelectedSp->setVisible(false);

			isShowingOption = false;

			cleanOption();
		}
		return true;
	}
}

void GameScene::createTowerCallBack(cocos2d::Ref* ref)
{
	int id = (static_cast<Node*>(ref))->getTag();
	
	Tower* tower = Tower::create(id);

	money -= tower->value;
	moneyLabel->setString(String::createWithFormat("%d",money)->getCString());

	if (cubeSelectedPoint != nullptr)
	{
		tower->setPosition(*cubeSelectedPoint);
	}

	this->towerList.push_back(tower);
	this->addChild(tower, TOWER_ZO);

	//���Ž�����Ч
	SimpleAudioEngine::getInstance()->playEffect("Music/click_buy.wav");
	towerBuilt++;
	UserDefault::getInstance()->setIntegerForKey("allTowerNum", 1 + UserDefault::getInstance()->getIntegerForKey("allTowerNum", 0));
	UserDefault::getInstance()->flush();
	
	delete cubeSelectedPoint;
	cubeSelectedPoint = nullptr;

	isShowingOption = false;

	cubeSelectedSp->setVisible(false);

	cleanOption();
}

void GameScene::upgradeTowerCallBack(Ref* ref)
{
	int tag = (static_cast<Node*>(ref))->getTag();

	//���������
	if (tag == 1)
	{
		towerSelected->upgrade();

		money -= towerSelected->value;
		moneyLabel->setString(String::createWithFormat("%d", money)->getCString());
	}
	else
	{
		money += towerSelected->sellValue;
		moneyLabel->setString(String::createWithFormat("%d", money)->getCString());

		towerList.remove(towerSelected);
		towerSelected->removeFromParentAndCleanup(true);
	}
	//�����Ч
	SimpleAudioEngine::getInstance()->playEffect("Music/click_buy.wav");


	towerSelected = nullptr;
	isShowingOption = false;

	cleanOption();
}

void GameScene::towerAttack(Tower* tower)
{
	//������Ч
	SimpleAudioEngine::getInstance()->playEffect(String::createWithFormat("Music/tower%d_attack.wav", tower->id)->getCString());


	Monster* target;

	//ȷ���Ƿ���Ҫ���ȹ�����ѡ�е�Ŀ��
	if (targetSelected != nullptr &&
		(targetSelected->getPosition() - tower->getPosition()).length() <= (float)tower->range)
	{
		target = targetSelected;
	}
	else if (tower->target != nullptr)
	{
		target = tower->target;
	}
	else
	{
		return;
	}


		if(target != nullptr)
		{
			//�����ӵ�
			auto bullet = Bullet::create(tower);
			bullet->setPosition(tower->getPosition());
			this->bulletList.push_back(bullet);
			this->addChild(bullet, BULLET_ZO);

			//�����λ�ã�Ԥ�У�
			Point nextPoint;

			switch (target->dir)
			{
			case UP:
				nextPoint = Point(target->getPositionX(), target->getPositionY() + 40);
				break;
			case DOWN:
				nextPoint = Point(target->getPositionX(), target->getPositionY() - 40);
				break;
			case RIGHT:
				nextPoint = Point(target->getPositionX() + 40, target->getPositionY());
				break;
			case LEFT:
				nextPoint = Point(target->getPositionX() - 40, target->getPositionY());
				break;
			}

			//�����������
			Point shootVector = tower->target->getPosition() - tower->getPosition();
			//������׼��(����������Ϊ1)
			Point normalizedVector = ccpNormalize(shootVector);
			//�����ת�Ļ���
			float radians = atan2(normalizedVector.y, -normalizedVector.x);
			//������ת���ɽǶ�
			float degree = CC_RADIANS_TO_DEGREES(radians);

			float moveTime = shootVector.getLength() / (bullet->speed);

			auto move = MoveTo::create(moveTime, nextPoint);//���ж���

			auto rotateAnimate = RotateTo::create(0.01f, degree - 90);//�����Ƕȶ���

			//�ӵ�����ʱ�л�ͼƬ����
			bullet->runAction(RepeatForever::create(Animate::create(Animation::createWithSpriteFrames(bullet->flyFrames,0.1f))));

			
			//�ӵ��ƶ�
			auto blow = Animate::create(Animation::createWithSpriteFrames(bullet->blowFrames, 0.1f, 1));

			auto seq = Sequence::create(move, blow, CallFuncN::create(CC_CALLBACK_0(Bullet::stopFlying, bullet)), NULL);

			bullet->runAction(seq);




			tower->fire();
			this->runAction(Sequence::create(
				DelayTime::create(tower->delay),
				CallFuncN::create(CC_CALLBACK_0(GameScene::towerReload, this, tower)),
				NULL));
		}
}

void GameScene::towerReload(Tower* tower)
{
	tower->reload();
}

void GameScene::cleanOption()
{
	if (this->getChildByName("circle") != NULL)
	{
		this->getChildByName("circle")->removeFromParentAndCleanup(true);
	}

	menu->removeAllChildrenWithCleanup(true);

	menuItem_T1 = nullptr;
	menuItem_T2 = nullptr;
	menuItem_T3 = nullptr;
	menuItem_Sell = nullptr;
	menuItem_Up = nullptr;
}

void GameScene::Myupdate(float dt)
{
	if (!isGameOver)
	{

		//����״̬λ�����Ƿ�ˢ�֣�ˢ��ǰ�������ӳ٣�
		if (canCreateMonster && !this->isScheduled(schedule_selector(GameScene::monsterCreate)))
		{
			if (nowWave == wave && !isFinalWave)
			{
				isFinalWave = true;
				finalWaveTip();
			}
			this->schedule(
				schedule_selector(GameScene::monsterCreate),
				2.0f,
				CC_REPEAT_FOREVER,
				3.0f);
		}

		if (!canCreateMonster && this->isScheduled(schedule_selector(GameScene::monsterCreate)))
			this->unschedule(schedule_selector(GameScene::monsterCreate));


		//�����ƶ�
		if (!monsterList.empty())
		{
			monsterMove();
		}
		else if (monsterList.empty() && nowWave < wave && !canCreateMonster)	//��������
		{
			canCreateMonster = true;
			nowWave++;
			waveLabel->setString(String::createWithFormat("wave %d/%d", nowWave, wave)->getCString());
		}



		for (auto iter : towerList)
		{
			//������Ƿ������
			if (iter->isMax)
				iter->icon->setVisible(false);

			if (money >= iter->upValue && !iter->isMax)
				iter->showIcon();

			if ((money < iter->upValue) && (iter->icon->isVisible()) && (iter->upValue > 0))
				iter->icon->setVisible(false);



			//δû�й���Ŀ���������Ŀ��
			for (auto mons : monsterList)
			{
				if ((iter->getPosition() - mons->getPosition()).length() <= iter->range)//���ڷ�Χ��
				{
					if (iter->target == nullptr)
					{
						iter->target = mons;
					}
				}

				if ((iter->getPosition() - mons->getPosition()).length() > iter->range)//���ܳ���Χ���޳�
				{
					if (iter->target == mons)
					{
						iter->target = nullptr;
					}
				}

			}


			//������Ƿ���Թ���
			if ((iter->target != nullptr) && !iter->isFiring)
			{
				towerAttack(iter);
			}
		}



		//��ʾ��ѡ�еĹ���
		if (targetSelected != nullptr && !(targetSelected->icon->isVisible()))
		{
			targetSelected->icon->setVisible(true);
		}


		//�Ƴ���ը��Ч
		auto blow = blowList.begin();
		while (blow != blowList.end())
		{
			if (!((*blow)->isBlowing))
			{
				auto temp = *blow;
				blowList.erase(blow);
				temp->removeFromParentAndCleanup(true);
				blow = blowList.begin();
			}
			else
			{
				++blow;
			}
		}


		//�����Ƴ����ڷ��е��ӵ�
		auto iter = bulletList.begin();
		while (iter != bulletList.end())
		{
				if (!((*iter)->isFlying))
				{
					auto temp = *iter;
					bulletList.erase(iter);
					temp->removeFromParentAndCleanup(true);
					temp = nullptr;
					iter = bulletList.begin();
				}
				else
				{
					++iter;
				}
		}



		//�ټ������е��ӵ��ӵ��Ƿ���й���
		auto bullet = bulletList.begin();
		while (bullet != bulletList.end())
		{
			if (!monsterList.empty())
			{
				auto mons = monsterList.begin();
				while (mons != monsterList.end())
				{
						Rect rect = Rect(
							(*mons)->getPositionX() - 50,
							(*mons)->getPositionY() - 50,
							(*mons)->getContentSize().width,
							(*mons)->getContentSize().height
						);

						if ((!(*bullet)->isWithin) &&rect.containsPoint((*bullet)->getPosition()))
						{
							//�����ܻ� 
							(*mons)->blood -= (*bullet)->attack;

							auto blow = Animate::create(Animation::createWithSpriteFrames((*bullet)->blowFrames, 0.1f, 1));
							(*bullet)->runAction(Sequence::create(blow, CallFuncN::create(CC_CALLBACK_0(Bullet::stopFlying, *bullet)), NULL));
							(*bullet)->isWithin = true;



							if ((*mons)->blood > 0)//�ܻ�����
							{
								(*mons)->timer->setPercentage((*mons)->blood / (*mons)->maxblood * 100);
								
								//ÿ������ֻ�ܱ��ֳ�һ���ܻ�Ч��
								if (!(*mons)->isGettingHurt)
								{
									//�ӵ�2����Ч����ȼ�գ�
									if ((*bullet)->id == 2)
									{
										(*mons)->isGettingHurt = true;

										//�𽥵�Ѫ
										auto seq = Sequence::create(
											DelayTime::create(1.0f),
											CallFuncN::create(CC_CALLBACK_0(Monster::cutBlood,(*mons))),
											NULL);
										
										auto cutBlood = Repeat::create(seq, (*bullet)->duration);

										(*mons)->runAction(Sequence::create(
											cutBlood,
											CallFuncN::create(CC_CALLBACK_0(Monster::resumeFromB2, (*mons))),
											NULL
											));
											
										//ȼ��
										auto burn = Animate::create(Animation::createWithSpriteFrames((*bullet)->effectFrames,0.25f,(*bullet)->duration*2));

										Effect* effect = Effect::create();
										effect->setTexture(SpriteFrameCache::getInstance()->getSpriteFrameByName("bullet2_5.png")->getTexture());
										effect->setPosition(40, 20);
										(*mons)->addChild(effect,100);

										effect->runAction(Sequence::create(
											burn,
											CallFuncN::create(CC_CALLBACK_0(Effect::stopUsing,effect)),
											NULL)
										);
									}

									//�ӵ�3����Ч�������٣�
									if ((*bullet)->id == 3)
									{
										(*mons)->isGettingHurt = true;

										//ˮ��
										auto water = Animate::create(Animation::createWithSpriteFrames((*bullet)->effectFrames, 0.25f, (*bullet)->duration * 2));

										Effect* effect = Effect::create();
										effect->setTexture(SpriteFrameCache::getInstance()->getSpriteFrameByName("bullet3_5.png")->getTexture());
										effect->setPosition(40, 40);
										(*mons)->addChild(effect,100);

										effect->runAction(Sequence::create(
											water,
											CallFuncN::create(CC_CALLBACK_0(Effect::stopUsing, effect)),
											NULL)
										);

										//����
										(*mons)->runAction(Sequence::create(
											CallFuncN::create(CC_CALLBACK_0(Monster::slowDown, *mons,(*bullet)->tower->rank)),
											DelayTime::create((*bullet)->duration),
											CallFuncN::create(CC_CALLBACK_0(Monster::resumeFromB3, *mons, (*bullet)->tower->rank)),
											NULL)
										);


									}

								}
							}
							else//�ܻ�����
							{

								SimpleAudioEngine::getInstance()->playEffect("Music/dead_1.wav");


								//��ը��Ч
								{
									Vector<SpriteFrame*> flame;
									flame.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("blow1.png"));
									flame.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("blow2.png"));

									auto blow = Blow::create();
									blow->setTexture(flame.front()->getTexture());
									blow->setPosition((*mons)->getPosition());
									this->blowList.push_back(blow);
									this->addChild(blow, TOWER_ZO);

									blow->runAction(Sequence::create(
										Animate::create(Animation::createWithSpriteFrames(flame,0.3f,1)),
										CallFuncN::create(CC_CALLBACK_0(Blow::stopBlow,blow)),
										NULL)
									);
								}


								++killNum;
								UserDefault::getInstance()->setIntegerForKey("allKillNum", 1 + UserDefault::getInstance()->getIntegerForKey("allKillNum", 0));
								UserDefault::getInstance()->flush();

								getMoney((*mons)->getPosition());

								if (money >= ACHIEVE_MONEY) {
									UserDefault::getInstance()->setBoolForKey("specialAchieve2", true);
									UserDefault::getInstance()->flush();
								}

								moneyLabel->setString(String::createWithFormat("%d",money)->getCString());
								if (*mons == targetSelected)
									targetSelected = nullptr;

								//�������Ըù�������target��Ҫ��Ϊnullptr
								for (auto t : towerList)
								{
									if (t->target == *mons)
									{
										t->target = nullptr;
									}
								}

								auto temp = *mons;
								this->monsterList.erase(mons);
								temp->removeFromParentAndCleanup(true);
								temp = nullptr;
								

				
								mons = monsterList.begin();
								continue;
							}
						}
					++mons;
				}
			}
			++bullet;
		}


		//���²˵���
		if (isShowingOption)
		{
			if (menuItem_T1 != nullptr)
			{
				if (money >= valueTable[0][0] && !menuItem_T1->isEnabled())
				{
					menuItem_T1->setNormalImage(Sprite::create("GameScene/Option/tower1_can_build.png"));
					menuItem_T1->setSelectedImage(Sprite::create("GameScene/Option/tower1_can_build.png"));
					menuItem_T1->setEnabled(true);
				}
			}

			if (menuItem_T2 != nullptr)
			{
				if (money >= valueTable[0][1] && !menuItem_T2->isEnabled())
				{
					menuItem_T2->setNormalImage(Sprite::create("GameScene/Option/tower2_can_build.png"));
					menuItem_T2->setSelectedImage(Sprite::create("GameScene/Option/tower2_can_build.png"));
					menuItem_T2->setEnabled(true);
				}
			}

			if (menuItem_T3 != nullptr)
			{
				if (money >= valueTable[0][2] && !menuItem_T3->isEnabled())
				{
					menuItem_T3->setNormalImage(Sprite::create("GameScene/Option/tower3_can_build.png"));
					menuItem_T3->setSelectedImage(Sprite::create("GameScene/Option/tower3_can_build.png"));
					menuItem_T3->setEnabled(true);
				}
			}

			if (menuItem_Up != nullptr)
			{
				if (money >= valueTable[towerSelected->rank][towerSelected->id - 1])
				{
					menuItem_Up->setNormalImage(Sprite::create(String::createWithFormat("GameScene/Option/tower%d_can_toR%d.png", towerSelected->id, towerSelected->rank + 1)->getCString()));
					menuItem_Up->setSelectedImage(Sprite::create(String::createWithFormat("GameScene/Option/tower%d_can_toR%d.png", towerSelected->id, towerSelected->rank + 1)->getCString()));
					menuItem_Up->setEnabled(true);
				}
			}
		}


		//�������й���
		if (heart > 0 && monsterList.empty() && nowWave >= wave && canCreateMonster == false)
		{
			isWon = true;
			isGameOver = true;
		}				
		if (heart <= 0)
		{
			isWon = false;
			isGameOver = true;
		}
			

	}
	else
	{
		
		this->pauseSchedulerAndActions();
		Director::getInstance()->getScheduler()->setTimeScale(1.0f);
		MySceneManager::myGetInstance()->toEndMenu(this);
	}
}


void GameScene::menuPauseCallback(Ref* pSender) {
	SimpleAudioEngine::getInstance()->playEffect("Music/click_normal.wav");

	MySceneManager::myGetInstance()->toPauseMenu(this);

	//��Ϸ����ֹͣ
	pauseSchedulerAndActions();
}

void GameScene::menuMusicCallback(Ref* pSender) {
	SimpleAudioEngine::getInstance()->playEffect("Music/click_normal.wav");

	if (UserDefault::getInstance()->getBoolForKey("isbgMusicOpen")) {
		musicRight->setTexture("MenuScene/bt_bgMusicClose.png");
		SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
		UserDefault::getInstance()->setBoolForKey("isbgMusicOpen", false);
	}
	else {
		musicRight->setTexture("MenuScene/bt_bgMusicOpen.png");
		SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
		UserDefault::getInstance()->setBoolForKey("isbgMusicOpen", true);
	}
	UserDefault::getInstance()->flush();

}

void GameScene::menuWinCallback(Ref* psender) {
	this->pauseSchedulerAndActions();
	isWon = true;
	SimpleAudioEngine::getInstance()->playEffect("Music/click_normal.wav");
	MySceneManager::myGetInstance()->toEndMenu(this);

	Director::getInstance()->getScheduler()->setTimeScale(1.0f);
}

void GameScene::menuFailCallback(Ref* psender) {
	isWon = false;
	this->pauseSchedulerAndActions();
	SimpleAudioEngine::getInstance()->playEffect("Music/click_normal.wav");
	MySceneManager::myGetInstance()->toEndMenu(this);

	Director::getInstance()->getScheduler()->setTimeScale(1.0f);
}


void GameScene::getMoney(Point point)
{
	SimpleAudioEngine::getInstance()->playEffect("Music/getMoney.wav");

	srand((unsigned)time(nullptr));

	int temp = rand() % 10;
	Blow* coin = Blow::create();

	coin->setScale(0.6f);
	coin->setPosition(point.x, point.y + 50);
	this->addChild(coin, NUM_ZO);

	if (temp <= 5)
	{
		money += 30;
		coin->setTexture("GameScene/money2.png");
	}
	else
	{
		money += 20;
		coin->setTexture("GameScene/money1.png");
	}

	auto easeIn = EaseExponentialIn::create(Spawn::create(MoveBy::create(0.7f, Vec2(0, 150)), FadeOut::create(0.7f), NULL));
	auto seq = Sequence::create(easeIn, CallFuncN::create(CC_CALLBACK_0(Blow::stopBlow, coin)), NULL);
	coin->runAction(seq);

	moneyLabel->setString(String::createWithFormat("%d", money)->getCString());
}

void GameScene::showTip()
{
	for (auto iter : blankVec)
	{
		auto tipSp = Blow::create();
		tipSp->setTexture("GameScene/cube_tip.png");
		tipSp->setPosition(iter->getPositionX() + 52.5, iter->getPositionY() + 40);
		tipSp->setOpacity(0);
		this->blowList.push_back(tipSp);
		this->addChild(tipSp, 10);


		auto in = FadeIn::create(0.3f);
		auto in2 = in->clone();
		auto in3 = in->clone();

		auto out = FadeOut::create(0.5f);
		auto out2 = out->clone();
		auto out3 = out->clone();

		auto delay = DelayTime::create(0.2f);
		auto d2 = delay->clone();
		auto d3 = delay->clone();

		tipSp->runAction(Sequence::create(
			in,
			delay,
			out,
			in2,
			d2,
			out2,
			in3,
			d3,
			out3,
			CallFuncN::create(CC_CALLBACK_0(Blow::stopBlow, tipSp)),
			NULL
		));
	}
}

void GameScene::finalWaveTip()
{
	SimpleAudioEngine::getInstance()->playEffect("Music/warning.wav");

	auto bg_final = Blow::create();
	bg_final->setTexture("GameScene/bg_final.png");
	bg_final->setPosition(Vec2(-550, 500));		//550,600
	this->blowList.push_back(bg_final);
	this->addChild(bg_final, 30);

	auto finalWave = Sprite::create("GameScene/final_1.png");
	finalWave->setPosition(Vec2(300, -50));
	bg_final->addChild(finalWave);

	SpriteFrame* frame = NULL;
	Vector<SpriteFrame*> frameVec;
	SpriteFrameCache * framecache = SpriteFrameCache::getInstance();
	framecache->addSpriteFramesWithFile("GameScene/final.plist", "GameScene/final.png");

	for (int i = 1; i <= 2; i++) {
		frame = framecache->getSpriteFrameByName(StringUtils::format("final_%d.png", i));
		frameVec.pushBack(frame);
	}

	Animation *animation = Animation::createWithSpriteFrames(frameVec);
	animation->setLoops(-1);
	animation->setDelayPerUnit(0.2f);
	Animate * action = Animate::create(animation);

	finalWave->runAction(action);

	auto moveIn = MoveTo::create(1.5f, Vec2(550, 500));
	auto moveOut = MoveTo::create(1.5f, Vec2(1750, 500));
	auto _moveIn = EaseSineOut::create(moveIn);
	auto _moveOut = EaseSineIn::create(moveOut);
	bg_final->runAction(Sequence::create(_moveIn, _moveOut, CallFuncN::create(CC_CALLBACK_0(Blow::stopBlow,bg_final)),NULL));
}

void GameScene::countDown()
{
	auto countSp1 = Blow::create();
	countSp1->setTexture("GameScene/ready_1.png");
	countSp1->setPosition(visibleSize.width / 2 + visibleOrigin.x, visibleSize.height / 2 + visibleOrigin.y);
	countSp1->setVisible(false);
	this->blowList.push_back(countSp1);
	this->addChild(countSp1, 100);

	auto countSp2 = Blow::create();
	countSp2->setTexture("GameScene/ready_2.png");
	countSp2->setPosition(visibleSize.width / 2 + visibleOrigin.x, visibleSize.height / 2 + visibleOrigin.y);
	this->blowList.push_back(countSp2);
	this->addChild(countSp2, 100);

	auto countSpGo = Blow::create();
	countSpGo->setTexture("GameScene/ready_go.png");
	countSpGo->setPosition(visibleSize.width / 2 + visibleOrigin.x, visibleSize.height / 2 + visibleOrigin.y);
	countSpGo->setVisible(false);
	this->blowList.push_back(countSpGo);
	this->addChild(countSpGo, 100);

	auto delay1 = DelayTime::create(1.0f);
	auto delay2 = DelayTime::create(2.0f);

	auto scale1 = ScaleTo::create(0.4f, 0.3f);
	auto scale2 = scale1->clone();
	auto scale3 = scale1->clone();

	auto _delay1 = DelayTime::create(0.2f);
	auto _delay2 = _delay1->clone();
	auto _delay3 = _delay1->clone();

	auto _scale1 = ScaleTo::create(0.4f, 0.05f);
	auto _scale2 = _scale1->clone();
	auto _scale3 = _scale1->clone();

	auto dong_1 = CallFunc::create([]() {SimpleAudioEngine::getInstance()->playEffect("Music/dong.wav"); });
	auto dong_2 = dong_1->clone();
	auto ding = CallFunc::create([]() {SimpleAudioEngine::getInstance()->playEffect("Music/ding.wav"); });


	countSp2->runAction(Sequence::create(dong_1,scale1,_delay1,_scale1,CallFunc::create(CC_CALLBACK_0(Blow::stopBlow, countSp2)),NULL));

	countSp1->runAction(Sequence::create(delay1, dong_2,CallFuncN::create(CC_CALLBACK_0(Blow::setVisible, countSp1, true)),scale2, _delay2, _scale2, CallFunc::create(CC_CALLBACK_0(Blow::stopBlow, countSp1)),NULL));

	countSpGo->runAction(Sequence::create(delay2, ding,CallFuncN::create(CC_CALLBACK_0(Blow::setVisible, countSpGo, true)),scale3, _delay3, _scale3, CallFunc::create(CC_CALLBACK_0(Blow::stopBlow, countSpGo)),NULL));
}

void GameScene::startGame()
{
	//���㴥������
	auto touchlistener = EventListenerTouchOneByOne::create();
	touchlistener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchlistener, this);

	this->showTip();

	//���ݵ�ǰ�أ�������Ӧ��������
	switch (UserDefault::getInstance()->getIntegerForKey("nowLevel"))
	{
	case 1:
		SimpleAudioEngine::getInstance()->playBackgroundMusic("Music/bg_Game_normal.wav", true);
		break;
	case 2:
	case 5:
		SimpleAudioEngine::getInstance()->playBackgroundMusic("Music/bg_ice.mp3", true);
		break;
	case 3:
	case 4:
		SimpleAudioEngine::getInstance()->playBackgroundMusic("Music/bg_fire.mp3", true);
		break;
	case 6:
		SimpleAudioEngine::getInstance()->playBackgroundMusic("Music/bg_light.mp3", true);
		break;

	default:
		break;
	}

	if (!UserDefault::getInstance()->getBoolForKey("isbgMusicOpen")) {
		SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
	}
}

void GameScene::menuSpeedCallback(Ref * psender) {
	SimpleAudioEngine::getInstance()->playEffect("Music/click_normal.wav");

	if (!isDoubleSpeed)
	{//����
		isDoubleSpeed = true;
		speed->setTexture("GameScene/speed_icon_2.png");
		Director::getInstance()->getScheduler()->setTimeScale(2.0f);
	}
	else
	{//�ر�
		isDoubleSpeed = false;
		speed->setTexture("GameScene/speed_icon_1.png");
		Director::getInstance()->getScheduler()->setTimeScale(1.0f);
	}
}
