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
	//burning texture
	burn_texture = app->tex->Load(app->anim_bank->animations_xml_node.child("burn").child("animations").child("burn").attribute("texture").as_string());
	burn.frames = app->anim_bank->LoadFrames(app->anim_bank->animations_xml_node.child("burn").child("animations").child("burn"));
	dying_burn.frames = app->anim_bank->LoadFrames(app->anim_bank->animations_xml_node.child("burn").child("animations").child("dying_burn"));

	pugi::xml_node tesla_trooper_node = app->config.child("object").child("enemies").child("tesla_trooper");
	pugi::xml_node anim_node			= app->anim_bank->animations_xml_node.child("tesla").child("animations");


	//TEXTURES =============================================
	explosion_apper_tex = app->tex->Load("textures/Objects/particles/explosion2.png");
	tex = app->tex->Load(tesla_trooper_node.child("tex_path").child_value());
	tex_damaged = app->tex->Load(tesla_trooper_node.child("tex_damaged_path").child_value());
	oiled_tex	= app->tex->Load(tesla_trooper_node.child("tex_oiled_path").child_value());
	
	


	//ANIMATIONS =============================================
	idle.frames = app->anim_bank->LoadFrames(anim_node.child("idle"));
	walk.frames = app->anim_bank->LoadFrames(anim_node.child("walk"));
	attack.frames = app->anim_bank->LoadFrames(anim_node.child("attack"));
	death.frames = app->anim_bank->LoadFrames(anim_node.child("death"));

	spawn_anim.frames = app->anim_bank->LoadFrames(app->anim_bank->animations_xml_node.child("portal").child("animations").child("spawn"));


	sfx_attack = app->audio->LoadFx("audio/Fx/entities/enemies/tesla-trooper/laser-tesla-trooper.wav");
	sfx_spawn = app->audio->LoadFx("audio/Fx/entities/enemies/tesla-trooper/siroon.wav", 20);
	sfx_hit = app->audio->LoadFx("audio/Fx/entities/enemies/tesla-trooper/hit.wav", 25);
	sfx_death = app->audio->LoadFx("audio/Fx/entities/enemies/tesla-trooper/death.wav", 25);

	app->audio->PlayFx(sfx_spawn);


	original_speed = speed = app->objectmanager->tesla_trooper_info.speed;

	detection_range = ((*app->render->cameras.begin())->screen_section.w / app->map->data.tile_width)* 1.33f; // 1.33 are 4/3
	squared_detection_range = detection_range * detection_range;
	coll_w = 0.5f;
	coll_h = 0.5f;
	coll = app->collision->AddCollider(pos, coll_w, coll_h, TAG::ENEMY, BODY_TYPE::DYNAMIC, 0.0f, this);
	coll->SetObjOffset({ -coll_w * 0.5f, -coll_h * 0.5f });

	draw_offset			= { 24, 28 };
	normal_draw_offset = { 24, 28 };
	electrocuted_draw_offset = { 24, 28 };


	//parameters-------------------------------------------
	attack_damage = app->objectmanager->tesla_trooper_info.attack_damage;
	attack_range = app->objectmanager->tesla_trooper_info.attack_range;
	attack_range_squared = attack_range * attack_range;
	attack_frequency = app->objectmanager->tesla_trooper_info.attack_frequency;
	

	//Timers ----------------
	check_path_time = 2.f; // 10s

	damaged_sprite_time = 75;

	scale = 0.75f;

	
}

bool Obj_TeslaTrooper::Start()
{
	curr_tex = tex;
	last_texture = tex;
	curr_anim = &walk;
	draw = false;
	state = ENEMY_STATE::SPAWN; //enemy
	life = app->objectmanager->tesla_trooper_info.life_multiplier * pow(app->objectmanager->tesla_trooper_info.life_exponential_base, app->scene->round - 1);
	app->audio->PlayFx(sfx_spawn);
	coll->ActiveOnTrigger(true);
	ResetAllAnimations();

	return true;
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
void Obj_TeslaTrooper::Move(const float & dt)
{
	

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
		}
	}
	if (update_velocity_vec.ReadSec() > 1)
	{
		UpdateVelocity();
	}

	UpdatePos(dt);

	if (path_timer.ReadSec() > check_path_time || (teleport_timer.ReadSec() >= check_teleport_time && (target == nullptr || !target->Alive())))
	{
		state = ENEMY_STATE::GET_PATH;
		curr_anim = &idle;
	}

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


bool Obj_TeslaTrooper::Draw(float dt, Camera * camera)
{

	if ((state == ENEMY_STATE::TELEPORT_IN || state == ENEMY_STATE::TELEPORT_OUT) && in_portal != nullptr)
	{
		SDL_Rect portal_frame = in_portal->GetFrame(0);
		app->render->Blit(
			portal_tex,
			pos_screen.x - portal_frame.w * 0.5f,
			pos_screen.y - portal_frame.h,
			camera,
			&portal_frame);
	}

	if (draw)
	{
		app->render->BlitScaled(
			curr_tex,
			pos_screen.x - draw_offset.x,
			pos_screen.y - draw_offset.y,
			camera,
			&frame,
			scale,
			scale);
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


