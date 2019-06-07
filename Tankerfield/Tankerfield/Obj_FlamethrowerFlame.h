#ifndef __OBJ_FLAMETHROWERFLAME_H__
#define __OBJ_FLAMETHROWERFLAME_H__

#include "Circle.h"
#include "Point.h"
#include "Animation.h"
#include "Object.h"
#include "PerfTimer.h"
#include "WeaponInfo.h"

struct SDL_Texture;
class Timer;

class Obj_FlamethrowerFlame : public Object
{
public:

	Obj_FlamethrowerFlame(fPoint pos);
	~Obj_FlamethrowerFlame();

	bool Update(float dt) override;
	bool Draw(Camera* camera) override;

public:
	Obj_Tank * tank		= nullptr;
	bool is_holding = false;

private:
	Animation fire_start;
	Animation fire;
	Animation fire_reverse;
	Animation fire_end;
	SDL_Texture * tex	= nullptr;
	float scale = 1.f;
	SDL_Point pivot;
	iPoint			draw_offset_original = { 0, 0 }; //the original offset without de offset of the direction.
};

#endif