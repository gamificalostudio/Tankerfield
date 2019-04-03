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


	//void OnCollision(Collider*, Collider*, float dt);

private:

	void Draw();
	void SetRect(int x, int y, int w, int h);

private:
	SDL_Rect frame;
	//Type type;
};

#endif