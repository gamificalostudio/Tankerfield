#ifndef __BULLET_LASER__H__
#define __BULLET_LASER__H__

#include "PerfTimer.h"
#include "Point.h"
#include "Obj_Bullet.h"

struct SDL_Texture;
class Obj_TeslaTrooper;

class Laser_Bullet : public Obj_Bullet
{
public:

	Laser_Bullet(fPoint pos);
	~Laser_Bullet();

public:
	//Add functions that are overriden
	//void OnTrigger(Collider* collider_1);

public:
	bool Start();

public:
	bool charged = false;
	uint kill_counter = 0u;
	uint kill_counter_max = 0u;

private:

};

#endif