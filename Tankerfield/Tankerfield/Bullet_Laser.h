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
	Collider* explosion = nullptr;

public:
	std::vector<Object*> hitted_enemies; //object to include all types of enemies

private:

};

#endif