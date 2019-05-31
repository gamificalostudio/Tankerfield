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
	//burning texture
	burn_texture = app->tex->Load(app->anim_bank->animations_xml_node.child("burn").child("animations").child("burn").attribute("texture").as_string());
	burn.frames = app->anim_bank->LoadFrames(app->anim_bank->animations_xml_node.child("burn").child("animations").child("burn"));
	dying_burn.frames = app->anim_bank->LoadFrames(app->anim_bank->animations_xml_node.child("burn").child("animations").child("dying_burn"));

	pugi::xml_node suicidal_node = app->config.child("object").child("enemies").child("suicidal");
	pugi::xml_node anim_node = app->anim_bank->animations_xml_node.child("suicidal").child("animation");

	tex = app->tex->Load(suicidal_node.child("tex_path").child_value());


	idle.frames = app->anim_bank->LoadFrames(anim_node.child("idle"));
	walk.frames = app->anim_bank->LoadFrames(anim_node.child("walk"));
	attack.frames = app->anim_bank->LoadFrames(anim_node.child("attack"));
	death.frames = app->anim_bank->LoadFrames(anim_node.child("death"));

	
	original_speed = speed = app->objectmanager->suicidal_info.speed;
	detection_range = ((*app->render->cameras.begin())->screen_section.w / app->map->data.tile_width) * 1.33f;

	normal_draw_offset = { 33, 50 };
	draw_offset = normal_draw_offset;

	coll_w = 0.5f;
	coll_h = 0.5f;

	coll = app->collision->AddCollider(pos, coll_w, coll_h, TAG::ENEMY, BODY_TYPE::DYNAMIC, 0.0f, this);
	coll->SetObjOffset({ -coll_w * 2.25f, -coll_h * 1.75f });

	attack_damage = app->objectmanager->suicidal_info.attack_damage;
	attack_range = app->objectmanager->suicidal_info.attack_range;
	attack_range_squared = attack_range * attack_range;
	attack_frequency = app->objectmanager->suicidal_info.attack_frequency;
	

	check_path_time = 2.0f;

	scale = 0.75f;
}

bool Obj_Suicidal::Start()
{
	curr_tex = tex;
	state = ENEMY_STATE::SPAWN;
	curr_anim = &idle;
	life = app->objectmanager->suicidal_info.life_multiplier * pow(app->objectmanager->suicidal_info.life_exponential_base, app->scene->round - 1);
	ResetAllAnimations();

	return true;
}

Obj_Suicidal::~Obj_Suicidal()
{

}

void Obj_Suicidal::Spawn(const float& dt)
{
	state = ENEMY_STATE::GET_PATH;
}

void Obj_Suicidal::Attack()
{
	if (life > 0 && app->scene->game_state != GAME_STATE::NO_TYPE)
	{
		if (target != nullptr
			&& target->coll->GetTag() == TAG::PLAYER
			&& pos_map.DistanceNoSqrt(target->pos_map) < attack_range_squared
			&& perf_timer.ReadMs() > (double)attack_frequency)
		{
			curr_anim = &attack;
			target->ReduceLife(attack_damage);
			perf_timer.Start();
			app->audio->PlayFx(sfx_attack);
			state = ENEMY_STATE::DEAD;
		}

		if (curr_anim == &attack
			&& curr_anim->Finished())
		{
			curr_anim = &idle;
			attack.Reset();
			
		}
	}
}

