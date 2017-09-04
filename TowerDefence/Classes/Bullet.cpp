#include"Bullet.h"
#include"Tower.h"
#include"GameScene.h"
USING_NS_CC;

float BulletScaleTable[3] = { 1.0,		1.2,		1.5 };

Bullet* Bullet::create(Tower* tower)
{
	auto cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile(String::createWithFormat("GameScene/Bullet/bullet%d.plist", tower->id)->getCString());

	Bullet* bullet = new Bullet();

	if (bullet&&bullet->initWithSpriteFrameName(String::createWithFormat("bullet%d_1.png", tower->id)->getCString()))
	{
		bullet->tower = tower;
		bullet->id = tower->id;
		bullet->attack = tower->attack;
		bullet->setScale(BulletScaleTable[tower->rank - 1]);//���ݵȼ������ӵ���С
		bullet->speed = tower->bulletSpeed;
		bullet->duration = tower->bulletDuration;
		bullet->isWithin = false;

		//�������ӵ���ʼ����
		bullet->isFlying = true;

		bullet->autorelease();

		bullet->flyFrames.pushBack(
			cache->getSpriteFrameByName(String::createWithFormat("bullet%d_1.png", tower->id)->getCString())
		);
		bullet->flyFrames.pushBack(
			cache->getSpriteFrameByName(String::createWithFormat("bullet%d_2.png", tower->id)->getCString())
		);

		bullet->blowFrames.pushBack(
			cache->getSpriteFrameByName(String::createWithFormat("bullet%d_3.png", tower->id)->getCString())
		);
		bullet->blowFrames.pushBack(
			cache->getSpriteFrameByName(String::createWithFormat("bullet%d_4.png", tower->id)->getCString())
		);

		if (bullet->id == 2 || bullet->id == 3)
		{
			bullet->effectFrames.pushBack(
				cache->getSpriteFrameByName(String::createWithFormat("bullet%d_5.png", tower->id)->getCString())
			);
			bullet->effectFrames.pushBack(
				cache->getSpriteFrameByName(String::createWithFormat("bullet%d_6.png", tower->id)->getCString())
			);

		}

		return bullet;
	}
}

void Bullet::stopFlying()
{
	isFlying = false; 
}