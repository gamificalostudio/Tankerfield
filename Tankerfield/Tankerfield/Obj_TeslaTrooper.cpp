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
#include "M_PickManager.h"
#include "M_AnimationBank.h"
#include "M_Scene.h"
#include "Obj_Tank.h"

Obj_TeslaTrooper::Obj_TeslaTrooper(fPoint pos) : Object (pos)
{
	pugi::xml_node tesla_trooper_node = app->config.child("object").child("tesla_trooper");

	tex = app->tex->Load("textures/Objects/shk-sheet.png");
	portal_tex = app->tex->Load("textures/Objects/portal.png");
	curr_tex = tex;

	walk.frames = app->anim_bank->LoadFrames(tesla_trooper_node.child("animations").child("walk"));
	attack.frames = app->anim_bank->LoadFrames(tesla_trooper_node.child("animations").child("attack"));
	portal_animation.frames = app->anim_bank->LoadFrames(app->config.child("object").child("portal").child("animations").child("open"));
	portal_close_anim.frames = app->anim_bank->LoadFrames(app->config.child("object").child("portal").child("animations").child("close"));
	curr_anim = &walk;


	//Things
	state				= TROOPER_STATE::GET_PATH;
	speed				= 1.5F;
	range_pos.center	= pos_map;
	range_pos.radius	= 0.5f;
	detection_range		= (*app->render->cameras.begin())->screen_section.w/app->map->data.tile_width;
	coll				= app->collision->AddCollider(pos, 0.5f, 0.5f, Collider::TAG::ENEMY,0.f, this);
	coll->AddRigidBody(Collider::BODY_TYPE::DYNAMIC);
	coll->SetObjOffset({ -.25f, -.25f });
	draw_offset			= { 32, 38 };
	
	attack_damage		= 10;
	attack_range		= 1;
	attack_range_squared = attack_range * attack_range;
	attack_frequency = 3000.0f;
	//Timers ----------------
	check_teleport_time = 1; //10s
	teleport_timer.Start();
	check_path_time = 1.f; // 1s
	path_timer.Start();
}

Obj_TeslaTrooper::~Obj_TeslaTrooper()
{
}

bool Obj_TeslaTrooper::Update(float dt)
{
	Movement(dt);
	Attack();
	
	return true;
}

void Obj_TeslaTrooper::Attack()
{
	if (target != nullptr
		&& pos_map.DistanceNoSqrt(target->pos_map) < attack_range_squared
		&& perf_timer.ReadMs() > (double)attack_frequency)
	{
		curr_anim = &attack;
		target->SetLife(target->GetLife() - attack_damage);
		perf_timer.Start();
	}

	if (curr_anim == &attack&&curr_anim->Finished())
	{
		curr_anim = &walk;
		attack.Reset();
	}

}

void Obj_TeslaTrooper::Movement(float &dt)
{
	
	switch (state)
	{
	case TROOPER_STATE::GET_PATH:
	{
		path.clear();
		move_vect.SetToZero();

		target = app->objectmanager->GetNearestTank(pos_map);
		if (target != nullptr)
		{
			if (this->pos_map.DistanceManhattan(target->pos_map) <= detection_range)
			{
				if (app->pathfinding->CreatePath((iPoint)pos_map, (iPoint)target->pos_map) != -1)
				{

					std::vector<iPoint> aux = *app->pathfinding->GetLastPath();
					for (std::vector<iPoint>::iterator iter = aux.begin()+1; iter != aux.end(); ++iter)
					{
						path.push_back({ (*iter).x + 0.5f,(*iter).y + 0.5f });
					}

					state = TROOPER_STATE::RECHEAD_POINT;
				}
				
			
			}
			else if (teleport_timer.ReadSec() >= check_teleport_time)
			{
				state = TROOPER_STATE::GET_TELEPORT_POINT;
			}

		}

		path_timer.Start();
	}
	break;
	case TROOPER_STATE::MOVE:
	{
		if (IsOnGoal(next_pos))
		{
			path.erase(path.begin());
			state = TROOPER_STATE::RECHEAD_POINT;
		}
		pos_map += move_vect * speed * dt;
		range_pos.center = pos_map;

		if (path_timer.ReadSec() >= check_path_time)
			state = TROOPER_STATE::GET_PATH;
		
	}
	break;
	case TROOPER_STATE::RECHEAD_POINT:
	{
		if (path.size() > 0)
		{
			next_pos = (fPoint)(*path.begin());
			move_vect = (fPoint)(next_pos)-pos_map;
			move_vect.Normalize();

			//Change sprite direction
			angle = atan2(move_vect.y, -move_vect.x)  * RADTODEG - ISO_COMPENSATION;
			state = TROOPER_STATE::MOVE;
		}
		else
			state = TROOPER_STATE::GET_PATH;
	}
	break;

	case TROOPER_STATE::GET_TELEPORT_POINT:
	{
		move_vect.SetToZero();
		float distance_to_tank = this->pos_map.DistanceManhattan(target->pos_map);
		SpawnPoint* nearest_spawners_points = nullptr;
		float last_distance_to_spawnpoint = 0.f;
		for (std::vector<SpawnPoint*>::iterator spawn_point = app->map->data.spawners_position_enemy.begin(); spawn_point != app->map->data.spawners_position_enemy.end(); ++spawn_point)
		{
			float distance_to_this_spawnpoint = this->pos_map.DistanceManhattan((*spawn_point)->pos);
			if (distance_to_this_spawnpoint < distance_to_tank && target->pos_map.DistanceManhattan((*spawn_point)->pos))
			{
				if (nearest_spawners_points == nullptr || distance_to_this_spawnpoint < last_distance_to_spawnpoint)
				{
					if (teleport_spawnpoint==nullptr || teleport_spawnpoint != nearest_spawners_points)
					{
						nearest_spawners_points = (*spawn_point);
						last_distance_to_spawnpoint = distance_to_this_spawnpoint;
					}
					
				}
			}
		}
		if (nearest_spawners_points != nullptr)
		{
			teleport_spawnpoint = nearest_spawners_points;
			state = TROOPER_STATE::TELEPORT_IN;
			in_portal = &portal_animation;
			angle = -90;
			teleport_timer.Start();
			teleport_anim_duration.Start();
		}
	}
	break;
	case TROOPER_STATE::TELEPORT_IN:
	{
		if (teleport_anim_duration.ReadSec() >= 1)
		{
			in_portal = &portal_close_anim;
			draw = false;
			if (in_portal->Finished())
			{
				pos_map = teleport_spawnpoint->pos;
				state = TROOPER_STATE::TELEPORT_OUT;
				teleport_timer.Start();
				angle = 90;

			}
		}
		
		if(in_portal!=nullptr)
			in_portal->NextFrame(dt);
	}
		break;
	case TROOPER_STATE::TELEPORT_OUT:
	{
			if (in_portal->Finished())
			{
				in_portal = &portal_animation;
				state = TROOPER_STATE::GET_PATH;
				angle = -90;
				draw = true;
			}

		if (in_portal != nullptr)
			in_portal->NextFrame(dt);
	}
	break;
	
	default:
		assert(true && "A tesla trooper have no state");
		break;
	}

}

void Obj_TeslaTrooper::DrawDebug(const Camera* camera)
{
	if (path.size() >= 2)
	{
		for (std::vector<fPoint>::iterator iter = path.begin(); iter != path.end() - 1; ++iter)
		{
			fPoint point1 = { (*iter).x + 0.5F, (*iter).y + 0.5F };
			fPoint point2 = { (*(iter + 1)).x + 0.5F, (*(iter + 1)).y + 0.5F };
			app->render->DrawIsometricLine(point1, point2, { 255,255,255,255 }, camera);
		}
	}

}

bool Obj_TeslaTrooper::Draw(float dt, Camera * camera)
{
	if (state == TROOPER_STATE::TELEPORT_IN && in_portal != nullptr)
	{
	SDL_Rect portal_frame = in_portal->GetFrame(0);
		app->render->Blit(
			portal_tex,
			pos_screen.x - portal_frame.w*0.5f,
			pos_screen.y- portal_frame.h,
			camera,
			&portal_frame);
	}
	if(draw)
	app->render->Blit(
		curr_tex,
		pos_screen.x - draw_offset.x,
		pos_screen.y - draw_offset.y,
		camera,
		&frame);
	return true;
}

bool Obj_TeslaTrooper::IsOnGoal(fPoint goal)
{
	return range_pos.IsPointIn(goal);
}

void Obj_TeslaTrooper::OnTrigger(Collider* collider)
{
	if ((collider->GetTag() == Collider::TAG::BULLET)||(collider->GetTag() == Collider::TAG::FRIENDLY_BULLET))
	{
		life -= collider->damage;
		if (life <= 0)
		{
			app->pick_manager->PickUpFromEnemy(pos_map);
			to_remove = true;
		}
	}
}