#ifndef __OBJ_HEALING_ANIMATION_H__
#define __OBJ_HEALING_ANIMATION_H__

#include "Point.h"
#include "Animation.h"
#include <map>
#include "Object.h"

struct SDL_Texture;

class Obj_Healing_Animation : public Object
{
public:

	Obj_Healing_Animation(fPoint pos);
	~Obj_Healing_Animation();

	bool Update(float dt);

private:
	static Animation* anim;
	static SDL_Texture * tex;

};

#endif