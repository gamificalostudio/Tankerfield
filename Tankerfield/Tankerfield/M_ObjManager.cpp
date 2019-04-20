
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
#include "Obj_Building.h"
#include "Bullet_Basic.h"
#include "Bullet_Missile.h"
#include "Bullet_Laser.h"
#include "Healing_Bullet.h"
#include "Obj_Explosion.h"
#include "Obj_HealingAnimation.h"
#include "M_Map.h"
#include "Brofiler/Brofiler.h"
#include "Obj_Item.h"
#include "Item_HealthBag.h"
#include "Item_HappyHour.h"
#include "Obj_PickUp.h"
#include "Obj_RewardBox.h"

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
	BROFILER_CATEGORY("Object Manager: PreUpdate", Profiler::Color::Lavender);
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
	BROFILER_CATEGORY("Object Manager: Update", Profiler::Color::ForestGreen);

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
				{
					Obj_Tank* aux = (Obj_Tank*)(*iterator);
					obj_tanks.remove((Obj_Tank*)(*iterator));
				}

				if ((*iterator)->coll != nullptr)
				{
					(*iterator)->coll->object = nullptr;
					(*iterator)->coll->Destroy();
					(*iterator)->coll = nullptr;
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

				if ((*iterator)->curr_anim != nullptr)
				{
					(*iterator)->curr_anim->NextFrame(dt);
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
	BROFILER_CATEGORY("Object Manger: PostUpdate", Profiler::Color::ForestGreen);
	std::vector<Object*> draw_objects;

	for (std::list<Object*>::iterator item = objects.begin(); item != objects.end(); ++item)
	{
		if (*item != nullptr)
		{
			(*item)->CalculateDrawVariables();
		}
	}

	for (std::vector<Camera*>::iterator item_cam = app->render->cameras.begin(); item_cam != app->render->cameras.end(); ++item_cam)
	{
		SDL_RenderSetClipRect(app->render->renderer, &(*item_cam)->viewport);

		for (std::list<Object*>::iterator item = objects.begin(); item != objects.end(); ++item)
		{
			if (app->render->IsOnCamera((*item)->pos_screen.x - (*item)->draw_offset.x, (*item)->pos_screen.y - (*item)->draw_offset.y, (*item)->frame.w, (*item)->frame.h, (*item_cam)))
			{
				draw_objects.push_back(*item);
			}
		}

		std::sort(draw_objects.begin(), draw_objects.end(), M_ObjManager::SortByYPos);

		//Draw all the shadows first
		for (std::vector<Object*>::iterator item = draw_objects.begin(); item != draw_objects.end(); ++item)
		{
		  (*item)->DrawShadow((*item_cam));
		}

		//Draw the objects above the shadows
		for (std::vector<Object*>::iterator item = draw_objects.begin(); item != draw_objects.end(); ++item)
		{
		  (*item)->Draw(dt, (*item_cam));

		  if (app->scene->draw_debug) {
			  (*item)->DrawDebug((*item_cam));
			  DrawDebug((*item), (*item_cam));
		  }
		}
		draw_objects.clear();
    }
	SDL_RenderSetClipRect(app->render->renderer, nullptr);
   
	return true;
}

// Called before quitting
bool M_ObjManager::CleanUp()
{
	DeleteObjects();

	return true;
}

Object* M_ObjManager::CreateObject(ObjectType type, fPoint pos)
{
	Object* ret = nullptr;
	switch (type)
	{
	case ObjectType::TESLA_TROOPER:
		ret = DBG_NEW Obj_TeslaTrooper(pos);
		ret->type = ObjectType::TESLA_TROOPER;
		break;
	case ObjectType::TANK:
		ret = DBG_NEW Obj_Tank(pos);
		ret->type = ObjectType::TANK;
		obj_tanks.push_back((Obj_Tank*)ret);
		break;
	case ObjectType::BASIC_BULLET:
		ret = DBG_NEW Bullet_Basic(pos);
		ret->type = ObjectType::BASIC_BULLET;
		break;
	case ObjectType::BULLET_MISSILE:
		ret = DBG_NEW Bullet_Missile(pos);
		ret->type = ObjectType::BULLET_MISSILE;
		break;
	case ObjectType::BULLET_LASER:
		ret = new Laser_Bullet(pos);
		ret->type = ObjectType::BULLET_LASER;
		break;
	case ObjectType::HEALING_BULLET:
		ret = new Healing_Bullet(pos);
		ret->type = ObjectType::HEALING_BULLET;
		break;
	case ObjectType::STATIC:
		ret = DBG_NEW Obj_Building(pos);
		ret->type = ObjectType::STATIC;
		break;
	case ObjectType::REWARD_ZONE:
		ret = DBG_NEW Reward_Zone(pos);
		ret->type = ObjectType::REWARD_ZONE;
		break;
	case ObjectType::BRUTE:
		ret = new Obj_Brute(pos);
		ret->type = ObjectType::BRUTE;
		break;
	case ObjectType::EXPLOSION:
		ret = DBG_NEW Obj_Explosion(pos);
		ret->type = ObjectType::EXPLOSION;
		break;
	case ObjectType::HEALING_ANIMATION:
		ret = new Obj_Healing_Animation(pos);
		ret->type = ObjectType::HEALING_ANIMATION;
		break;
	case ObjectType::HEALTH_BAG:
		ret = DBG_NEW Item_HealthBag(pos);
		ret->type = ObjectType::HEALTH_BAG;
		break;
	case ObjectType::HAPPY_HOUR_ITEM:
		ret = new Item_HappyHour(pos);
		ret->type = ObjectType::HAPPY_HOUR_ITEM;
		break;
	case ObjectType::PICK_UP:
		ret = DBG_NEW Obj_PickUp(pos);
		ret->type = ObjectType::PICK_UP;
		break;
	case ObjectType::REWARD_BOX:
		ret = new Obj_RewardBox(pos);
		ret->type = ObjectType::REWARD_BOX;
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
	obj_tanks.clear();
}


Obj_Tank* M_ObjManager::GetNearestTank(fPoint pos, float max_dist)
{
	Obj_Tank * closest_tank = nullptr;
	float max_dist_squared;
	if (max_dist == FLOAT_MAX)
	{
		max_dist_squared = FLOAT_MAX;//Can't get any higher value
	}
	else
	{
		max_dist_squared = max_dist * max_dist;
	}
	float lowest_distance = max_dist_squared;

	for (std::list<Obj_Tank*>::iterator iter = obj_tanks.begin(); iter != obj_tanks.end(); ++iter)
	{
		float distance = pos.DistanceNoSqrt((*iter)->pos_map);
		if ((*iter)->GetLife() > 0
			&& distance < max_dist_squared
			&& distance < lowest_distance)
		{
			closest_tank = (*iter);
			lowest_distance = distance;
		}
	}

	return closest_tank;
}

std::list<Object*> M_ObjManager::GetObjects() const
{
	return this->objects;
}

void M_ObjManager::DrawDebug(const Object* obj, Camera* camera)
{
	SDL_Rect section = { obj->pos_screen.x - obj->draw_offset.x, obj->pos_screen.y - obj->draw_offset.y, obj->frame.w, obj->frame.h };

	Uint8 alpha = 0;
	switch (obj->type)
	{
	case ObjectType::TANK:
		app->render->DrawQuad(section, 255, 0, 0, alpha);
		break;
	case ObjectType::STATIC:
		app->render->DrawQuad(section, 0, 255, 0, alpha);
		break;
	case ObjectType::TESLA_TROOPER:
		app->render->DrawQuad(section, 0, 0, 255, alpha);
		break;
	case ObjectType::EXPLOSION:
		app->render->DrawQuad(section, 255, 0, 255, alpha);
	default:
		break;
	}

	app->render->DrawCircle(obj->pos_screen.x + obj->pivot.x, obj->pos_screen.y + obj->pivot.y, 3, camera, 0, 255, 0);
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