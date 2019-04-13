#ifndef __M_PICKMANAGER_H__
#define __M_PICKMANAGER_H__

#include "Module.h"
#include <list>
#include "Point.h"


class Obj_PickUp;


class M_PickManager : public Module
{
public:

	M_PickManager();

	// Destructor
	virtual ~M_PickManager();

	// Called before render is available
//	bool Awake(pugi::xml_node&) override;

	// Called before the first frame
	//bool Start() override;

	//bool Update(float dt) override;

	// Called before quitting
	//bool CleanUp() override;

	//Create a Pick up random
	void CreatePickUp(fPoint pos_map);

	//Calculate if a pickUp has to be created after enemy die
	bool PickUpFromEnemy(fPoint pos_map);

public:

	//std::list<SDL_Texture*>	textures;

private:
	uint percentage_to_spawn = 100;
};



#endif // __PICKMANAGER_H__
