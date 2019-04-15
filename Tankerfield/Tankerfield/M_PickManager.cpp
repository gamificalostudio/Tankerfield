#include "Brofiler/Brofiler.h"


#include "App.h"
#include "M_PickManager.h"
#include "M_ObjManager.h"


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


bool M_PickManager::PickUpFromEnemy(fPoint pos_map)
{
	bool ret = false;
	uint probability = rand() % 100;

	if (probability < percentage_to_spawn)
	{
		CreatePickUp(pos_map);
		ret = true;
	}

	return ret;
}

void M_PickManager::CreatePickUp(fPoint pos_map)
{
	app->objectmanager->CreateObject(ObjectType::PICK_UP, pos_map);
}

