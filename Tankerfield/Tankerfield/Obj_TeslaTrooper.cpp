#include <list>
#include <assert.h>
#include <vector>
#include <math.h>

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
#include "M_Audio.h"
#include "M_Map.h"
#include "M_Collision.h"
#include "WeaponInfo.h"
#include "M_PickManager.h"
#include "M_AnimationBank.h"
#include "M_Scene.h"
#include "Obj_Tank.h"
#include "Bullet_Laser.h"
#include "Obj_Bullet.h"




Obj_TeslaTrooper::Obj_TeslaTrooper(fPoint pos) : Obj_Enemy(pos)
{
	pugi::xml_node tesla_trooper_node	= app->config.child("object").child("tesla_trooper");
	pugi::xml_node anim_node			= app->anim_bank->animations_xml_node.child("tesla").child("animations");

	tex			= app->tex->Load(tesla_trooper_node.child("tex_path").child_value());
	tex_damaged = app->tex->Load(tesla_trooper_node.child("tex_damaged_path").child_value());
	oiled_tex	= app->tex->Load(tesla_trooper_node.child("tex_oiled_path").child_value());
	portal_tex	= app->tex->Load(tesla_trooper_node.child("tex_portal").child_value());
	curr_tex = tex;

	explosion_apper_tex = app->tex->Load("textures/Objects/particles/explosion2.png");

	//Loading animations ------------------------------------------------------------------------------------------------------------------------
	idle.frames		= app->anim_bank->LoadFrames(anim_node.child("idle"));
	walk.frames		= app->anim_bank->LoadFrames(anim_node.child("walk"));
	attack.frames	= app->anim_bank->LoadFrames(anim_node.child("attack"));
	death.frames	= app->anim_bank->LoadFrames(anim_node.child("death"));
	curr_anim = &idle;

	portal_animation.frames = app->anim_bank->LoadFrames(app->config.child("object").child("portal").child("animations").child("open"));
	portal_close_anim.frames = app->anim_bank->LoadFrames(app->config.child("object").child("portal").child("animations").child("close"));


	//curr_anim = &idle;
	curr_anim = &walk;
	spawn_anim.frames = app->anim_bank->LoadFrames(app->anim_bank->animations_xml_node.child("portal").child("animations").child("spawn"));


	sfx_attack = app->audio->LoadFx("audio/Fx/entities/enemies/tesla-trooper/laser-tesla-trooper.wav");
	sfx_spawn = app->audio->LoadFx("audio/Fx/entities/enemies/tesla-trooper/siroon.wav",20);
	sfx_hit = app->audio->LoadFx("audio/Fx/entities/enemies/tesla-trooper/hit.wav", 25);
	sfx_death = app->audio->LoadFx("audio/Fx/entities/enemies/tesla-trooper/death.wav", 25);

	//app->audio->PlayFx(sfx_spawn);
	draw = false;

	//Things
	state				= ENEMY_STATE::SPAWN; //enemy
	original_speed=speed= tesla_trooper_node.child("speed").attribute("num").as_float();
	/*range_pos.center	= pos_map;
	range_pos.radius	= 0.5f;*/
	detection_range		= ((*app->render->cameras.begin())->screen_section.w/app->map->data.tile_width)* 1.33f; // 1.33 son 4/3
	squared_detection_range = detection_range * detection_range;
	coll_w = 0.5f;
	coll_h = 0.5f;
	coll				= app->collision->AddCollider(pos, coll_w, coll_h, Collider::TAG::ENEMY,0.f, this);
	coll->AddRigidBody(Collider::BODY_TYPE::DYNAMIC);
	coll->SetObjOffset({ -coll_w * 0.5f, -coll_h * 0.5f });

	draw_offset			= { 24, 28 };

	//parameters-------------------------------------------
	attack_damage		= tesla_trooper_node.child("attack_damage").attribute("num").as_float();
	attack_range		= tesla_trooper_node.child("attack_range").attribute("num").as_float();
	attack_range_squared = attack_range * attack_range;
	attack_frequency = tesla_trooper_node.child("attack_frequency").attribute("num").as_float();
	life = pow(tesla_trooper_node.child("base_life").attribute("num").as_float(), app->scene->round);
	
	//teleport 
	check_teleport_time = 10; //10s
	teleport_timer.Start();

	teleport_enemies_max = tesla_trooper_node.child("teleport_max_enemies").attribute("num").as_uint();

	//Timers ----------------
	check_path_time = 2.f; // 10s


	damaged_sprite_time = 150;

	scale = 0.75f;

	app->audio->PlayFx(sfx_spawn);
}

Obj_TeslaTrooper::~Obj_TeslaTrooper()
{
}


void Obj_TeslaTrooper::TeleportOut(float & dt)
{
	if (in_portal->Finished())
	{
		in_portal->Reset();
		in_portal = &portal_animation;
		state = ENEMY_STATE::GET_PATH;
		angle = -90;
		draw = true;
	}
	if (in_portal != nullptr)
	{
		in_portal->NextFrame(dt);
	}
}
void Obj_TeslaTrooper::TeleportIn(float & dt)
{
	move_vect.SetToZero();
	if (in_portal != nullptr)
		in_portal->NextFrame(dt);

	if (teleport_anim_duration.ReadSec() >= 1)
	{
		in_portal = &portal_close_anim;
		draw = false;
		if (in_portal->Finished())
		{
			in_portal->Reset();
			pos_map = teleport_spawnpoint->pos;
			state = ENEMY_STATE::TELEPORT_OUT;
			teleport_timer.Start();
			angle = 90;
		}
	}
}
void Obj_TeslaTrooper::GetTeleportPoint()
{
	move_vect.SetToZero();
	float distance_to_tank = this->pos_map.DistanceManhattan(target->pos_map);
	SpawnPoint* nearest_spawners_points = nullptr;
	float last_distance_to_spawnpoint = 0.f;

	for (std::vector<SpawnPoint*>::iterator spawn_point = app->map->data.spawners_position_enemy.begin(); spawn_point != app->map->data.spawners_position_enemy.end(); ++spawn_point)
	{
		float distance_to_this_spawnpoint = this->pos_map.DistanceManhattan((*spawn_point)->pos);

		if ((target->pos_map.DistanceManhattan((*spawn_point)->pos) <= distance_to_tank
			/*&& distance_to_this_spawnpoint <= distance_to_tank*/)
			&& (nearest_spawners_points == nullptr || distance_to_this_spawnpoint < last_distance_to_spawnpoint)
			&& (teleport_spawnpoint == nullptr || teleport_spawnpoint != (*spawn_point)))
		{
			nearest_spawners_points = (*spawn_point);
			last_distance_to_spawnpoint = distance_to_this_spawnpoint;
		}
	}

	if (nearest_spawners_points != nullptr && distance_to_tank > target->pos_map.DistanceManhattan(nearest_spawners_points->pos))
	{
		check_teleport_time = nearest_spawners_points->pos.DistanceTo(pos_map) / speed;
		uint number_of_enemies = app->objectmanager->GetNumberOfEnemies();
		if (number_of_enemies <= teleport_enemies_max)
		{
			check_teleport_time = check_teleport_time * ((number_of_enemies) / teleport_enemies_max);
		}
		teleport_spawnpoint = nearest_spawners_points;
		state = ENEMY_STATE::TELEPORT_IN;
		in_portal = &portal_animation;
		angle = -90;

		teleport_anim_duration.Start();
	}
	else
	{
		state = ENEMY_STATE::GET_PATH;

	}
	teleport_timer.Start();
}
void Obj_TeslaTrooper::Spawn(const float & dt)
{
	spawn_anim.NextFrame(dt);
	if ((int)spawn_anim.current_frame >= 6)
	{
		draw = true;
	}
	if (spawn_anim.Finished())
	{
		spawn_anim.Reset();
		state = ENEMY_STATE::GET_PATH;
	}
}
void Obj_TeslaTrooper::Idle()
{
	if (path_timer.ReadSec() > check_path_time)
	{
		state = ENEMY_STATE::GET_PATH;
	}
}
int Obj_TeslaTrooper::Move(float & dt)
{
	int retflag;
	retflag = 1;
	if (IsOnGoal(next_pos))
	{
		update_velocity_vec.Start();
		if (path.size() > 0)
		{
			path.erase(path.begin());
		}

		if (path.size() > 0)
		{
			next_pos = (fPoint)(*path.begin());
			next_pos += {0.5f, 0.5f};
			UpdateVelocity();
		}
		else
		{
			state = ENEMY_STATE::GET_PATH;
			curr_anim = &idle;
			{ retflag = 2;
			return retflag; };
		}
	}
	if (update_velocity_vec.ReadSec() > 1)
	{
		UpdateVelocity();
	}

	pos_map += move_vect * speed * dt;
	range_pos.center = pos_map;

	if (path_timer.ReadSec() > check_path_time || (teleport_timer.ReadSec() >= check_teleport_time && (target == nullptr || !target->Alive())))
	{
		state = ENEMY_STATE::GET_PATH;
		curr_anim = &idle;
	}
	return retflag;
	
}

void Obj_TeslaTrooper::GetPath()
{
	curr_anim = &idle;
	move_vect.SetToZero();
	target = app->objectmanager->GetNearestTank(pos_map);

	if (target != nullptr)
	{
		if (pos_map.DistanceNoSqrt(target->pos_map) <= squared_detection_range
			&& app->pathfinding->CreatePath((iPoint)pos_map, (iPoint)target->pos_map) != -1)
		{
			path.clear();
			path = *app->pathfinding->GetLastPath();
			if (path.size() > 0)
				path.erase(path.begin());

			next_pos = (fPoint)(*path.begin());
			UpdateVelocity();

			state = ENEMY_STATE::MOVE;
			curr_anim = &walk;
		}
		else
		{
			if (teleport_timer.ReadSec() >= check_teleport_time && path.size() == 0)
			{
				state = ENEMY_STATE::GET_TELEPORT_POINT;
				curr_anim = &idle;
			}
			else if (path.size() > 0)
			{
				state = ENEMY_STATE::MOVE;
				curr_anim = &walk;
			}
			else
			{
				state = ENEMY_STATE::IDLE;
				curr_anim = &idle;
				path_timer.Start();
			}
		}
	}
}

//void Obj_TeslaTrooper::DrawDebug(const Camera* camera)
//{
//	if (path.size() >= 2)
//	{
//		for (std::vector<fPoint>::iterator iter = path.begin(); iter != path.end() - 1; ++iter)
//		{
//			fPoint point1 = { (*iter).x + 0.5F, (*iter).y + 0.5F };
//			fPoint point2 = { (*(iter + 1)).x + 0.5F, (*(iter + 1)).y + 0.5F };
//			app->render->DrawIsometricLine(point1, point2, { 255,255,255,255 }, camera);
//		}
//	}
//
//}

bool Obj_TeslaTrooper::Draw(float dt, Camera * camera)
{

	if ((state == ENEMY_STATE::TELEPORT_IN || state == ENEMY_STATE::TELEPORT_OUT) && in_portal != nullptr)
	{
		SDL_Rect portal_frame = in_portal->GetFrame(0);
		app->render->Blit(
			portal_tex,
			pos_screen.x - portal_frame.w * 0.5f,
			pos_screen.y- portal_frame.h,
			camera,
			&portal_frame);
	}
	
	if (draw)
	{
		Obj_Enemy::Draw(dt, camera);
	}
	if (state == ENEMY_STATE::SPAWN)
	{
		SDL_Rect rect = spawn_anim.GetFrame(0);
		app->render->Blit(
			portal_tex,
			pos_screen.x - rect.w*0.5f,
			pos_screen.y - rect.h,
			camera,
			&rect);

	}


	return true;
}

inline void Obj_TeslaTrooper::UpdateVelocity()
{
	fPoint new_move_vec = (fPoint)(next_pos)-pos_map;
	new_move_vec.Normalize();
	if (new_move_vec != move_vect)
	{
		move_vect = new_move_vec;
		angle = atan2(move_vect.y, -move_vect.x)  * RADTODEG - ISO_COMPENSATION;
	}
}

