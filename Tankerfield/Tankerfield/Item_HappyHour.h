#ifndef __ITEM_HAPPYHOUR_H__
#define __ITEM_HAPPYHOUR_H__

#include "Obj_Item.h"
#include "PerfTimer.h"
struct SDL_Texture;

class Item_HappyHour : public Obj_Item
{
public:
	Item_HappyHour(fPoint pos);

	bool Update(float dt) override;

	bool Use() override;

private:
	SDL_Texture * tex = nullptr;
	Animation anim;

	int original_time_between_bullets = 0;
	int happy_time_between_bullets = 0;

	PerfTimer time;
};

#endif