#ifndef __HEALINGSHOT_AREA__
#define __HEALINGSHOT_AREA__

#include "App.h"
#include "M_Collision.h"
#include "Object.h"
#include "PerfTimer.h"
#include "Obj_Bullet.h"

class PerfTimer;
class Collision;
struct SDL_Texture;
class Obj_Tank;

class HealingShot_Area : public Obj_Bullet
{
public:

	HealingShot_Area(fPoint pos);

	~HealingShot_Area();

	bool Start() override;

	bool Update(float dt) override;

public:
	int healing = 15;

public:
	Obj_Tank* tank_parent = nullptr;

private:
	PerfTimer time;

};

#endif // !__HEALING_AREA_SHOT__
