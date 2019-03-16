#include "j1App.h"
#include "j1Render.h"
#include "EntityManager.h"
#include "j1Textures.h"
#include "Entity.h"
#include "j1Audio.h"
#include "j1Window.h"
//#include "j1Collision.h"
#include "p2Log.h"
//#include "j1Map.h"
#include "j1Scene.h"
//#include "Player.h"

//#include "Brofiler/Brofiler.h"

EntityManager::EntityManager()
{
	name.assign("entity_manager");
}

// Destructor
EntityManager::~EntityManager()
{

}

bool EntityManager::Awake(pugi::xml_node& config)
{
	bool ret = true;
	std::list<Entity*>::iterator iterator;

	for (iterator = entities.begin(); iterator!=entities.end(); iterator++)
	{
		if ((*iterator) != nullptr) 
			(*iterator)->Awake(config);
	}

	return ret;
}

bool EntityManager::Start()
{
	bool ret = true;	
	std::list<Entity*>::iterator iterator;

	for (iterator = entities.begin(); iterator != entities.end(); iterator++)
	{
		if ((*iterator) != nullptr)
			(*iterator)->Start();
	}

	return ret;
}

bool EntityManager::PreUpdate()
{
	//BROFILER_CATEGORY("EntityManager: PreUpdate", Profiler::Color::Green);
	std::list<Entity*>::iterator iterator;

	for (iterator = entities.begin(); iterator != entities.end(); iterator++)
	{
		if ((*iterator) != nullptr)
			(*iterator)->PreUpdate();
	}

	return true;
}

// Called before render is available
bool EntityManager::Update(float dt)
{
	//BROFILER_CATEGORY("EntityManager: Update", Profiler::Color::Green);
	std::list<Entity*>::iterator iterator;

	for (iterator = entities.begin(); iterator != entities.end(); iterator++)
	{
		if ((*iterator) != nullptr)
			(*iterator)->Update(dt);
	}

	for (iterator = entities.begin(); iterator != entities.end(); iterator++)
	{
		if ((*iterator) != nullptr)
			(*iterator)->Draw(dt);
	}

	return true;
}

bool EntityManager::PostUpdate()
{
	//BROFILER_CATEGORY("EntityManager: PostUpdate", Profiler::Color::Green);
	std::list<Entity*>::iterator iterator;

	for (iterator = entities.begin(); iterator != entities.end(); iterator++)
	{
		if ((*iterator) != nullptr)
			(*iterator)->PostUpdate();
	}

	return true;
}

void EntityManager::Draw(float dt)
{
}

// Called before quitting
bool EntityManager::CleanUp()
{
	std::list<Entity*>::iterator iterator;

	for (iterator = entities.begin(); iterator != entities.end(); iterator++)
	{
		if ((*iterator) != nullptr) {
			(*iterator)->CleanUp();
			delete((*iterator));
			(*iterator) = nullptr;
			entities.erase(iterator);
		}
	}

	entities.clear();

	return true;
}

Entity* EntityManager::CreateEntity(EntityType type, int x, int y)
{
	Entity* ret = nullptr;
	while (ret == nullptr) {
		switch (type) {
			/*case EntityType::PLAYER: {ret = new Player();
				ret->type = PLAYER;
				break; }

			}*/
			if (ret != nullptr)
				entities.push_back(ret);
		}
		return ret;
	}
}

void EntityManager::DeleteEntities()
{
	std::list<Entity*>::iterator iterator;

	for (iterator = entities.begin(); iterator != entities.end(); iterator++)
	{
		if ((*iterator) != nullptr) {
			(*iterator)->CleanUp();
			delete((*iterator));
			(*iterator) = nullptr;
			entities.erase(iterator);
		}
	}

	entities.clear();
}


bool EntityManager::Load(pugi::xml_node& load)
{
	bool ret = true;

	return ret;
}

bool EntityManager::Save(pugi::xml_node& save) const
{
	bool ret = true;

	return ret;
}

Player* EntityManager::GetPlayerData() const 
{
	std::list<Entity*>::const_iterator iterator;

	for (iterator = entities.cbegin(); iterator != entities.cend(); iterator++)
	{
		if ((*iterator) != nullptr) {
			if ((*iterator)->type == PLAYER)
				return (Player*)(*iterator);
		}
	}

}




