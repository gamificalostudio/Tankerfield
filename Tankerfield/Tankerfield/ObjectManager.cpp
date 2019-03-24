//#include "Brofiler/Brofiler.h"

#include "p2Log.h"

#include "j1App.h"
#include "j1Render.h"
#include "ObjectManager.h"
#include "j1Textures.h"
#include "Object.h"
#include "j1Audio.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"
#include <string>
//#include "j1Collision.h"
//#include "j1Map.h"
//#include "Player.h"

ObjectManager::ObjectManager()
{
	name.assign("object_manager");
}

// Destructor
ObjectManager::~ObjectManager()
{

}

bool ObjectManager::Awake(pugi::xml_node& config)
{
	bool ret = true;
	std::list<Object*>::iterator iterator;

	for (iterator = objects.begin(); iterator!=objects.end(); iterator++)
	{
		if ((*iterator) != nullptr) 
			(*iterator)->Awake(config);
	}

	return ret;
}

bool ObjectManager::Start()
{
	bool ret = true;

	LoadObjectTextures();

	for (std::list<Object*>::iterator iterator = objects.begin(); iterator != objects.end(); iterator++)
	{
		if ((*iterator) != nullptr)
			(*iterator)->Start();
	}

	return ret;
}

void ObjectManager::LoadObjectTextures()
{
	//pugi::xml_node node = App->config.child("spritesheet");
	//std::string path = node.value().as_string();
	tank_base = App->tex->Load(App->config.child("object").child("spritesheets").child("tank_base").text().as_string());
}

bool ObjectManager::PreUpdate()
{
	//BROFILER_CATEGORY("EntityManager: PreUpdate", Profiler::Color::Green);
	std::list<Object*>::iterator iterator;

	for (iterator = objects.begin(); iterator != objects.end(); iterator++)
	{
		if ((*iterator) != nullptr)
		{
			(*iterator)->PreUpdate();
		}
	}

	return true;
}

// Called before render is available
bool ObjectManager::Update(float dt)
{
	//BROFILER_CATEGORY("EntityManager: Update", Profiler::Color::Green);
	std::list<Object*>::iterator iterator;

	for (iterator = objects.begin(); iterator != objects.end(); iterator++)
	{
		if ((*iterator) != nullptr)
		{
			(*iterator)->Update(dt);
		}
	}

	for (iterator = objects.begin(); iterator != objects.end(); iterator++)
	{
		if ((*iterator) != nullptr)
		{
			(*iterator)->Draw(dt);
		}
	}

	return true;
}

bool ObjectManager::PostUpdate()
{
	//BROFILER_CATEGORY("EntityManager: PostUpdate", Profiler::Color::Green);
	std::list<Object*>::iterator iterator;

	for (iterator = objects.begin(); iterator != objects.end(); iterator++)
	{
		if ((*iterator) != nullptr)
		{
			(*iterator)->PostUpdate();
		}
	}

	return true;
}

void ObjectManager::Draw(float dt)
{
}

// Called before quitting
bool ObjectManager::CleanUp()
{
	std::list<Object*>::iterator iterator;

	for (iterator = objects.begin(); iterator != objects.end(); iterator++)
	{
		if ((*iterator) != nullptr) {
			(*iterator)->CleanUp();
			delete((*iterator));
			(*iterator) = nullptr;
			objects.erase(iterator);
		}
	}

	objects.clear();

	return true;
}

Object* ObjectManager::CreateObject(ObjectType type, int x, int y)
{
	Object* ret = nullptr;
	while (ret == nullptr) {
		switch (type) {
			/*case EntityType::PLAYER: {ret = new Player();
				ret->type = PLAYER;
				break; }

			}*/
			if (ret != nullptr)
				objects.push_back(ret);
		}
		return ret;
	}
}

void ObjectManager::DeleteEntities()
{
	std::list<Object*>::iterator iterator;

	for (iterator = objects.begin(); iterator != objects.end(); iterator++)
	{
		if ((*iterator) != nullptr) {
			(*iterator)->CleanUp();
			delete((*iterator));
			(*iterator) = nullptr;
			objects.erase(iterator);
		}
	}

	objects.clear();
}


bool ObjectManager::Load(pugi::xml_node& load)
{
	bool ret = true;

	return ret;
}

bool ObjectManager::Save(pugi::xml_node& save) const
{
	bool ret = true;

	return ret;
}

Player* ObjectManager::GetPlayerData() const 
{
	std::list<Object*>::const_iterator iterator;

	for (iterator = objects.cbegin(); iterator != objects.cend(); iterator++)
	{
		if ((*iterator) != nullptr) {
			if ((*iterator)->type == PLAYER)
				return (Player*)(*iterator);
		}
	}

}




