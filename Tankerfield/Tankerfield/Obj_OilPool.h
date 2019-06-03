#ifndef __OBJ_OILPOOL__
#define __OBJ_OILPOOL__

#include "Object.h"
#include "Obj_Bullet.h"
#include "PerfTimer.h"

class Obj_OilPool : public Obj_Bullet
{

public:

	Obj_OilPool(fPoint pos);
	~Obj_OilPool();

public:

	bool Start() override;

	bool Update(float dt) override;

private:

	PerfTimer time;
};

#endif // !__OBJ_OILPOOL__

