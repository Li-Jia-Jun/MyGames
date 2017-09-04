#pragma once
#include"cocos2d.h"

class Monster;
class GameScene;

//Tower��ĳ�Ա����ֻ��������Ĳ���
class Tower : public cocos2d::Sprite {
public:
	int id;																					//����
	int attack;																			//������
	int range;																			//��Χ
	int rank;																				//�ȼ�
	double delay;																		//������
	int value;																				//��ǰ�۸�
	int upValue;																		//�����۸���������ʱupValue = -1��isMax = true��
	int sellValue;																		//���ۼ۸�	

	int bulletSpeed;
	float bulletDuration;

	bool isFiring;																		//�����־λ
	bool isMax;																			//������־λ

	cocos2d::Sprite* icon;														//����ͼ�꣨һ��ʼ���ɼ���

	Monster* target;																	//��������ڵĵ���

public:
	static Tower* create(int id);

	void fire() { isFiring = true; };
	void reload() { isFiring = false; };

	void showIcon() { icon->setVisible(true); };

	void upgrade();
};