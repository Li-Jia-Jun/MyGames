#pragma once
#include"cocos2d.h"

class Monster;
class Tower;

class Bullet : public cocos2d::Sprite {
public:
	static Bullet* create(Tower*);

	void stopFlying();

public:
	int id;											//����
	int speed;									//�����ٶ�
	int attack;									//������
	bool isWithin;							//�Ƿ�򵽹�������

	float duration;

	bool isFlying;								//�Ƿ����ڷ��еĿ���λ

	Tower* tower;								//������ӵ�����

	cocos2d::Vector<cocos2d::SpriteFrame*> flyFrames;
	cocos2d::Vector<cocos2d::SpriteFrame*> blowFrames;
	cocos2d::Vector<cocos2d::SpriteFrame*>effectFrames;
};
