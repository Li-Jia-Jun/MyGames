#pragma once
#include<iostream>
#include"cocos2d.h"
#include"BaseCube.h"
#include"BasePuzzle.h"
#include"SimpleAudioEngine.h"
USING_NS_CC;

struct Selected {
	int x;
	int y;
};

class GameScene : public Layer {
public:
	CREATE_FUNC(GameScene);
	//int preSelected;
	Selected selected;
	Selected preSelected;
	int count;
	BaseCube* now_puz[4][4];//as paper 
	BasePuzzle* basePuzzle;
	BaseCube* now_ans[4][4];
	Sprite* bgImage;
	Sprite* bg_time;
	Sprite* bg_count;

	int time;
	Sprite* time1;
	Sprite* time2;
	Sprite* time3;
	int s = 0;
	int ss = 0;
	int m = 0;
	Sprite* count_ge;
	Sprite* count_shi;

public:
	GameScene();
	~GameScene();
	
	void setDefalut();

public:
	static Scene* createScene();
	virtual bool init();

	void update(float dt);

	void fillInCallBack(Ref* pSender);//the call back of four fillin buttons
	void deleteCallBack(Ref* pSender);
	void cleanCallBack(Ref* pSender);
	void checkCallBack(Ref* pSender);
	void pauseCallBack(Ref* pSender);
	//void returnGame();//used when something wrong is checked out in paper 
	//void endGame();//used when paper is correct
};