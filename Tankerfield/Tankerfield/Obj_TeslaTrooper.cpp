#include <list>
#include <assert.h>
#include <vector>

#include "Brofiler/Brofiler.h"
#include "PugiXml\src\pugixml.hpp"

#include "Point.h"
#include "Log.h"
#include "Animation.h"

#include "App.h"
#include "Object.h"
#include "Obj_TeslaTrooper.h"
#include "M_Textures.h"
#include "M_ObjManager.h"
#include "M_Render.h"
#include "M_Scene.h"
#include "M_Pathfinding.h"
#include "M_Input.h"
#include "Animation.h"
#include "M_Map.h"
#include "M_Collision.h"
#include "WeaponInfo.h"

//Static variables inicialization
SDL_Texture * Obj_TeslaTrooper::tex = nullptr;
Animation * Obj_TeslaTrooper::walk = nullptr;

Obj_TeslaTrooper::Obj_TeslaTrooper(fPoint pos) : Object (pos)
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
	speed				= 1.5F;
	range_pos.center	= pos_map;
	range_pos.radius	= 0.5f;
	follow_range		= 10.0f;
	check_path_time		= 1.f;
	coll				= app->collision->AddCollider(pos, 0.5f, 0.5f, Collider::TAG::ENEMY,0.f, this);
	coll->AddRigidBody(Collider::BODY_TYPE::DYNAMIC);
	draw_offset = { 32, 38 };
	coll->SetObjOffset({ -.25f, -.25f });
	timer.Start();
}

Obj_TeslaTrooper::~Obj_TeslaTrooper()
{
}

bool Obj_TeslaTrooper::Update(float dt)
{
	switch (state)
	{
	case TROOPER_STATE::GET_PATH:
		path.clear();
		target = app->objectmanager->GetNearestTank(pos_map);
		if (target != nullptr && pos_map.DistanceManhattan(target->pos_map) <= follow_range)
			if (app->pathfinding->CreatePath((iPoint)pos_map, (iPoint)target->pos_map) != -1)
			{
				std::vector<iPoint> aux = *app->pathfinding->GetLastPath();
				for (std::vector<iPoint>::iterator iter = aux.begin(); iter != aux.end(); ++iter)
				{
					path.push_back({ (*iter).x + 0.5f,(*iter).y + 0.5f });
				}
				state = TROOPER_STATE::RECHEAD_POINT;
			}
		timer.Start();
		break;
	case TROOPER_STATE::MOVE:

			if (IsOnGoal(next_pos))
			{
				path.erase(path.begin());
				state = TROOPER_STATE::RECHEAD_POINT;
			}
			pos_map += move_vect * speed * dt;
			range_pos.center = pos_map;
		break;
	case TROOPER_STATE::RECHEAD_POINT:
		{
			if (timer.ReadSec() >= check_path_time)
				state = TROOPER_STATE::GET_PATH;

			else if (path.size() > 0)
			{
				next_pos = (fPoint)(*path.begin());
				move_vect = (fPoint)(next_pos)-pos_map;
				move_vect.Normalize();

				//Change sprite direction
				angle = atan2(move_vect.y, -move_vect.x)  * RADTODEG /*+ ISO_COMPENSATION*/;
				state = TROOPER_STATE::MOVE;
			}
			else
				state = TROOPER_STATE::GET_PATH;

		}
		break;
	default:
		assert(true && "A tesla trooper have no state");
		break;
	}

	if (target != nullptr)
	{
		fPoint enemy_screen_pos = app->map->MapToScreenF(fPoint(this->pos_map.x, this->pos_map.y));
		fPoint target_screen_pos = app->map->MapToScreenF(fPoint(target->pos_map.x, target->pos_map.y));
		
		if (!attack_available)
		{
			if (TeslaTrooperCanAttack(enemy_screen_pos, target_screen_pos))
			{
				current_state = CURRENT_POS_STATE::STATE_ATTACKING;
				attack_available = true;
				perf_timer.Start();
			}
			else
			{
				current_state = CURRENT_POS_STATE::STATE_WAITING;
				attack_available = false;
			}
		}

		if (current_state == CURRENT_POS_STATE::STATE_ATTACKING)
		{
			if (perf_timer.ReadMs() > (double)attack_frequency)
			{
				target->ReduceHitPoints(25);
				attack_available = false;
			}
		}

		if (target->GetHitPoints() < 0)
		{
			// target->to_remove = true;   // CRASH !
			
			/* Used for debugging :) TOBEDELETED*/
			int i = 0;
			int j = 0;
		}
	}

	return true;
}


bool Obj_TeslaTrooper::IsOnGoal(fPoint goal)
{
	return range_pos.IsPointIn(goal);
}

void Obj_TeslaTrooper::OnTrigger(Collider* collider)
{
	if ((collider->GetTag() == Collider::TAG::BULLET))
	{
		life -= collider->damage;
		if (life <= 0)
		{
			to_remove = true;
		}
	}
}

bool Obj_TeslaTrooper::TeslaTrooperCanAttack(const fPoint& enemy_screen_pos, const fPoint& target_screen_pos) const
{
	return ((enemy_screen_pos.x > target_screen_pos.x && enemy_screen_pos.x < target_screen_pos.x + (float)attack_range.x)
		|| (enemy_screen_pos.x < target_screen_pos.x && enemy_screen_pos.x > target_screen_pos.x - (float)attack_range.x)
		|| (enemy_screen_pos.y > target_screen_pos.y && enemy_screen_pos.y < target_screen_pos.y + (float)attack_range.y)
		|| (enemy_screen_pos.y < target_screen_pos.y && enemy_screen_pos.y > target_screen_pos.y - (float)attack_range.y));
}