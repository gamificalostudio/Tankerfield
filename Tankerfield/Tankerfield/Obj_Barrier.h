#ifndef __OBJ_BARRIER_H__
#define __OBJ_BARRIER_H__

#include "Object.h"

struct SDL_Texture;

class Obj_Barrier : public Object
{
public:
	Obj_Barrier(fPoint pos);

	bool Update(float dt);

private:

	Animation anim;
	SDL_Texture * tex = nullptr;

	PerfTimer time;
};

#endif
