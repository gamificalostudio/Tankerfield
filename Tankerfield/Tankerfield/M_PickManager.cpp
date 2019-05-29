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

void M_PickManager::CreatePickUp(fPoint pos_map, PICKUP_TYPE type_of_pick_up, ItemType item_type, WEAPON weapon_type, uint levels_to_add)
{
	Obj_PickUp* ret = (Obj_PickUp*)app->objectmanager->CreateObject(ObjectType::PICK_UP, pos_map);

	if (type_of_pick_up == PICKUP_TYPE::NO_TYPE)
	{
		type_of_pick_up = RandomPickUp();
	}

	if (type_of_pick_up == PICKUP_TYPE::WEAPON)
	{
		if (weapon_type == WEAPON::MAX_WEAPONS)
		{
			ret->type_of_weapon = RandomWeapon();
		}
		else
		{
			ret->type_of_weapon = weapon_type;
		}
		ret->level_of_weapon += app->scene->round + levels_to_add;
	}
	else if (type_of_pick_up == PICKUP_TYPE::ITEM)
	{
		if (item_type == ItemType::MAX_ITEMS)
		{
			ret->type_of_item = RandomItem();
		}
		else
		{
			ret->type_of_item = item_type;
		}
	}

	ret->CreatePickUpUI(type_of_pick_up);
}

PICKUP_TYPE M_PickManager::RandomPickUp() const
{
	return (PICKUP_TYPE)(rand() % (uint)PICKUP_TYPE::MAX_TYPES);
}

WEAPON M_PickManager::RandomWeapon() const
{
	return (WEAPON)(rand() % ((uint)WEAPON::MAX_WEAPONS - 1) + 1 /*The plus 1 is because the basic shoot is the number 0, and it can't be created, and the -1 is because the max weapon include the basic bullet*/);
}

ItemType M_PickManager::RandomItem() const
{
	return (ItemType)(rand() % (uint)ItemType::MAX_ITEMS);
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

