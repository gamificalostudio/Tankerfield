#ifndef __OBJ_OILPOOL__
#define __OBJ_OILPOOL__

#include "Object.h"
#include "PerfTimer.h"

class Obj_OilPool : public Object
{

public:

	Obj_OilPool(fPoint pos);
	~Obj_OilPool();

public:

	bool Update(float dt);

private:

	PerfTimer time;
};

#endif // !__OBJ_OILPOOL__

