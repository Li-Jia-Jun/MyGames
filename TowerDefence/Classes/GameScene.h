#pragma once
#include"cocos2d.h"
#include "MySceneManager.h"
//#include"TowerOption.h"

class Monster;
class Tower;
class Bullet;
class Blow;
class Effect;

////////////////��Ϸ�ж���������Ҫ�޸ĵ��ٶ�ֵ
	//�ӵ������ٶȡ�
	//���ﶯ�������ٶȡ�
	//	�����������ٶȡ�
	//	�������ٶȡ�
	//	�����ƶ��ٶȡ�
	//	ˢ���ٶȡ�
	//	ˢ�ֵ���ʱ�ٶȡ�


class GameScene : public cocos2d::Layer {
public:

	cocos2d::Sprite* Yeye;										//үү~

	int money;															//��Ǯ
	int heart;																//������
	int nowWave;														//��ǰ����
	int wave;																//����
	cocos2d::LabelTTF* moneyLabel;
	cocos2d::LabelTTF* heartLabel;
	cocos2d::LabelTTF* waveLabel;
	cocos2d::LabelTTF* levelLabel;					
	cocos2d::Sprite* bar;											//��Ϸ�Ϸ��˵���

	std::list<Monster*> monsterList;						//��������
	std::list<Tower*> towerList;								//����������
	std::list<Bullet*>bulletList;								//�ӵ�����
	std::list<Blow*>blowList;									//Ч������
	std::list<Effect*>effectList;								//�ӵ�Ч������

	cocos2d::Vector<Node*> blankVec;					//�հ״�����
	cocos2d::TMXLayer* blankLayer;										

	std::vector<cocos2d::Point> path;					//·��

	std::vector<std::string>waveInfo;						//������Ϣ����һ���ǲ�����������ÿ��������Ϣ��

	bool isGameOver;												//��Ϸ�Ƿ��������λ
	int killNum;															//ɱ����
	int towerBuilt;														//���ֽ�����
	bool isWon;															//��Ϸ�Ƿ�ʤ������λ
	bool canCreateMonster;										//�Ƿ�����������λ
	bool isShowingOption;										//�Ƿ�������ʾ�˵�����λ
	bool isDoubleSpeed;											//�����ٿ���λ
	bool isFinalWave;												//�Ƿ������һ��

	cocos2d::Size visibleSize;
	cocos2d::Point visibleOrigin;

	int level;																//�ؿ�


	Monster* targetSelected;									//ѡ�еĹ���
	Tower* towerSelected;										//ѡ�е�����������ʾ�˵���
	cocos2d::Point* cubeSelectedPoint;					//ѡ�еĽ�������ê��Ϊ���ģ�

	cocos2d::Menu* menu;										//optionMenu
	cocos2d::MenuItemImage* menuItem_T1;		//������1�˵���
	cocos2d::MenuItemImage* menuItem_T2;		//������2�˵���
	cocos2d::MenuItemImage* menuItem_T3;		//������3�˵���
	cocos2d::MenuItemImage* menuItem_Sell;		//���۲˵���
	cocos2d::MenuItemImage* menuItem_Up;		//�����˵���


	cocos2d::Sprite*  cubeSelectedSp;					//ѡ�пհ׵ı�ʶ

public:
	CREATE_FUNC(GameScene);

	static cocos2d::Scene* createScene();

	virtual bool init();

	void initMap();													//��ȡ��ͼ��Ϣ

	void getWaveInfo();											//��ȡ���ﲨ��Ϣ

	bool initPath();													//��ȡ·����Ϣ

	void initOptionMenu();										//��ʼ���˵��㣨һ��ʼ���ɼ������ɵ����

	void monsterCreate(float);								//������һ������

	void monsterMove();											//���й����ƶ�

	void showOptionInBlank();								//�հ״���ѡ��˵�
	void showOptionWithTower();							//����������ѡ��˵�

	bool onTouchBegan(cocos2d::Touch*,cocos2d::Event*);

	void towerAttack(Tower*);									//������

	void towerReload(Tower*);								//���������

	void getMoney(Point);										//��ȡ��Ǯ

	void showTip();													//��ʼʱ��ʾ������

	void countDown();												//��ʼǰ����ʱ

	void startGame();												//������Ϸ

	void finalWaveTip();											//���һ��������ʾ

	void cleanOption();											//ȡ���˵�

	void Myupdate(float);
private:
	void createTowerCallBack(cocos2d::Ref*);		//�����˵��ص��¼�

	void upgradeTowerCallBack(cocos2d::Ref*);	//�������˵��ص��¼�

	void menuPauseCallback(Ref* pSender);
	void menuMusicCallback(Ref* pSender);
	void menuWinCallback(Ref* psender);
	void menuFailCallback(Ref* psender);
	void menuSpeedCallback(Ref * psender);

	Sprite* musicRight;
	Sprite* speed;
};

///////////////////////////////
//��ű�ը����ʱ����
class Blow : public cocos2d::Sprite {
public:
	CREATE_FUNC(Blow);
	bool isBlowing = true;

	void stopBlow() { isBlowing = false; };
};

//////////////////////////////
//����ӵ���Ч����ʱ����
class Effect : public cocos2d::Sprite {
public:
	CREATE_FUNC(Effect);
	void stopUsing() { this->setVisible(false); };
};