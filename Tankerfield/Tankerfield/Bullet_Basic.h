#ifndef __BULLET_BASIC_H__
#define __BULLET_BASIC_H__

#include "PerfTimer.h"
#include "Point.h"
#include "Bullet.h"

struct SDL_Texture;

class Bullet_Basic : public Obj_Bullet
{
public:
	Bullet_Basic();
	Bullet_Basic(int x, int y);
	~Bullet_Basic();

public:
	bool Awake(pugi::xml_node & tank_node);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
};

#endif