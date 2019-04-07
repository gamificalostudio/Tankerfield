#ifndef __OBJ_STATIC_H__
#define __OBJ_STATIC_H__

#include "Point.h"
#include "Animation.h"
#include <map>
#include "Object.h"

struct SDL_Texture;

class Obj_Static : public Object
{
public:

	Obj_Static();
	Obj_Static(fPoint pos);
	~Obj_Static();

	bool Awake(pugi::xml_node & static_node) override;
	bool Start() override;
	bool PostUpdate(float dt) override;

	//void OnCollision(Collider*, Collider*, float dt);

private:



private:

	SDL_Texture* garage = nullptr;
	//Type type;
};

#endif