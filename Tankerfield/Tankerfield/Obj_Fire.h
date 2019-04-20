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

	bool death = false;

	int frame_explosion = 0;

	float speed = 0.f;
	float time = 0.f;

	Circle range_pos;

	// ---
	Animation anim;
	SDL_Texture * tex = nullptr;
};

#endif