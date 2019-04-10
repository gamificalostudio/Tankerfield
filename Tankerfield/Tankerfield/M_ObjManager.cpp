
#include <string>

#include "Brofiler/Brofiler.h"
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

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
#include "Obj_Brute.h"
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"
#include <string>
#include <algorithm>
#include "Obj_Tank.h"
#include "Obj_Static.h"
#include "Bullet_Basic.h"
#include "M_Map.h"
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
	return ret;
}

bool M_ObjManager::PreUpdate()
{
	BROFILER_CATEGORY("EntityManager: PreUpdate", Profiler::Color::Lavender);
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
				if ((*iterator)->type == ObjectType::TANK)
					obj_tanks.erase(iterator);

				if ((*iterator)->coll != nullptr)
				{
					(*iterator)->coll->Destroy();
				}


				delete((*iterator));
				(*iterator) = nullptr;
				iterator = objects.erase(iterator);
			}
			else
			{
				// Update Components ======================================

				if ((*iterator)->coll != nullptr)
				{
					(*iterator)->coll->SetPosToObj();
				}

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

bool M_ObjManager::PostUpdate(float dt)
{
	BROFILER_CATEGORY("EntityManager: PostUpdate", Profiler::Color::ForestGreen);

	std::vector<Object*> draw_objects;

	for (std::list<Object*>::iterator item = objects.begin(); item!= objects.end(); ++item)
	{
		if (*item != nullptr)
		{
			(*item)->CalculateDrawVariables();
			if (app->render->IsOnCamera((*item)->pos_screen.x - (*item)->draw_offset.x, (*item)->pos_screen.y - (*item)->draw_offset.y, (*item)->frame.w, (*item)->frame.h))
			{
				draw_objects.push_back(*item);
			}
		}
	}

	std::sort(draw_objects.begin(), draw_objects.end(), M_ObjManager::SortByYPos);

	//Draw all the shadows first
	for (std::vector<Object*>::iterator item = draw_objects.begin(); item != draw_objects.end(); ++item)
	{
		(*item)->DrawShadow();
	}

	//Draw the objects above the shadows
	for (std::vector<Object*>::iterator item = draw_objects.begin(); item != draw_objects.end(); ++item)
	{
		(*item)->Draw(dt);

		if (app->scene->draw_debug) {
			(*item)->DrawDebug();
		}
	}

	draw_objects.clear();

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
		ret->type = ObjectType::TESLA_TROOPER;
		break;
	case ObjectType::TANK:
		ret = new Obj_Tank(pos);
		ret->type = ObjectType::TANK;
		obj_tanks.push_back(ret);
		break;
	case ObjectType::BASIC_BULLET:
		ret = new Bullet_Basic(pos);
		ret->type = ObjectType::BASIC_BULLET;
		break;
	case ObjectType::STATIC:
		ret = new Obj_Static(pos);
		ret->type = ObjectType::STATIC;
		break;
	case ObjectType::REWARD_ZONE:
		ret = new Reward_Zone(pos);
		ret->type = ObjectType::REWARD_ZONE;
	case ObjectType::BRUTE:
		ret = new Obj_Brute(pos);
		ret->type = ObjectType::BRUTE;
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
		(*iterator)->to_remove = true;
	}
}

Object * M_ObjManager::GetNearestTank(fPoint pos)
{
	Object* ret = (*obj_tanks.begin());
	if (ret != nullptr)
	{
		float distance = pos.DistanceTo(ret->pos_map);
		for (std::list<Object*>::iterator iter = obj_tanks.begin(); iter != obj_tanks.end(); ++iter)
		{
			float new_distance = pos.DistanceTo((*iter)->pos_map);
			if (new_distance  < distance)
			{
				distance = new_distance;
				ret = *iter;
			}
		}
	}
	
	return ret;
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


bool M_ObjManager::SortByYPos(Object * obj1, Object * obj2)
{
	return obj1->pivot.y + obj1->pos_screen.y < obj2->pivot.y + obj2->pos_screen.y;
}