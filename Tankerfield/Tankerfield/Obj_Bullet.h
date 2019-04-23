#ifndef __OBJ_BULLET_H__
#define __OBJ_BULLET_H__

#include "Object.h"

class PerfTimer;
class Collision;
struct SDL_Texture;
class Obj_Tank;

class Obj_Bullet : public Object
{
public:

	Obj_Bullet(fPoint pos);

	~Obj_Bullet();

	//No need to rewrite them if they do the same

	bool Start();

	bool Update(float dt);
	
	void OnTrigger(Collider* collider_1);

	void SetBulletProperties(float speed, float bullet_life_ms, float damage, fPoint direction, float angle);

	void SetPlayer(Obj_Tank* player);

public:

	float speed = 0.f;

	fPoint direction = { 0.f, 0.f };

	float bullet_life_ms = 0.f;

	PerfTimer bullet_life_ms_timer;

	SDL_Texture * tex = nullptr;

	Animation anim;

	Obj_Tank* player = nullptr;

	//SFX impact_sound

	//Particle impact_particle
};

#endif
