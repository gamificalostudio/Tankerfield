#ifndef __OBJ_EXPLOSION_H__
#define __OBJ_EXPLOSION_H__

#include "Circle.h"
#include "Point.h"
#include "Animation.h"
#include "Object.h"
#include "PerfTimer.h"
#include "WeaponInfo.h"

struct SDL_Texture;
class Timer;

class Obj_Explosion : public Object
{
public:
	Obj_Explosion(fPoint pos);

	bool Update(float dt) override;

	void SetExplosionDamage(float damage);

private:
	int frame_num = 0;
	int frame_damage = 0;
	int explosion_damage = 0;
	Animation anim;
	SDL_Texture * tex = nullptr;
};

#endif