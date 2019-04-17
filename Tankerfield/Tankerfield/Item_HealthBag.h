#ifndef __ITEM_HEALTHBAG_H__
#define __ITEM_HEALTHBAG_H__

#include "Obj_Item.h"

struct SDL_Texture;

class Item_HealthBag : public Obj_Item
{
public:
	Item_HealthBag(fPoint pos);

	bool Update(float dt) override;

	bool Use() override;

private:
	SDL_Texture * tex = nullptr;
	Animation anim;
};

#endif