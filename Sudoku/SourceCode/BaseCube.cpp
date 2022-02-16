#include "BaseCube.h"

//bool BaseCube::init() {
//
//	return true;
//}

BaseCube::BaseCube(int value, CUBE_TYPE type, char* imageName)
{
	sprite = Sprite::create();
	//sprite->setAnchorPoint(Vec2(0, 0));
	//this->addChild(sprite);

	this->value = value;
	this->type = type;
	this->imageName = imageName;
	sprite->setTexture(this->imageName);
	// bool isInit = init();
}

void BaseCube::setBaseCube(int value, CUBE_TYPE type, char* imageName)
{
	this->value = value;
	this->type = type;
	this->imageName = imageName;
	sprite->setTexture(this->imageName);
}

BaseCube::~BaseCube()
{

}

void BaseCube::change(int value, CUBE_TYPE type, char* imageName)
{
	this->value = value;
	this->type = type;
	this->imageName = imageName;
	sprite->setTexture(this->imageName);
}

//int BaseCube::getValue()
//{
//	return value;
//}

void BaseCube::changeCUBE_TYPE(CUBE_TYPE type)
{
	this->type = type;
}