#ifndef __BULLET_ROCKETLAUNCHER__H__
#define __BULLET_ROCKETLAUNCHER__H__

#include "PerfTimer.h"
#include "Point.h"
#include "Obj_Bullet.h"

class Bullet_RocketLauncher : public Obj_Bullet
{
public:

	Bullet_RocketLauncher(fPoint pos);
	~Bullet_RocketLauncher();

public:
	//Add functions that are overriden
	//void OnTrigger(Collider* collider_1);

public:
	bool Start();

public:

private:
	float attack_damage = 0.0f;
};

#endif /* __BULLET_ROCKETLAUNCHER__H__ */