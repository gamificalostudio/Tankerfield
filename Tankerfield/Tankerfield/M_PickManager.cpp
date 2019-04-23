#include "Brofiler/Brofiler.h"


#include "App.h"
#include "M_PickManager.h"
#include "M_ObjManager.h"
#include "M_Map.h"
#include "M_Scene.h"
#include "Obj_Tank.h"
#include "Obj_RewardBox.h"

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

	percentage_spawn_item_from_enemy = config.child("percentage_spawn_item_from_enemy").attribute("value").as_int(0);
	percentage_spawn_reward_box = config.child("percentage_spawn_reward_box").attribute("value").as_int(0);
	return ret;
}

bool M_PickManager::Start()
{
	bool ret = true;

	return ret;
}


void M_PickManager::PickUpFromEnemy(fPoint pos_map, PICKUP_TYPE type_of_pick_up)
{
	uint probability = rand() % 100;
	if (type_of_pick_up == PICKUP_TYPE::MAX_TYPES) // if you do not pass a type of pickUp, create an item with the probability defined in the config.
	{
		if (probability < percentage_spawn_item_from_enemy)
		{
			CreatePickUp(pos_map, PICKUP_TYPE::ITEM);
		}
	}
	else //if you pass a type of pickUp, create it directly. For direct rewards.
	{
		CreatePickUp(pos_map, type_of_pick_up);
	}
}

void M_PickManager::CreatePickUp(fPoint pos_map, PICKUP_TYPE type_of_pick_up, uint levels_to_add)
{
	Obj_PickUp* ret = (Obj_PickUp*)app->objectmanager->CreateObject(ObjectType::PICK_UP, pos_map);

	ret->GenerationOfPickUp(type_of_pick_up, levels_to_add);

}

Obj_RewardBox* M_PickManager::CreateRewardBox(fPoint pos_map)
{
	return (Obj_RewardBox*)app->objectmanager->CreateObject(ObjectType::REWARD_BOX, pos_map);
}

void M_PickManager::CreateRewardBoxWave()
{
	for (std::vector<SpawnPoint*>::iterator iterator = app->map->data.spawners_position_reward_box.begin(); iterator != app->map->data.spawners_position_reward_box.end(); ++iterator)
	{
		if (!(*iterator)->occupied)
		{
			uint probability = rand() % 100;
			if (probability < percentage_spawn_reward_box)
			{
				Obj_RewardBox* ret = CreateRewardBox((*iterator)->pos);
				ret->my_spawn_point = (*iterator);
				(*iterator)->occupied = true;
			}
		}			
	}
}

