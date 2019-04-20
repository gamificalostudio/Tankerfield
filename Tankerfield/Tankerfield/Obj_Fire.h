#ifndef __OBJ_FIRE_H__
#define __OBJ_FIRE_H__

#include "Circle.h"
#include "Point.h"
#include "Animation.h"
#include "Object.h"
#include "PerfTimer.h"

struct SDL_Texture;
class Timer;

class Obj_Fire : public Object
{
public:

	Obj_Fire(fPoint pos);
	~Obj_Fire();

	bool Update(float dt) override;

public:
	Obj_Tank * tank = nullptr;

private:

	// ---
	Animation anim;
	SDL_Texture * tex = nullptr;
};

#endif