#pragma once
#include<iostream>
#include"BaseCube.h"
#include"cocos2d.h"
USING_NS_CC;

class Global {
public:
	static BaseCube* selected_puz[4][4];
	static BaseCube* selected_ans[4][4];
	static int mode;
};
