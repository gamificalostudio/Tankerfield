#ifndef __OBJ_PORTAL_H__
#define __OBJ_PORTAL_H__

#include "Animation.h"
#include "Object.h"
#include "PerfTimer.h"

class Item_InstantHelp;

struct SDL_Texture;

class Obj_Portal : public Object
{
public:

	Obj_Portal(fPoint pos);
	~Obj_Portal();

	bool Update(float dt) override;

	bool Awake(pugi::xml_node&) { return true; };

public:
	Item_InstantHelp * instant_help = nullptr;

private:
	// ---
	Animation anim;
	SDL_Texture * tex = nullptr;
	PerfTimer time;
};

#endif