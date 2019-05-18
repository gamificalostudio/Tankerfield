#ifndef __BULLET_OIL_H__
#define __BULLET_OIL_H__

#include "PerfTimer.h"
#include "Point.h"
#include "Obj_Bullet.h"

struct SDL_Texture;

class Bullet_Oil : public Obj_Bullet
{
public:

	Bullet_Oil(fPoint pos);
	~Bullet_Oil();

public:

	//Add functions that are overriden
	bool Start();

};

#endif