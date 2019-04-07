#ifndef __OBJ_BULLET_H__
#define __OBJ_BULLET_H__

#include "Object.h"

class PerfTimer;
class Collision;
struct SDL_Texture;

class Obj_Bullet : public Object
{
public:

	Obj_Bullet(fPoint pos);

	~Obj_Bullet();

	//No need to rewrite them if they do the same
  
	bool Start() override;
	bool Update(float dt) override;
	
	void OnTrigger(Collider* collider_1) override;

public:

	float speed = 0.f;

	fPoint direction = { 0.f, 0.f };

	int damage = 0;

	float bullet_life_ms = 0.f;

	PerfTimer bullet_life_ms_timer;

	float time_between_bullets = 0.f;

	static SDL_Texture * tex;
	static Animation * anim;

	//SFX impact_sound

	//Particle impact_particle
};

#endif
