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
	~Bullet_Missile();

public:

	//Add functions that are overriden
	void OnTrigger(Collider* collider_1);

public:

	Collider* explosion = nullptr;

private:

	int explosion_t;

	PerfTimer explosion_time;
};

#endif
