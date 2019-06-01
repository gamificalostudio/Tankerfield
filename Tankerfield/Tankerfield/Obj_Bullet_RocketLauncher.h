#ifndef __OBJ_BULLET_ROCKETLAUNCHER_H__
#define __OBJ_BULLET_ROCKETLAUNCHER_H__

#include "Object.h"

class PerfTimer;
class Collision;
struct SDL_Texture;
class Obj_Tank;

class Obj_Bullet_RocketLauncher : public Object
{
public:

	Obj_Bullet_RocketLauncher(fPoint pos);

	~Obj_Bullet_RocketLauncher();

	//No need to rewrite them if they do the same

	bool Start();

	bool Update(float dt);

	void OnTriggerEnter(Collider* collider_1);

	void SetBulletProperties(float speed, float bullet_life_ms, float damage, fPoint direction, float angle, bool charged = false);

	void SetPlayer(Obj_Tank* player);

public:

	float speed = 0.f;

	fPoint direction = { 0.f, 0.f };

	float bullet_life_ms = 0.f;

	PerfTimer bullet_life_ms_timer;

	SDL_Texture* tex = nullptr;

	Animation anim;

	Obj_Tank* player = nullptr;

	bool charged = false;

	//SFX impact_sound

	//Particle impact_particle
};

#endif /* __OBJ_BULLET_ROCKETLAUNCHER_H__ */