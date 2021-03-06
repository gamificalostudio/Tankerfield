#include <string>
#include <algorithm>

#include "Brofiler/Brofiler.h"
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

#include "Log.h"
#include "App.h"
#include "M_Render.h"
#include "M_ObjManager.h"
#include "M_Textures.h"
#include "M_Audio.h"
#include "M_Window.h"
#include "M_Scene.h"
#include "M_Map.h"
#include "M_Collision.h"

#include "Camera.h"

#include "Object.h"
#include "Obj_TeslaTrooper.h"
#include "Obj_Suicidal.h"
#include "Obj_Brute.h"
#include "Obj_RocketLauncher.h"
#include "Obj_Tank.h"
#include "Obj_Building.h"
#include "Bullet_Basic.h"
#include "Bullet_Missile.h"
#include "Bullet_Laser.h"
#include "Bullet_Oil.h"
#include "Obj_OilPool.h"
#include "Healing_Bullet.h"
#include "Obj_Explosion.h"
#include "Obj_HealingAnimation.h"
#include "Obj_Fire.h"
#include "Obj_Item.h"
#include "Item_HealthBag.h"
#include "Item_HappyHour.h"
#include "Item_InstantHelp.h"
#include "Obj_PickUp.h"
#include "Obj_RewardBox.h"
#include "Obj_CannonFire.h"
#include "Bullet_RocketLauncher.h"
#include "Obj_Item.h"
#include "Obj_Portal.h"
#include "Obj_ElectroShotAnimation.h"
#include "Obj_Tank_MainMenu.h"
#include "HealingShot_Area.h"
#include "Oil_Splash.h"
#include "Obj_Smoke.h"
#include "Obj_Emitter.h"
#include "Obj_SpawnPoint.h"

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

	//Open balance.xml
	std::string path = config.child_value();
	pugi::xml_parse_result result = balance_xml_doc.load_file(path.c_str());
	if (result == NULL)
	{
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	}
	else
	{
		balance_xml_node = balance_xml_doc.child("balance");
	}

	//Fill balance structs
	LoadBalanceVariables(balance_xml_node);

	//TODO: Close document

	particle_system.Awake();

	return ret;
}

bool M_ObjManager::Start()
{
	bool ret = true;

	//FillPool(ObjectType::TESLA_TROOPER, 100);
	//FillPool(ObjectType::BRUTE, 2);
	//FillPool(ObjectType::SUICIDAL, 2);
	//FillPool(ObjectType::ROCKETLAUNCHER, 2);

	particle_system.Start();

	return ret;
}

bool M_ObjManager::PreUpdate()
{
	BROFILER_CATEGORY("Object Manager: PreUpdate", Profiler::Color::Lavender);

	if (delete_all_enemies == true)
	{
		for (std::list<Object*>::iterator iterator = enemies.begin(); iterator != enemies.end(); ++iterator)
		{
			(*iterator)->return_to_pool = true;
		}
		delete_all_enemies = false;
		enemies.clear();
	}

	std::list<Object*>::iterator iterator;
	for (iterator = objects.begin(); iterator != objects.end();)
	{
		if ((*iterator) != nullptr)
		{
			if ((*iterator)->return_to_pool)
			{
				DesactivateObject(iterator);
			}
			else if ((*iterator)->to_remove)
			{
				RemoveObject(iterator);
			}
			else if ((*iterator)->active == true)
			{
				(*iterator)->PreUpdate();
				++iterator;
			}
			else
				++iterator;
		}
		else
			++iterator;

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

			if ((*iterator)->active == true)
			{
				UpdateObject(iterator, dt);
			}
			++iterator;
		}
		else
		{
			++iterator;
		}
	}
	
	return true;
}

bool M_ObjManager::IsEnemy(ObjectType type)
{
	return (type >= ObjectType::TESLA_TROOPER && type <= ObjectType::ROCKETLAUNCHER);//Change this enemy for the last enemy on the ObjectType enum
}

inline void M_ObjManager::RemoveObject(std::list<Object*>::iterator & iterator)
{
	(*iterator)->CleanUp();

	if ((*iterator)->type >= ObjectType::TESLA_TROOPER
		&& (*iterator)->type <= ObjectType::ROCKETLAUNCHER)
	{
		enemies.remove((*iterator));
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

inline void M_ObjManager::DesactivateObject(std::list<Object*>::iterator & iterator)
{
	(*iterator)->Desactivate();
	(*iterator)->return_to_pool = false;
	(*iterator)->active = false;

	if ((*iterator)->type >= ObjectType::TESLA_TROOPER && (*iterator)->type <= ObjectType::ROCKETLAUNCHER)
	{
		enemies.remove((*iterator));
	}
	if ((*iterator)->coll != nullptr)
	{
		(*iterator)->coll->SetIsTrigger(false);
	}
	ReturnToPool((*iterator));
	++iterator;
}

void M_ObjManager::DesactivateObject(Object * iterator)
{
	iterator->Desactivate();
	iterator->return_to_pool = false;
	iterator->active = false;

	if (IsEnemy(iterator->type))
	{
		enemies.remove(iterator);
	}
	if (iterator->coll != nullptr)
	{
		iterator->coll->SetIsTrigger(false);
	}
	ReturnToPool(iterator);
}

inline void M_ObjManager::UpdateObject(std::list<Object*>::iterator & iterator, const float & dt)
{
	if ((*iterator)->curr_anim != nullptr)
	{
		(*iterator)->curr_anim->NextFrame(dt);
	}

	(*iterator)->Update(dt);

	// Update Components ======================================
	if ((*iterator)->coll != nullptr)
	{
		(*iterator)->coll->SetPosToObj();
	}


}

void M_ObjManager::FillPool(ObjectType type, uint number)
{
	for (uint i = 0; i < number; ++i)
	{
		Object* ret=nullptr;
		fPoint pos(-100, -100);
		switch (type)
		{
		case ObjectType::TESLA_TROOPER:
			ret = DBG_NEW Obj_TeslaTrooper(pos);
			ret->type = ObjectType::TESLA_TROOPER;
			break;
		case ObjectType::SUICIDAL:
			ret = DBG_NEW Obj_Suicidal(pos);
			ret->type = ObjectType::SUICIDAL;
			break;
		case ObjectType::ROCKETLAUNCHER:
			ret = DBG_NEW Obj_RocketLauncher(pos);
			ret->type = ObjectType::ROCKETLAUNCHER;
			break;
		case ObjectType::TANK:
			ret = DBG_NEW Obj_Tank(pos);
			ret->type = ObjectType::TANK;
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
			ret = DBG_NEW Laser_Bullet(pos);
			ret->type = ObjectType::BULLET_LASER;
			break;
		case ObjectType::HEALING_BULLET:
			ret = DBG_NEW Healing_Bullet(pos);
			ret->type = ObjectType::HEALING_BULLET;
			break;
		case ObjectType::BULLET_OIL:
			ret = DBG_NEW Bullet_Oil(pos);
			ret->type = ObjectType::BULLET_OIL;
			break;
		case ObjectType::BULLET_ROCKETLAUNCHER:
			ret = DBG_NEW Bullet_RocketLauncher(pos);
			ret->type = ObjectType::BULLET_ROCKETLAUNCHER;
			break;
		case ObjectType::STATIC:
			ret = DBG_NEW Obj_Building(pos);
			ret->type = ObjectType::STATIC;
			break;
		case ObjectType::OIL_POOL:
			ret = DBG_NEW Obj_OilPool(pos);
			ret->type = ObjectType::OIL_POOL;
			break;
		case ObjectType::BRUTE:
			ret = DBG_NEW Obj_Brute(pos);
			ret->type = ObjectType::BRUTE;
			enemies.push_back(ret);
			break;
		case ObjectType::EXPLOSION:
			ret = DBG_NEW Obj_Explosion(pos);
			ret->type = ObjectType::EXPLOSION;
			break;
		case ObjectType::CANNON_FIRE:
			ret = DBG_NEW Obj_CannonFire(pos);
			ret->type = ObjectType::CANNON_FIRE;
			break;
		case ObjectType::HEALING_ANIMATION:
			ret = DBG_NEW Obj_Healing_Animation(pos);
			ret->type = ObjectType::HEALING_ANIMATION;
			break;
		case ObjectType::FIRE_DEAD:
			ret = DBG_NEW Obj_Fire(pos);
			ret->type = ObjectType::FIRE_DEAD;
			break;
		case ObjectType::PORTAL:
			ret = DBG_NEW Obj_Portal(pos);
			ret->type = ObjectType::PORTAL;
			break;
		case ObjectType::PICK_UP:
			ret = DBG_NEW Obj_PickUp(pos);
			ret->type = ObjectType::PICK_UP;
			break;
		case ObjectType::REWARD_BOX:
			ret = DBG_NEW Obj_RewardBox(pos);
			ret->type = ObjectType::REWARD_BOX;
			break;
		case ObjectType::TANK_MAIN_MENU:
			ret = DBG_NEW Obj_Tank_MainMenu(pos);
			ret->type = ObjectType::REWARD_BOX;
			break;
		case ObjectType::ELECTRO_SHOT_ANIMATION:
			ret = DBG_NEW Eletro_Shot_Animation(pos);
			ret->type = ObjectType::ELECTRO_SHOT_ANIMATION;
			break;
		case ObjectType::HEALING_AREA_SHOT:
			ret = DBG_NEW HealingShot_Area(pos);
			ret->type = ObjectType::HEALING_AREA_SHOT;
			break;
		case ObjectType::OIL_SPLASH:
			ret = DBG_NEW Oil_Splash(pos);
			ret->type = ObjectType::OIL_SPLASH;
			break;
		case ObjectType::DAMAGED_SMOKE:
			ret = DBG_NEW Obj_Smoke(pos);
			ret->type = ObjectType::DAMAGED_SMOKE;
			break;
		default:
			LOG("Object could not be created. Type not detected correctly or hasn't a case.");

		}
		
		if(ret!=nullptr)
			DesactivateObject(ret);
	}
}

bool M_ObjManager::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Object Manger: PostUpdate", Profiler::Color::ForestGreen);

	std::vector<Object*> draw_objects;

	for (std::list<Object*>::iterator item = objects.begin(); item != objects.end(); ++item)
	{
		if ( (*item) != nullptr && (*item)->active == true)
		{
			(*item)->CalculateDrawVariables();
		}
	}

	for (std::vector<Camera*>::iterator item_cam = app->render->cameras.begin(); item_cam != app->render->cameras.end(); ++item_cam)
	{
		SDL_RenderSetClipRect(app->render->renderer, &(*item_cam)->screen_section);

		for (std::list<Object*>::iterator item = objects.begin(); item != objects.end(); ++item)
		{
			if ((*item)->active == true && app->render->IsOnCamera((*item)->pos_screen.x - (*item)->draw_offset.x, (*item)->pos_screen.y - (*item)->draw_offset.y, (*item)->frame.w, (*item)->frame.h, (*item_cam)))
			{
				draw_objects.push_back(*item);
			}
		}

		std::sort(draw_objects.begin(), draw_objects.end(), M_ObjManager::SortByYPos);

		//Draw all the shadows first
		for (std::vector<Object*>::iterator item = draw_objects.begin(); item != draw_objects.end(); ++item)
		{
			if ((*item) != nullptr)
			{
				(*item)->DrawShadow((*item_cam), dt);
			}
		}

		//Draw the objects above the shadows
		for (std::vector<Object*>::iterator item = draw_objects.begin(); item != draw_objects.end(); ++item)
		{
			if ((*item) != nullptr)
			{
				(*item)->Draw((*item_cam));
			}
		}

		//Draw debug over the objects

		if (app->debug->debug_sprite_sorting)
		{
			for (std::vector<Object*>::iterator item = draw_objects.begin(); item != draw_objects.end(); ++item)
			{
				if ((*item) != nullptr)
				{
					(*item)->DebugSpriteSorting((*item_cam));
				}
			}
		}

		if (app->debug->debug_pathfinding)
		{
			for (std::vector<Object*>::iterator item = draw_objects.begin(); item != draw_objects.end(); ++item)
			{
				if ((*item) != nullptr)
				{
					(*item)->DebugPathfinding((*item_cam));
				}
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
	for (std::list<Object*>::iterator iterator = objects.begin(); iterator != objects.end(); ++iterator)
	{
		if ((*iterator) != nullptr)
		{
			delete (*iterator);
			(*iterator) = nullptr;
		}
	}

	objects.clear();
	obj_tanks.clear();
	enemies.clear();
	pool_of_objects.clear();

	particle_system.CleanUp();

	return true;
}

bool M_ObjManager::Reset()
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
		enemies.push_back(ret);
		break;
	case ObjectType::SUICIDAL:
		ret = DBG_NEW Obj_Suicidal(pos);
		ret->type = ObjectType::SUICIDAL;
		enemies.push_back(ret);
		break;
	case ObjectType::ROCKETLAUNCHER:
		ret = DBG_NEW Obj_RocketLauncher(pos);
		ret->type = ObjectType::ROCKETLAUNCHER;
		enemies.push_back(ret);
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
		ret = DBG_NEW Laser_Bullet(pos);
		ret->type = ObjectType::BULLET_LASER;
		break;
	case ObjectType::HEALING_BULLET:
		ret = DBG_NEW Healing_Bullet(pos);
		ret->type = ObjectType::HEALING_BULLET;
		break;
	case ObjectType::BULLET_OIL:
		ret = DBG_NEW Bullet_Oil(pos);
		ret->type = ObjectType::BULLET_OIL;
		break;
	case ObjectType::BULLET_ROCKETLAUNCHER:
		ret = DBG_NEW Bullet_RocketLauncher(pos);
		ret->type = ObjectType::BULLET_ROCKETLAUNCHER;
		break;
	case ObjectType::STATIC:
		ret = DBG_NEW Obj_Building(pos);
		ret->type = ObjectType::STATIC;
		break;
	case ObjectType::OIL_POOL:
		ret = DBG_NEW Obj_OilPool(pos);
		ret->type = ObjectType::OIL_POOL;
		break;
	case ObjectType::BRUTE:
		ret = DBG_NEW Obj_Brute(pos);
		ret->type = ObjectType::BRUTE;
		enemies.push_back(ret);
		break;
	case ObjectType::EXPLOSION:
		ret = DBG_NEW Obj_Explosion(pos);
		ret->type = ObjectType::EXPLOSION;
		break;
	case ObjectType::CANNON_FIRE:
		ret = DBG_NEW Obj_CannonFire(pos);
		ret->type = ObjectType::CANNON_FIRE;
		break;
	case ObjectType::HEALING_ANIMATION:
		ret = DBG_NEW Obj_Healing_Animation(pos);
		ret->type = ObjectType::HEALING_ANIMATION;
		break;
	case ObjectType::FIRE_DEAD:
		ret = DBG_NEW Obj_Fire(pos);
		ret->type = ObjectType::FIRE_DEAD;
		break;
	case ObjectType::PORTAL:
		ret = DBG_NEW Obj_Portal(pos);
		ret->type = ObjectType::PORTAL;
		break;
	case ObjectType::PICK_UP:
		ret = DBG_NEW Obj_PickUp(pos);
		ret->type = ObjectType::PICK_UP;
		break;
	case ObjectType::REWARD_BOX:
		ret = DBG_NEW Obj_RewardBox(pos);
		ret->type = ObjectType::REWARD_BOX;
		break;
	case ObjectType::TANK_MAIN_MENU:
		ret = DBG_NEW Obj_Tank_MainMenu(pos);
		ret->type = ObjectType::REWARD_BOX;
		break;
	case ObjectType::ELECTRO_SHOT_ANIMATION:
		ret = DBG_NEW Eletro_Shot_Animation(pos);
		ret->type = ObjectType::ELECTRO_SHOT_ANIMATION;
		break;
	case ObjectType::HEALING_AREA_SHOT:
		ret = DBG_NEW HealingShot_Area(pos);
		ret->type = ObjectType::HEALING_AREA_SHOT;
		break;
	case ObjectType::OIL_SPLASH:
		ret = DBG_NEW Oil_Splash(pos);
		ret->type = ObjectType::OIL_SPLASH;
		break;
	case ObjectType::DAMAGED_SMOKE:
		ret = DBG_NEW Obj_Smoke(pos);
		ret->type = ObjectType::DAMAGED_SMOKE;
		break;
	case ObjectType::EMITTER_FIRE:
		ret = DBG_NEW Obj_Emitter(pos, particle_system.vecEmitterData[type]);
		ret->type = ObjectType::EMITTER_FIRE;
		break;
	case ObjectType::SPAWNPOINT:
		ret = DBG_NEW Obj_SpawnPoint(pos);
		ret->type = ObjectType::SPAWNPOINT;
		break;
	default:
		LOG("Object could not be created. Type not detected correctly or hasn't a case.");

	}
	
  
	if (ret != nullptr)
	{
		ret->Start();
		objects.push_back(ret);
	}
  
	return ret;
}

Obj_Item * M_ObjManager::CreateItem(ItemType type, fPoint pos)
{
	Obj_Item* ret = nullptr;

	switch (type)
	{
	case ItemType::HEALTH_BAG:
		ret = DBG_NEW Item_HealthBag(pos);
		ret->type = ItemType::HEALTH_BAG;
		break;
	case ItemType::HAPPY_HOUR_ITEM:
		ret = DBG_NEW Item_HappyHour(pos);
		ret->type = ItemType::HAPPY_HOUR_ITEM;
		break;
	case ItemType::INSTANT_HELP:
		ret = DBG_NEW Item_InstantHelp(pos);
		ret->type = ItemType::INSTANT_HELP;
		break;
	}

	if (ret != nullptr)
	{
		objects.push_back(ret);
	}

	return ret;
}

Object * M_ObjManager::GetObjectFromPool(ObjectType type, fPoint map_pos)
{
	Object* ret = nullptr;

	std::map<ObjectType, std::list<Object*>>::iterator iterator = pool_of_objects.find(type);
	if (iterator != pool_of_objects.end())
	{
		std::list<Object*>* list = &(*iterator).second;
		if (list->size() > 0)
		{
			ret = (*list->begin());
			list->pop_front();
		}
	}
	if (ret != nullptr && type >= ObjectType::TESLA_TROOPER && type <= ObjectType::ROCKETLAUNCHER)
	{
		enemies.push_back(ret);
	}
	if (ret == nullptr)
	{
		ret = CreateObject(type, map_pos);
	}

	if (ret != nullptr)
	{
		ret->active = true;
		ret->SetMapPos(map_pos);
		ret->Start();
		return ret;
	}
	return nullptr;
}

void M_ObjManager::ReturnToPool(Object * object)
{
	std::map<ObjectType, std::list<Object*>>::iterator iter = pool_of_objects.find(object->type);
	if (iter != pool_of_objects.end())
	{
		(*iter).second.push_back(object);
	}
	else
	{
		std::list<Object*> list_obj;
		list_obj.push_back(object);
		pool_of_objects[object->type] = list_obj;
	}
}


void M_ObjManager::DeleteObjects()
{
	for (std::list<Object*>::iterator iterator = objects.begin(); iterator != objects.end(); ++iterator)
	{
		if ((*iterator) != nullptr)
		{
			(*iterator)->CleanUp();

			if ((*iterator)->coll != nullptr)
			{
				(*iterator)->coll->Destroy();
				(*iterator)->coll = nullptr;
			}
			delete (*iterator);
			(*iterator) = nullptr;
		}
	}

	objects.clear();
	obj_tanks.clear();
	enemies.clear();
	pool_of_objects.clear();
}


Obj_Tank* M_ObjManager::GetNearestTank(fPoint pos, float max_dist)
{
	Obj_Tank * closest_tank = nullptr;
	float max_dist_squared;
	if (max_dist == FLT_MAX)
	{
		max_dist_squared = FLT_MAX;//Can't get any higher value
	}
	else
	{
		max_dist_squared = max_dist * max_dist;
	}
	float lowest_distance = max_dist_squared;

	for (std::vector<Obj_Tank*>::iterator iter = obj_tanks.begin(); iter != obj_tanks.end(); ++iter)
	{
		float distance = pos.DistanceNoSqrt((*iter)->pos_map);
		if ((*iter)->Alive()
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

void M_ObjManager::LoadBalanceVariables(pugi::xml_node & balance_node)
{
	//ENEMIES

	//TESLA_TROOPER
	pugi::xml_node tesla_trooper_node = balance_node.child("enemies").child("tesla_trooper");
	tesla_trooper_info.life_multiplier			= tesla_trooper_node.child("life_multiplier").attribute("num").as_float();
	tesla_trooper_info.life_exponential_base	= tesla_trooper_node.child("life_exponential_base").attribute("num").as_float();
	tesla_trooper_info.speed					= tesla_trooper_node.child("speed").attribute("num").as_float();
	tesla_trooper_info.attack_damage			= tesla_trooper_node.child("attack_damage").attribute("num").as_int();
	tesla_trooper_info.attack_range				= tesla_trooper_node.child("attack_range").attribute("num").as_float();
	tesla_trooper_info.attack_frequency			= tesla_trooper_node.child("attack_frequency").attribute("num").as_uint();
	tesla_trooper_info.teleport_max_enemies		= tesla_trooper_node.child("teleport_max_enemies").attribute("num").as_int();
	tesla_trooper_info.detection_range			= tesla_trooper_node.child("detection_range").attribute("num").as_float();

	//BRUTE
	pugi::xml_node brute_node = balance_node.child("enemies").child("brute");
	brute_info.life_multiplier					= brute_node.child("life_multiplier").attribute("num").as_float();
	brute_info.life_exponential_base			= brute_node.child("life_exponential_base").attribute("num").as_float();
	brute_info.speed							= brute_node.child("speed").attribute("num").as_float();
	brute_info.detection_range					= brute_node.child("detection_range").attribute("num").as_float();
	brute_info.attack_damage					= brute_node.child("attack_damage").attribute("num").as_int();
	brute_info.attack_range						= brute_node.child("attack_range").attribute("num").as_float();
	brute_info.attack_frequency					= brute_node.child("attack_frequency").attribute("num").as_float();

	//ROCKET LAUNCHER
	pugi::xml_node rocket_launcher_node = balance_node.child("enemies").child("rocket_launcher");
	rocket_launcher_info.attack_damage			= rocket_launcher_node.child("attack_damage").attribute("num").as_int();
	rocket_launcher_info.attack_frequency		= rocket_launcher_node.child("attack_frequency").attribute("num").as_float();
	rocket_launcher_info.attack_range			= rocket_launcher_node.child("attack_range").attribute("num").as_float();
	rocket_launcher_info.speed					= rocket_launcher_node.child("speed").attribute("num").as_float();
	rocket_launcher_info.life_multiplier		= rocket_launcher_node.child("life_multiplier").attribute("num").as_float();
	rocket_launcher_info.life_exponential_base	= rocket_launcher_node.child("life_exponential_base").attribute("num").as_float();
	rocket_launcher_info.detection_range		= rocket_launcher_node.child("detection_range").attribute("num").as_float();

	//SUICIDAL
	pugi::xml_node suicidal_node = balance_node.child("enemies").child("suicidal");
	suicidal_info.attack_damage					= suicidal_node.child("attack_damage").attribute("num").as_int();
	suicidal_info.attack_frequency				= suicidal_node.child("attack_frequency").attribute("num").as_float();
	suicidal_info.attack_range					= suicidal_node.child("attack_range").attribute("num").as_float();
	suicidal_info.speed							= suicidal_node.child("speed").attribute("num").as_float();
	suicidal_info.life_multiplier				= suicidal_node.child("life_multiplier").attribute("num").as_float();
	suicidal_info.life_exponential_base			= suicidal_node.child("life_exponential_base").attribute("num").as_float();
	suicidal_info.detection_range				= suicidal_node.child("detection_range").attribute("num").as_float();

	//WEAPONS

	//Basic weapon
	pugi::xml_node basic_weapon_node = balance_node.child("weapons").child("basic_weapon");
	basic_weapon_info.damage_multiplier			= basic_weapon_node.child("damage_multiplier").attribute("num").as_float();
	basic_weapon_info.damage_exponential_base	= basic_weapon_node.child("damage_exponential_base").attribute("num").as_float();
	basic_weapon_info.speed	= basic_weapon_node.child("speed").attribute("num").as_float();

	//Double missile
	pugi::xml_node double_missile_node = balance_node.child("weapons").child("double_missile");
	double_missile_info.damage_multiplier = double_missile_node.child("damage_multiplier").attribute("num").as_float();
	double_missile_info.explosion_damage_multiplier = double_missile_node.child("explosion_multiplier").attribute("num").as_float();
	double_missile_info.damage_exponential_base = double_missile_node.child("damage_exponential_base").attribute("num").as_float();
	double_missile_info.speed = double_missile_node.child("speed").attribute("num").as_float();

	//Healing shot
	pugi::xml_node healing_shot_node = balance_node.child("weapons").child("healing_shot");
	healing_shot_info.damage_multiplier = healing_shot_node.child("damage_multiplier").attribute("num").as_float();
	healing_shot_info.damage_exponential_base = healing_shot_node.child("damage_exponential_base").attribute("num").as_float();

	//Laser
	pugi::xml_node laser_node = balance_node.child("weapons").child("laser");
	laser_info.damage_multiplier = laser_node.child("damage_multiplier").attribute("num").as_float();
	laser_info.damage_exponential_base = laser_node.child("damage_exponential_base").attribute("num").as_float();

	//Electro shot
	pugi::xml_node electro_shot_node = balance_node.child("weapons").child("electro_shot");
	electro_shot_info.damage_multiplier = electro_shot_node.child("damage_multiplier").attribute("num").as_float();
	electro_shot_info.damage_exponential_base = electro_shot_node.child("damage_exponential_base").attribute("num").as_float();

	//Flamethrower
	pugi::xml_node flamethrower_node = balance_node.child("weapons").child("flamethrower");
	flamethrower_info.damage_multiplier = flamethrower_node.child("damage_multiplier").attribute("num").as_float();
	flamethrower_info.damage_exponential_base = flamethrower_node.child("damage_exponential_base").attribute("num").as_float();

	//Oil weapon
	pugi::xml_node oil_weapon_node = balance_node.child("weapons").child("oil_weapon");
	oil_weapon_info.damage_multiplier = oil_weapon_node.child("damage_multiplier").attribute("num").as_float();
	oil_weapon_info.damage_exponential_base = oil_weapon_node.child("damage_exponential_base").attribute("num").as_float();
	oil_weapon_info.speed = oil_weapon_node.child("speed").attribute("num").as_float();
}