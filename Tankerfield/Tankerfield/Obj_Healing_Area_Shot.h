#ifndef __HEALING_AREA_SHOT__
#define __HEALING_AREA_SHOT__

#include "App.h"
#include "M_Collision.h"
#include "Object.h"
#include "PerfTimer.h"

class Obj_Healing_Area_Shot : public Object
{
public:

	Obj_Healing_Area_Shot(fPoint pos);

	~Obj_Healing_Area_Shot();

	bool Update(float dt) override;

public:
	int healing = 15;
	
private:
	PerfTimer time;
};

#endif // !__HEALING_AREA_SHOT__
