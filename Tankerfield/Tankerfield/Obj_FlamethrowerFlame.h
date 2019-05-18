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
	bool Draw(float dt, Camera* camera) override;

public:
	Obj_Tank * tank		= nullptr;

private:
	Animation anim;
	SDL_Texture * tex	= nullptr;
	float scale = 1.f;
	SDL_Point pivot;
};

#endif