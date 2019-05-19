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
	pugi::xml_node anim_node = app->anim_bank->animations_xml_node.child("rocketlauncher");

	tex = app->tex->Load(rocket_launcher_node.child("tex_path").child_value());
	curr_tex = tex;

	//Loading animations ------------------------------------------------------------------------------------------------------------------------
	idle.frames = app->anim_bank->LoadFrames(anim_node.child("idle"));
	walk.frames = app->anim_bank->LoadFrames(anim_node.child("walk"));
	attack.frames = app->anim_bank->LoadFrames(anim_node.child("attack"));
	death.frames = app->anim_bank->LoadFrames(anim_node.child("death"));

	curr_anim = &idle;
	
	state = ENEMY_STATE::IDLE;
	detection_range = ((*app->render->cameras.begin())->screen_section.w / app->map->data.tile_width)* 1.33f;

	original_speed = speed = app->objectmanager->rocket_launcher_info.speed;

	spawn_draw_offset = { 49, 50 };
	normal_draw_offset = { 49, 50 };
	draw_offset = spawn_draw_offset;

	attack_damage = app->objectmanager->rocket_launcher_info.attack_damage;
	attack_range = app->objectmanager->rocket_launcher_info.attack_range;
	attack_range_squared = attack_range * attack_range;
	attack_frequency = app->objectmanager->rocket_launcher_info.attack_frequency;
	life = app->objectmanager->rocket_launcher_info.life_multiplier * pow(app->objectmanager->rocket_launcher_info.life_exponential_base, app->scene->round - 1);
	
	check_path_time = 2.0f;

	scale = 0.75f;

	coll_w = 0.5f;
	coll_h = 0.5f;
	coll = app->collision->AddCollider(pos, coll_w, coll_h, TAG::ENEMY, BODY_TYPE::DYNAMIC, 0.f, this);
	coll->SetObjOffset({ -coll_w * 2.0f, -coll_h * 1.0f });
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
		if (target != nullptr
			&& target->coll->GetTag() == TAG::PLAYER
			&& pos_map.DistanceNoSqrt(target->pos_map) < attack_range_squared
			&& perf_timer.ReadMs() > (double)attack_frequency)
		{
			curr_anim = &attack;
			//target->ReduceLife(attack_damage);
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

void Obj_RocketLauncher::ShootMissile()
{
	if (target != nullptr
		&& target->coll->GetTag() == TAG::PLAYER
		&& pos_map.DistanceNoSqrt(target->pos_map) < attack_range_squared)
	{
		fPoint p_dir = app->map->ScreenToMapF(target->pos_map.x, target->pos_map.y) - pos_map;
		p_dir.Normalize();

		Bullet_RocketLauncher* bullet = (Bullet_RocketLauncher*)app->objectmanager->CreateObject(ObjectType::BULLET_ROCKETLAUNCHER, this->pos_map + p_dir);
		bullet->SetBulletProperties(
			10.0f,
			2000.0f,
			20,
			p_dir,
			atan2(-p_dir.y, p_dir.x) * RADTODEG - 45);

		bullet->SetPlayer(target);
	}
}
