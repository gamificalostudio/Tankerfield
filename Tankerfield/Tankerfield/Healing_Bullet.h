#ifndef __HEALING_BULLET_H__
#define __HEALING_BULLET_H__

#include "PerfTimer.h"
#include "Point.h"
#include "Obj_Bullet.h"
#include "Obj_Tank.h"

struct SDL_Texture;

class Healing_Bullet : public Obj_Bullet
{
public:
	Healing_Bullet(fPoint pos);
	~Healing_Bullet();

public:
	//Add functions that are overriden
	bool Start();

public:
	Obj_Tank* tank_parent = nullptr;
};

#endif