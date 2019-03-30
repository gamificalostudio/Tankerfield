#ifndef __OBJ_BASIC_SHOOT_H__
#define __OBJ_BASIC_SHOOT_H__

#include "Obj_Weapon.h"
#include "PerfTimer.h"
#include "Point.h"

struct SDL_Texture;

class Obj_BasicShoot : public Obj_Weapon
{
public:
	Obj_BasicShoot();
	Obj_BasicShoot(int x, int y);
	~Obj_BasicShoot();

public:
	bool Awake(pugi::xml_node & tank_node);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

};

#endif