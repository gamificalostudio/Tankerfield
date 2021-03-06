#include <list>
#include <assert.h>
#include <vector>
//#include <tgmath.h>

#include "Brofiler/Brofiler.h"
#include "PugiXml\src\pugixml.hpp"

#include "Point.h"
#include "Log.h"
#include "Animation.h"

#include "App.h"
#include "Object.h"
#include "Obj_Suicidal.h"
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
#include "Bullet_Laser.h"
#include "Obj_Bullet.h"
#include "M_Audio.h"


Obj_Suicidal::Obj_Suicidal(fPoint pos) : Obj_Enemy(pos)
{
	pugi::xml_node suicidal_node = app->config.child("object").child("enemies").child("suicidal");
	pugi::xml_node anim_node = app->anim_bank->animations_xml_node.child("suicidal").child("animation");

	tex = app->tex->Load(suicidal_node.child("tex_path").child_value());
	tex_damaged = app->tex->Load(suicidal_node.child("tex_damaged_path").child_value());
	curr_tex = tex;

	idle.frames = app->anim_bank->LoadFrames(anim_node.child("idle"));
	walk.frames = app->anim_bank->LoadFrames(anim_node.child("walk"));
	attack.frames = app->anim_bank->LoadFrames(anim_node.child("attack"));
	death.frames = app->anim_bank->LoadFrames(anim_node.child("death"));

	scale = 1.5f;
	//INFO: Draw offset depends on the scale
	draw_offset = normal_draw_offset = (iPoint)(fPoint(32.f, 36.f) * scale);
	check_path_time = 2.0f;
}

//Called after creating the enemy
void Obj_Suicidal::SetStats(int level)
{
	detection_range = app->objectmanager->suicidal_info.detection_range;
	squared_detection_range = detection_range * detection_range;
	original_speed = speed = app->objectmanager->suicidal_info.speed;
	attack_damage = app->objectmanager->suicidal_info.attack_damage;
	attack_range = app->objectmanager->suicidal_info.attack_range;
	attack_range_squared = attack_range * attack_range;
	attack_frequency = app->objectmanager->suicidal_info.attack_frequency;
	life = app->objectmanager->suicidal_info.life_multiplier * pow(app->objectmanager->suicidal_info.life_exponential_base, level - 1);
}

Obj_Suicidal::~Obj_Suicidal()
{

}


void Obj_Suicidal::Spawn(const float& dt)
{
	if (coll == nullptr)
	{
		coll_w = 0.5f;
		coll_h = 0.5f;

		coll = app->collision->AddCollider(pos_map, coll_w, coll_h, TAG::ENEMY, BODY_TYPE::DYNAMIC, 0.0f, this);
		coll->SetObjOffset({ -coll_w * 0.5f, -coll_h * 0.5f });
	}
	else
	{
		coll->SetIsTrigger(true);
	}

	SetState(ENEMY_STATE::GET_PATH);
}

void Obj_Suicidal::Attack()
{
	if (life > 0 && app->scene->game_state != GAME_STATE::NO_TYPE)
	{
		if (target != nullptr
			&& pos_map.DistanceNoSqrt(target->pos_map) < attack_range_squared
			&& perf_timer.ReadMs() > (double)attack_frequency)
		{
			curr_anim = &attack;
			target->ReduceLife(attack_damage);
			perf_timer.Start();
			app->audio->PlayFx(sfx_attack);
			SetState(ENEMY_STATE::DEAD);
		}

		if (curr_anim == &attack
			&& curr_anim->Finished())
		{
			curr_anim = &idle;
			attack.Reset();
		}
	}
}

