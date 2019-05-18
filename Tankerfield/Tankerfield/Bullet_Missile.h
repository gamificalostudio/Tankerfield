#ifndef __BULLET_MISSILE_H__
#define __BULLET_MISSILE_H__

#include "PerfTimer.h"
#include "Point.h"
#include "Obj_Bullet.h"

struct SDL_Texture;

class Bullet_Missile : public Obj_Bullet
{
public:

	Bullet_Missile(fPoint pos);

	void OnTriggerEnter(Collider* collider_1);

public:
	int explosion_damage = 0;
};

#endif
