#ifndef __OBJ_BUILDING_H__
#define __OBJ_BUILDING_H__

#include "Point.h"
#include "Animation.h"
#include <map>
#include "Object.h"

struct SDL_Texture;

class Obj_Building : public Object
{
public:

	Obj_Building();
	Obj_Building(fPoint pos);
	~Obj_Building();

	bool Awake(pugi::xml_node & static_node) override;
	bool Start() override;

	void SetTexture(const char* path);

public:
	const char* path;
private:

	static SDL_Texture* texture;
};

#endif