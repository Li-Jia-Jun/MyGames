#pragma once
#include "cocos2d.h"
#include"MySceneManager.h"
USING_NS_CC;

class MenuScene : public cocos2d::Layer
{
public:
	static Scene* createScene();
	CREATE_FUNC(MenuScene);
	virtual bool init();

	MenuItemImage* leftItem;		//�������Ұ�ť
	MenuItemImage* rightItem;
	MenuItemImage* backItem;
	MenuItemImage* bgMusicItem;
	Sprite* musicRight;					//���ְ�ť�ұ߲���

private:

	void menuStartCallback(Ref* pSender);
	void menuAchieveCallback(Ref* pSender);
	void menuBackCallback(Ref* pSender);
	void menuMonsterCallback(Ref* pSender);
	void menuTowerCallback(Ref* pSender);
	void menuMusicCallback(Ref* pSender);
	void menuLeftCallback(Ref* pSender);
	void menuRightCallback(Ref* pSender);
	void setNowLevel(unsigned int);		//�����޸Ķ������ֺ�UserDefault�е�nowLevel  + �޸ĵײ�������Ϣ
	void setMapMove(bool isNextLevel);		//�����ƶ���ͼִ�ж���
	void removeMap();

	Sprite* levelNum;	//����level�Ҳ�����

	MenuItemImage* startItem;	//δ����ʱUnable
	MenuItemImage* _startItem;

	Sprite* lastLevelMap;		//ѡ�ؽ���������ͼ������ִ�ж���
	Sprite*nowLevelMap;
	Sprite* nextLevelMap;
	Sprite* bottomLevelMap; //���ڱ��ƶ��ĵײ��ͼ�£���ִ�ж���

	Sprite* nowLevelInfo;		//�ײ�������Ϣ
	
	unsigned int nowLevel;  //��ǰ��ʾ�Ĺؿ� �������� �м�ͼ �Լ���GameScene���Ĺؿ�ֵ��
	unsigned int topLevel;
};
