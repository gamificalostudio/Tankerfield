#include <list>
#include <assert.h>
#include <vector>

#include "Brofiler/Brofiler.h"
#include "PugiXml\src\pugixml.hpp"

#include "Point.h"
#include "Log.h"
#include "Balance.h"
#include "Animation.h"
#include "App.h"
#include "Object.h"
#include "Obj_RocketLauncher.h"
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
#include "M_AnimationBank.h"
#include "Obj_Tank.h"
#include "M_PickManager.h"
#include "Bullet_RocketLauncher.h"
#include "M_Audio.h"

Obj_RocketLauncher::Obj_RocketLauncher(fPoint pos) : Obj_Enemy(pos)
{
	pugi::xml_node rocket_launcher_node = app->config.child("object").child("enemies").child("rocket_launcher");
	pugi::xml_node anim_node = app->anim_bank->animations_xml_node.child("rocketlauncher").child("animation");

	tex = app->tex->Load(rocket_launcher_node.child("tex_path").child_value());
	curr_tex = tex;
	tex_damaged = app->tex->Load("textures/Objects/enemies/flakt-sheet-white.png");

	//Loading animations ------------------------------------------------------------------------------------------------------------------------
	idle.frames = app->anim_bank->LoadFrames(anim_node.child("idle"));
	walk.frames = app->anim_bank->LoadFrames(anim_node.child("walk"));
	attack.frames = app->anim_bank->LoadFrames(anim_node.child("attack"));
	death.frames = app->anim_bank->LoadFrames(anim_node.child("death"));
	SetState(ENEMY_STATE::IDLE);
	

	//spawn_draw_offset = { 49, 50 };
	scale = 1.5f;
	//NOTE: Draw offset depends on the scale
	draw_offset = normal_draw_offset = (iPoint)(fPoint(49.f, 48.f) * scale);
	electrocuted_draw_offset = (iPoint)(fPoint(35.f, 30.f) * scale);
	
	check_path_time = 2.0f;
	damaged_sprite_time = 75;


	can_attack = false;
	distance_to_player = 10; //this is in tiles
	deltatime_to_check_distance = 1;
}

void Obj_RocketLauncher::SetStats(int level)
{
	detection_range = app->objectmanager->rocket_launcher_info.detection_range;
	squared_detection_range = detection_range * detection_range;
	original_speed = speed = app->objectmanager->rocket_launcher_info.speed;
	attack_damage = app->objectmanager->rocket_launcher_info.attack_damage;
	attack_range = app->objectmanager->rocket_launcher_info.attack_range;
	attack_range_squared = attack_range * attack_range;
	attack_frequency = app->objectmanager->rocket_launcher_info.attack_frequency;
	life = app->objectmanager->rocket_launcher_info.life_multiplier * pow(app->objectmanager->rocket_launcher_info.life_exponential_base, level - 1);
}

Obj_RocketLauncher::~Obj_RocketLauncher()
{

}

void Obj_RocketLauncher::Spawn(const float& dt)
{
	if (coll==nullptr)
	{
		coll_w = 0.5f;
		coll_h = 0.5f;
		coll = app->collision->AddCollider(pos_map, coll_w, coll_h, TAG::ENEMY, BODY_TYPE::DYNAMIC, 0.f, this);
		coll->SetObjOffset({ -coll_w * 0.5f, -coll_h * 0.5f });
	}
	else
	{
		coll->SetIsTrigger(true);
	}
	state = ENEMY_STATE::GET_PATH;
}

void Obj_RocketLauncher::Attack()
{
	if (life > 0 && app->scene->game_state != GAME_STATE::NO_TYPE)
	{
		if (target != nullptr
			&& perf_timer.ReadMs() > (double)attack_frequency)
		{
			curr_anim = &attack;
			perf_timer.Start();
			app->audio->PlayFx(sfx_attack);
			ShootMissile();
		}

		if (curr_anim == &attack
			&& curr_anim->Finished())
		{
			curr_anim = &idle;
			attack.Reset();
		}
	}
}

void Obj_RocketLauncher::Move(const float & dt)
{
	if (timer_check_distance.ReadSec() >= deltatime_to_check_distance || fist_enter_to_move)
	{
		if (target->pos_map.DistanceNoSqrt(pos_map) <= 10 * 10)
		{
			can_attack = true;
			curr_anim = &idle;
		}
		else
		{
			can_attack = false;
			curr_anim = &walk;
		}

		timer_check_distance.Start();

		if (fist_enter_to_move)
			fist_enter_to_move = false;
	}
	if (!can_attack)
	{
		if (IsOnGoal(next_pos))
		{
			if (path.size() > 0)
				path.erase(path.begin());
			else
				state = ENEMY_STATE::GET_PATH;

			state = ENEMY_STATE::RECHEAD_POINT;
		}

		if (update_velocity_vec.ReadSec() > 1)
		{
			UpdateMoveVec();
		}

		if (path_timer.ReadSec() >= check_path_time)
			state = ENEMY_STATE::GET_PATH;

		UpdatePos(dt);

	}
}
	

void Obj_RocketLauncher::ShootMissile()
{
	fPoint offset_rocket{ 1.f,1.f };
	fPoint p_dir(0.0f, 0.0f);
	if (target != nullptr
		&& pos_map.DistanceNoSqrt(target->pos_map) < attack_range_squared)
	{
		if (!target->Alive())
		{
			SetState(ENEMY_STATE::GET_PATH);
		}
		else
		{
			p_dir = app->map->ScreenToMapF(target->pos_screen.x, target->pos_screen.y) - this->pos_map + offset_rocket;
			p_dir.Normalize();

			Bullet_RocketLauncher* bullet = (Bullet_RocketLauncher*)app->objectmanager->CreateObject(ObjectType::BULLET_ROCKETLAUNCHER, this->pos_map - offset_rocket /*+ p_dir*/);
			bullet->SetBulletProperties(
				9.0f,
				2000.0f,
				attack_damage,
				p_dir,
				atan2(-p_dir.y, p_dir.x) * RADTODEG - 45);

			bullet->SetPlayer(target);
		}
		
	}

}
