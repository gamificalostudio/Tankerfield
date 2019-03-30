#ifndef __OBJ_BULLET_H__
#define __OBJ_BULLET_H__

#include "Object.h"

class Obj_Bullet : public Object
{
public:
	Obj_Bullet();
	~Obj_Bullet();

	//No need to rewrite them if they do the same

	bool Update(float dt);
	bool PostUpdate();
	//OnCollision{
	//Do damage to entity
		//Show explosion particle
		//Sound
	//}

protected:
	float speed = 0.f;
	fPoint direction = { 0.f, 0.f };
	int damage = 0;
	float bullet_life_ms = 0.f;
	//SFX impact_sound
	//Particle impact_particle
};

#endif
