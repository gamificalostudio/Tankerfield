#ifndef __ITEM_INSTANTHELP_H__
#define __ITEM_INSTANTHELP_H__

#include "Obj_Item.h"
#include "PerfTimer.h"
#include "Obj_Portal.h"
#include "Object.h"
#include "Obj_Tank.h"

struct SDL_Texture;

class Item_InstantHelp : public Obj_Item
{
public:
	Item_InstantHelp(fPoint pos);

	bool Use() override;
	void Teleport(Obj_Tank* tank_to_teleport, Obj_Portal * portal_from);

public:
	Obj_Portal * portal1 = nullptr;
	Obj_Portal * portal2 = nullptr;


private:
	SDL_Texture * tex = nullptr;
	Animation anim;

	PerfTimer time;
};

#endif
