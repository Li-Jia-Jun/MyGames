#pragma once

#include"cocos2d.h"
#include"MySceneManager.h"

USING_NS_CC;

class EndMenu : public Layer {

public:
	CREATE_FUNC(EndMenu);
	virtual bool init();
	
	void showMenu();		//����GameScene����node��isWon��ֵ�ж�Ҫ��ʾ������

	bool isWon;
	int killNum;

private:
	void menuNextCallback(Ref* pSender);
	void menuRestartCallback(Ref* pSender);
	void menuBackCallback(Ref* pSender);

	Sprite* bg_bottom;
	Sprite* bg_board;
	Sprite* title;
	Menu* menu;
	MenuItem* nextItem;
};