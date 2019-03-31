#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "Object.h"
#include "PerfTimer.h"
#include "M_ObjManager.h"

class Obj_Bullet;

class Weapon
{
public:
	Weapon();
	Weapon(int damage, float speed, float life, float time_between_bullet, ObjectType type);
	~Weapon();

public:
	virtual void Shoot(float x, float y, fPoint & direction);

private:
	Obj_Bullet * bullet = nullptr;

public:
	int damage = 0;
	float speed = 0.f;
	float bullet_life_ms = 0.f;
	float time_between_bullets = 0.f;
	ObjectType bullet_type;
};

#endif