#ifndef __BULLET_BASIC_H__
#define __BULLET_BASIC_H__

#include "PerfTimer.h"
#include "Point.h"
#include "Obj_Bullet.h"

struct SDL_Texture;

class Bullet_Basic : public Obj_Bullet
{
public:
	Bullet_Basic(fPoint pos);
	~Bullet_Basic();

public:
	//Add functions that are overriden
};

#endif