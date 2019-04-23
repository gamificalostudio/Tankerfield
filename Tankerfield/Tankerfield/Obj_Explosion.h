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
	~Obj_Explosion();

	bool Update(float dt) override;

	//void SetExplosionDamage(WeaponInfo info);

	bool Awake(pugi::xml_node&) { return true; };

private:

	bool death = false;

	int frame_explosion = 0;
	int damage = 0;
	int original_damage = 200;
	int level = 0;

	float speed = 0.f;
	float time = 0.f;

	Circle range_pos;
	
	WeaponInfo weapon_info;

	// ---
	Animation anim;
	SDL_Texture * tex = nullptr;

	float coll_w = 0.f;
	float coll_h = 0.f;
};

#endif