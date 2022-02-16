#pragma once
#include<iostream>
#include"cocos2d.h"
USING_NS_CC;

typedef enum {
	TYPE_UNDO,
	TYPE_SELECTED,
	TYPE_FILLED,
	TYPE_BLANK,
	TYPE_WRONG,
}CUBE_TYPE;

class BaseCube {
public:
	//CREATE_FUNC(BaseCube);
	//virtual bool init();
	void setBaseCube(int value, CUBE_TYPE type, char* imageName);

	BaseCube(int value, CUBE_TYPE type, char* imageName);
	~BaseCube();

public:
	void change(int value, CUBE_TYPE type, char* imageName);
	
	void changeCUBE_TYPE(CUBE_TYPE type);

public:
	int value;
	CUBE_TYPE type;
	std::string imageName;
	Sprite* sprite;
};