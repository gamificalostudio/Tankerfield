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
#include "Tesla_Trooper.h"
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"
#include <string>
#include "Obj_Tank.h"
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

	for (std::list<Object*>::iterator iterator = objects.begin(); iterator != objects.end(); iterator++)
	{
		if ((*iterator) != nullptr)
		{
			(*iterator)->Start();
		}
	}

	tesla_trooper_texture = App->tex->Load("textures/Objects/shk-sheet.png");

	return ret;
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
	std::list<Object*>::iterator iterator = objects.begin();

	while (iterator != objects.end())
	{
		if ((*iterator) != nullptr)
		{
			(*iterator)->Update(dt);
			if ((*iterator)->to_remove)
			{
				//When we remove an element from the list, the other elements shift 1 space to our position
				//So we don't need increment the iterator to go to the next one
				delete((*iterator));
				(*iterator) = nullptr;
				iterator = objects.erase(iterator);
			}
			else
			{
				++iterator;
			}
		}
		else
		{
			++iterator;
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

// Called before quitting
bool ObjectManager::CleanUp()
{
	std::list<Object*>::iterator iterator = objects.begin();

	while (iterator != objects.end())
	{
		if ((*iterator) != nullptr) {
			(*iterator)->CleanUp();
			delete (*iterator);
			(*iterator) = nullptr;
			iterator = objects.erase(iterator);
		}
	}
	objects.clear();
	return true;
}

Object* ObjectManager::CreateObject(ObjectType type, float x, float y)
{
	Object* ret = nullptr;
	switch (type)
	{
	case ObjectType::TESLA_TROOPER:
		ret = new TeslaTrooper(x, y);
		ret->type = TESLA_TROOPER;
		break;
  case ObjectType::TANK:
		ret = new Obj_Tank(x, y);
    ret->type = TANK;
		break;
	}
  
	if (ret != nullptr)
	{
		objects.push_back(ret);
	}
  
	return ret;
}


void ObjectManager::DeleteObjects()
{
	std::list<Object*>::iterator iterator = objects.begin();

	while (iterator != objects.end())
	{
		if ((*iterator) != nullptr)
		{
			(*iterator)->CleanUp();
			delete (*iterator);
			(*iterator) = nullptr;
			iterator = objects.erase(iterator);
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
