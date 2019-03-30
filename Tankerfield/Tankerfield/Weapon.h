#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "Object.h"
#include "PerfTimer.h"
#include "Point.h"
#include "M_ObjManager.h"

struct SDL_Texture;

class Weapon
{
public:
	Weapon(int damage, float speed, float life, ObjectType type);
	~Weapon();

public:
	virtual void Shoot();

protected:
	void CalculateDirection();

public:
	int damage = 0;
	float speed = 0.f;
	float bullet_life_ms = 0.f;
	ObjectType bullet_type;

};

#endif