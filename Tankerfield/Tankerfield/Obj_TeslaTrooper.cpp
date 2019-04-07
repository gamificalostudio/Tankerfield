#include <list>

//#include "Brofiler/Brofiler.h"
#include "PugiXml\src\pugixml.hpp"

#include "Point.h"
#include "Log.h"
#include "Animation.h"

#include "App.h"
#include "Object.h"
#include "M_Textures.h"
#include "M_ObjManager.h"
#include "M_Render.h"
#include "M_Scene.h"
#include "M_Pathfinding.h"
#include "Obj_TeslaTrooper.h"
#include "M_Input.h"
#include "M_Map.h"



//Static variables inicialization
SDL_Texture * Obj_TeslaTrooper::tex = nullptr;
Animation * Obj_TeslaTrooper::walk = nullptr;

Obj_TeslaTrooper::Obj_TeslaTrooper(fPoint pos) : Object (pos)
{}

bool Obj_TeslaTrooper::Start()
{
	pugi::xml_node tesla_trooper_node = app->config.child("object").child("tesla_trooper");

	if (tex == nullptr)
	{
		tex = app->tex->Load("textures/Objects/shk-sheet.png");
	}
	curr_tex = tex;
  
	if (walk == nullptr)
	{
		walk = new Animation;
		walk->LoadAnimation(tesla_trooper_node.child("animations").child("walk"));
	}
	curr_anim = walk;

	speed = 1.5f;
	range_pos.center = pos_map;
	range_pos.radius = 0.2;

	timer.Start();

	return true;
}

bool Obj_TeslaTrooper::Update(float dt)
{
	if(app->input->GetKey(SDL_SCANCODE_W)==KEY_DOWN)
	{
		angle += 45;
	}
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
	{
		angle -= 45;
	}

  
	//if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
	//{
	//	life -= 100;
	//	LOG("life: %i", life);
	//}

	//if (life <= 0)
	//{
	//	death = true;
	//	to_remove = true;
	//}

	if (timer.ReadSec() >= 5)
	{
		target = app->objectmanager->GetNearestTank(pos_map);
		if (target != nullptr)
		{
			//iPoint origin = app->map->ScreenToMapI(pos_map.x, pos_map.y);
			//iPoint destination = app->map->ScreenToMapI(target->pos_map.x, target->pos_map.y);
			if (app->pathfinding->CreatePath((iPoint)pos_map, (iPoint)target->pos_map) != -1)
			{
				path = *app->pathfinding->GetLastPath();
				if (path.size() > 0)
					next_pos = (fPoint)(*path.begin());
			}
		}
		timer.Start();
	}

	if (path.size() > 0)
	{
		if (IsOnGoal(next_pos))
		{
			path.erase(path.begin());
			if (path.size() > 0)
				next_pos = (fPoint)(*path.begin());
		}
		else
		{
			fPoint move_vect = (fPoint)(next_pos)-pos_map;
			move_vect.Normalize();

			pos_map += move_vect * speed * dt;
			range_pos.center = pos_map;
		}
	
	}
	return true;
}

bool Obj_TeslaTrooper::IsOnGoal(fPoint goal)
{
	return range_pos.IsPointIn(goal);
}





