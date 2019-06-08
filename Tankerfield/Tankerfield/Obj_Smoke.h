#ifndef __OBJ_SMOKE_H__
#define __OBJ_SMOKE_H__

#include "Circle.h"
#include "Point.h"
#include "Animation.h"
#include "Object.h"
#include "PerfTimer.h"

struct SDL_Texture;
class Timer;

class Obj_Smoke : public Object
{
public:

	Obj_Smoke(fPoint pos);
	~Obj_Smoke();

	bool Update(float dt) override;

public:
	Obj_Tank * tank = nullptr;

private:

	// ---
	Animation anim;
	SDL_Texture * tex = nullptr;
};

#endif