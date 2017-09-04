#pragma once
#include"cocos2d.h"

typedef enum {
	UP,
	DOWN,
	RIGHT,
	LEFT
}Direction;


//Monster��ĳ�Ա����ֻ��������Ĳ���
class Monster : public cocos2d::Sprite {
public:
	double blood;																	//��ǰѪ��
	double maxblood;															//���Ѫ��
	int id;																				//����
	int zo;																				//ZOrder
	double speed;																	//�ٶ�
	Direction dir;																	//����
	int way;																			//��ǰ��·����λ��
	cocos2d::Sprite* bloodBox;											//Ѫ����
	cocos2d::Sprite* bloodSp;												//Ѫ��
	cocos2d::Sprite* icon;													//��ѡ�еı�ʶ
	cocos2d::ProgressTimer* timer;											

	bool isMoving;																//�����Ƿ����ƶ���־λ

	bool isGettingHurt = false;		

	cocos2d::Vector<cocos2d::SpriteFrame*> getHitFrames;	//�ӵ����й���

public:
	static Monster* create(int,int );
	void move(cocos2d::Point);
	void setMove() { isMoving = false; };

	/////////////////////////////
	void cutBlood();
	void resumeFromB2();
	void slowDown(int);
	void resumeFromB3(int);
};