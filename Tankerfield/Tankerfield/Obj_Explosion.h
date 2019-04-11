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
	SDL_Texture * tex;

	bool death = false;
	float speed = 0.f;

	Circle range_pos;

	Collider* coll_explosion=nullptr;

	// ---
	/* Attack properties */
	PerfTimer perf_timer;

	Animation* explosion_anim;
	SDL_Texture * explosion_tex;
};

#endif