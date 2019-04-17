#include "Brofiler/Brofiler.h"


#include "App.h"
#include "M_PickManager.h"
#include "M_ObjManager.h"
#include "M_Map.h"
#include "M_Scene.h"
#include "Obj_Tank.h"

M_PickManager::M_PickManager() : Module()
{
	name.assign("pick_manager");
}

// Destructor
M_PickManager::~M_PickManager()
{}

// Called before render is available
bool M_PickManager::Awake(pugi::xml_node& config)
{
	bool ret = true;

	percentage_to_spawn = config.child("percentage_to_spawn").attribute("value").as_int();

	return ret;
}

bool M_PickManager::Start()
{
	bool ret = true;
	for (std::vector<SpawnPoint*>::iterator iterator = app->map->data.spawners_position.begin(); iterator != app->map->data.spawners_position.end(); ++iterator)
	{
		if ((*iterator)->type == SpawnType::ENEMY)
		{
			CreateRewardBox((*iterator)->pos);
		}
	}

	return ret;
}


void M_PickManager::PickUpFromEnemy(fPoint pos_map)
{
	uint probability = rand() % 100;

	if (probability < percentage_to_spawn)
	{
		CreatePickUp(pos_map);
	}

}

void M_PickManager::CreatePickUp(fPoint pos_map, PICKUP_TYPE type_of_pick_up, uint levels_to_add)
{
	Obj_PickUp* ret = (Obj_PickUp*)app->objectmanager->CreateObject(ObjectType::PICK_UP, pos_map);

	ret->GenerationOfPickUp(type_of_pick_up, levels_to_add);

	}

void M_PickManager::CreateRewardBox(fPoint pos_map)
{
	app->objectmanager->CreateObject(ObjectType::REWARD_BOX, pos_map);
}

