#ifndef __OBJ_EXPLOSION_H__
#define __OBJ_EXPLOSION_H__

#include "Circle.h"
#include "Point.h"
#include "Animation.h"
#include "Object.h"
#include "PerfTimer.h"

struct SDL_Texture;
class Timer;

class Obj_Explosion : public Object
{
public:

	Obj_Explosion(fPoint pos);
	~Obj_Explosion();

	bool Update(float dt) override;

	bool Awake(pugi::xml_node&) { return true; };

private:

	bool death = false;

	int frame_explosion = 0;

	float speed = 0.f;
	float time = 0.f;

	Circle range_pos;
	
	// ---
	static Animation* anim;
	static SDL_Texture * tex;
};

#endif