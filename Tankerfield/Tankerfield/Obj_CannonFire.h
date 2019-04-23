#ifndef __OBJ_CANNON_FIRE_H__
#define __OBJ_CANNON_FIRE_H__

#include "Animation.h"
#include "Point.h"
#include "Object.h"

class SDL_Texture;

class Obj_CannonFire : public Object
{
public:
	Obj_CannonFire(fPoint pos_map);

	bool Update(float dt) override;

private:
	Animation anim;
	SDL_Texture * tex = nullptr;
};

#endif