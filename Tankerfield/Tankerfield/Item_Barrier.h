#ifndef __ITEM_BARRIER_H__
#define __ITEM_BARRIER_H__

#include "Obj_Item.h"
#include "Obj_Barrier.h"

struct SDL_Texture;

class Item_Barrier : public Obj_Item
{
public:
	Item_Barrier(fPoint pos);

	bool Update(float dt) override;

	bool Use() override;

private:
	SDL_Texture * tex = nullptr;
	Animation anim;
};

#endif