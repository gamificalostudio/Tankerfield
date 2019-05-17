#ifndef __OBJ_BUILDING_H__
#define __OBJ_BUILDING_H__

#include "Point.h"
#include "Rect.h"
#include "Animation.h"
#include <map>
#include "Object.h"

struct SDL_Texture;

class Obj_Building : public Object
{
public:

	Obj_Building(fPoint pos);
	~Obj_Building();

	void SetTexture(const char* path);
	void SetCollider(const fRect & collider_rect);

public:
	const char* path = "";
private:

	static SDL_Texture* texture;
};

#endif