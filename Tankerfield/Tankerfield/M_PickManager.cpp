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

//// Called before render is available
//bool M_Textures::Awake(pugi::xml_node& config)
//{
//	LOG("Init Image library");
//	bool ret = true;
//	// load support for the PNG image format
//	int flags = IMG_INIT_PNG;
//	int init = IMG_Init(flags);
//
//	if ((init & flags) != flags)
//	{
//		LOG("Could not initialize Image lib. IMG_Init: %s", IMG_GetError());
//		ret = false;
//	}
//
//	return ret;
//}

//// Called before the first frame
//bool M_Textures::Start()
//{
//	LOG("start textures");
//	bool ret = true;
//	return ret;
//}

//// Called before quitting
//bool M_Textures::CleanUp()
//{
//	LOG("Freeing textures and Image library");
//	std::list<SDL_Texture*>::const_iterator item;
//
//	for (item = textures.begin(); item != textures.end(); item++)
//	{
//		SDL_DestroyTexture(*item);
//	}
//
//	textures.clear();
//	IMG_Quit();
//	return true;
//}

bool M_PickManager::PickUpFromEnemy(fPoint pos_map)
{
	bool ret = false;
	uint percentage = rand() % 100;

	if (percentage < percentage_to_spawn)
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

