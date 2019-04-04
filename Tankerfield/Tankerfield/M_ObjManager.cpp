#include "Brofiler/Brofiler.h"

#include "Log.h"

#include "App.h"
#include "M_Render.h"
#include "M_ObjManager.h"
#include "M_Textures.h"
#include "Object.h"
#include "M_Audio.h"
#include "M_Window.h"
#include "M_Scene.h"
#include "Obj_TeslaTrooper.h"
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"
#include <string>
#include <algorithm>
#include "Obj_Tank.h"
#include "Obj_Static.h"
#include "Bullet_Basic.h"
#include "Brofiler/Brofiler.h"

M_ObjManager::M_ObjManager()
{
	name.assign("object_manager");
}

// Destructor
M_ObjManager::~M_ObjManager()
{

}

bool M_ObjManager::Awake(pugi::xml_node& config)
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

bool M_ObjManager::Start()
{
	bool ret = true;
	return true;
}

bool M_ObjManager::PreUpdate()
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
bool M_ObjManager::Update(float dt)
{
	BROFILER_CATEGORY("EntityManager: Update", Profiler::Color::ForestGreen);

	for (std::list<Object*>::iterator iterator = objects.begin(); iterator != objects.end();)
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

bool M_ObjManager::UpdateAll(float dt)
{
	bool ret = true;

	//TODO 4: Save entities on camera during update iteration in draw_entities vector and iterate after update iteration
	//move draw function, entities_drawn++ and DrawDebugQuad to the new iteration

	std::list<Object*> draw_objects;
	uint objects_drawn = 0;

	for (std::list<Object*>::iterator item = objects.begin(); item != objects.end(); ++item) {
		if (*item != nullptr) {
			ret = (*item)->Update(dt);

			if (app->render->IsOnCamera((*item)->pos_map.x, (*item)->pos_map.y, (*item)->size.x, (*item)->size.y)) {
				draw_objects.push_back(*item);
			}
		}
	}

	//std::sort(draw_objects.begin(), draw_objects.end(), M_ObjManager::SortByYPos);

	for (std::list<Object*>::iterator item = draw_objects.begin(); item != draw_objects.end(); ++item) {
		//(*item)->Draw();
		objects_drawn++;
	}

	draw_objects.clear();

	return ret;
}

bool M_ObjManager::PostUpdate(float dt)
{
	BROFILER_CATEGORY("EntityManager: PostUpdate", Profiler::Color::ForestGreen);
	std::list<Object*>::iterator iterator;

	for (iterator = objects.begin(); iterator != objects.end(); iterator++)
	{
		if ((*iterator) != nullptr)
		{
			(*iterator)->PostUpdate(dt);
		}
	}

	return true;
}

// Called before quitting
bool M_ObjManager::CleanUp()
{
	for (std::list<Object*>::iterator iterator = objects.begin(); iterator != objects.end(); ++iterator)
	{
		if ((*iterator) != nullptr) 
		{
			(*iterator)->CleanUp();
			delete (*iterator);
			(*iterator) = nullptr;
		}
	}
	
	objects.clear();
	return true;
}

Object* M_ObjManager::CreateObject(ObjectType type, fPoint pos)
{
	Object* ret = nullptr;
	switch (type)
	{
	case ObjectType::TESLA_TROOPER:
		ret = new Obj_TeslaTrooper(pos);
		ret->type = TESLA_TROOPER;
		break;
	case ObjectType::TANK:
		ret = new Obj_Tank(pos);
		ret->type = TANK;
		break;
	case ObjectType::BASIC_BULLET:
		ret = new Bullet_Basic(pos);
		ret->type = BASIC_BULLET;
		break;
	case ObjectType::STATIC:
		ret = new Obj_Static(pos);
		ret->type = STATIC;
		break;
	}
  
	if (ret != nullptr)
	{
		ret->Start();
		objects.push_back(ret);
	}
  
	return ret;
}


void M_ObjManager::DeleteObjects()
{
	for (std::list<Object*>::iterator iterator = objects.begin(); iterator != objects.end(); ++iterator)
	{
		if ((*iterator) != nullptr)
		{
			(*iterator)->CleanUp();
			delete (*iterator);
			(*iterator) = nullptr;
		}
	}

	objects.clear();
}

bool M_ObjManager::Load(pugi::xml_node& load)
{
	bool ret = true;

	return ret;
}

bool M_ObjManager::Save(pugi::xml_node& save) const
{
	bool ret = true;

	return ret;
}


bool M_ObjManager::SortByYPos(const Object * ent1, const Object * ent2)
{
	return ent1->pivot.y + ent1->pos_map.y < ent2->pivot.y + ent2->pos_map.y;
}