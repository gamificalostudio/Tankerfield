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

	bool Update(float dt) override;

	bool Use() override;

private:
	SDL_Texture * tex = nullptr;
	Animation anim;

	PerfTimer time;
};

#endif
