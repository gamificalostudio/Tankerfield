#ifndef __M_PICKMANAGER_H__
#define __M_PICKMANAGER_H__

#include "Module.h"
#include <list>
#include "Point.h"
#include "Obj_PickUp.h"

class Obj_RewardBox;

class M_PickManager : public Module
{
public:

	M_PickManager();

	// Destructor
	virtual ~M_PickManager();

	// Called before render is available
	bool Awake(pugi::xml_node&) override;

	bool Start();

	//Create a Pick up 
	//Put type_of_pickup = PICKUP_TYPE::NO_TYPE, weapon = WEAPON::MAX_WEAPONS and ItemType item_type = ItemType::MAX_ITEM
	void CreatePickUp(fPoint pos_map, PICKUP_TYPE type_of_pick_up = PICKUP_TYPE::NO_TYPE, ItemType item_type = ItemType::MAX_ITEMS, WEAPON weapon_type = WEAPON::MAX_WEAPONS, uint levels_to_add = 0u);

	//Create a reward box
	Obj_RewardBox* CreateRewardBox(fPoint pos_map);

	//Calculate if a pickUp has to be created after enemy die
	void PickUpFromEnemy(fPoint pos_map, PICKUP_TYPE type_of_pick_up = PICKUP_TYPE::MAX_TYPES);

	void CreateRewardBoxWave();

	PICKUP_TYPE RandomPickUp() const;
	WEAPON RandomWeapon() const;
	ItemType RandomItem() const;

private:
	uint percentage_spawn_item_from_enemy = NULL;
	uint percentage_spawn_reward_box = NULL;
};



#endif // __PICKMANAGER_H__
