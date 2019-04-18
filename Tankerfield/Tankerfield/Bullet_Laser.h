#ifndef __BULLET_LASER__H__
#define __BULLET_LASER__H__

#include "PerfTimer.h"
#include "Point.h"
#include "Obj_Bullet.h"

struct SDL_Texture;

class Bullet_Laser : public Obj_Bullet
{
public:

	Bullet_Laser(fPoint pos);
	~Bullet_Laser();

public:
	//Add functions that are overriden
	void OnTrigger(Collider* collider_1);

public:

	Collider* explosion = nullptr;

private:

};

#endif