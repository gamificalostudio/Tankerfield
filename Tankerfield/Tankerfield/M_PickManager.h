#ifndef __M_PICKMANAGER_H__
#define __M_PICKMANAGER_H__

#include "Module.h"
#include <list>
#include "Point.h"
#include "Obj_PickUp.h"


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
	void CreatePickUp(fPoint pos_map, PICKUP_TYPE type_of_pick_up = PICKUP_TYPE::NO_TYPE, uint levels_to_add = NULL);

	//Create a reward box
	void CreateRewardBox(fPoint pos_map);

	//Calculate if a pickUp has to be created after enemy die
	void PickUpFromEnemy(fPoint pos_map);



private:
	uint percentage_to_spawn = NULL;
};



#endif // __PICKMANAGER_H__
