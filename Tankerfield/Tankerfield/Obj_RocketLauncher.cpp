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

	curr_anim = &idle;
	
	state = ENEMY_STATE::IDLE;
	detection_range = ((*app->render->cameras.begin())->screen_section.w / app->map->data.tile_width)* 1.33f;

	original_speed = speed = app->objectmanager->rocket_launcher_info.speed;

	//spawn_draw_offset = { 49, 50 };
	normal_draw_offset = { 60, 60 };
	electrocuted_draw_offset = { 35, 30 };
	draw_offset = normal_draw_offset;

	attack_damage = app->objectmanager->rocket_launcher_info.attack_damage;
	attack_range = app->objectmanager->rocket_launcher_info.attack_range;
	attack_range_squared = attack_range * attack_range;
	attack_frequency = app->objectmanager->rocket_launcher_info.attack_frequency;
	life = app->objectmanager->rocket_launcher_info.life_multiplier * pow(app->objectmanager->rocket_launcher_info.life_exponential_base, app->scene->round - 1);
	deltatime_charge = 0.5f;

	check_path_time = 2.0f;
	damaged_sprite_time = 75;
	scale = 1.f;

	coll_w = 0.5f;
	coll_h = 0.5f;
	coll = app->collision->AddCollider(pos, coll_w, coll_h, TAG::ENEMY, BODY_TYPE::DYNAMIC, 0.f, this);
	coll->SetObjOffset({ -coll_w * 2.0f, -coll_h * 1.0f });
	can_attack = false;
	deltatime_to_check_distance = 1;
}

Obj_RocketLauncher::~Obj_RocketLauncher()
{

}

void Obj_RocketLauncher::Spawn(const float& dt)
{
	state = ENEMY_STATE::GET_PATH;
}

void Obj_RocketLauncher::Attack()
{
	if (life > 0 && app->scene->game_state != GAME_STATE::NO_TYPE)
	{
		if (timer_charging.ReadSec() >= deltatime_charge)
		{
			shoot = true;
		}
		if (shoot)
		{
			if (/*&& target->coll->GetTag() == TAG::PLAYER*/
			/*&& pos_map.DistanceNoSqrt(target->pos_map) < attack_range_squared*/
			  perf_timer.ReadMs() > (double)attack_frequency)
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
}

void Obj_RocketLauncher::Move(const float & dt)
{
	if (timer_check_distance.ReadSec() >= deltatime_to_check_distance || fist_enter_to_move)
	{
		if (target->pos_map.DistanceNoSqrt(pos_map) <= attack_range_squared)
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
	fPoint p_dir(0.0f, 0.0f);
	if (target != nullptr
		&& target->coll->GetTag() == TAG::PLAYER
		&& pos_map.DistanceNoSqrt(target->pos_map) < attack_range_squared)
	{
		p_dir = app->map->ScreenToMapF(target->pos_screen.x, target->pos_screen.y) - this->pos_map;
		p_dir.Normalize();

		Bullet_RocketLauncher* bullet = (Bullet_RocketLauncher*)app->objectmanager->CreateObject(ObjectType::BULLET_ROCKETLAUNCHER, this->pos_map + p_dir);
		bullet->SetBulletProperties(
			9.0f,
			2000.0f,
			attack_damage,
			p_dir,
			atan2(-p_dir.y, p_dir.x) * RADTODEG - 45);

		bullet->SetPlayer(target);
	}

}
