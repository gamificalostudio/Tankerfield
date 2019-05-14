#ifndef __ITEM_HEALINGAREA_H__
#define __ITEM_HEALINGAREA_H__

#include "Obj_Item.h"
#include "Timer.h"

struct SDL_Texture;

class Item_HealingArea : public Obj_Item
{
public:
	Item_HealingArea(fPoint pos);

	bool Update(float dt) override;

	bool Use() override;

	void OnTrigger(Collider * collider, float dt);

private:
	SDL_Texture * tex = nullptr;
	Animation anim;

	uint heal_by_second; 

	Timer Timer_life;
	float time_of_life_max; //Seconds

	float coll_w = 0.f; //tiles
	float coll_h = 0.f; //tiles
};

#endif 
