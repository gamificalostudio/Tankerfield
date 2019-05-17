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
	pugi::xml_node suicidal_node = app->config.child("object").child("suicidal");
	pugi::xml_node anim_node = app->anim_bank->animations_xml_node.child("suicidal");

	tex = app->tex->Load(suicidal_node.child("tex_path").child_value());
	curr_tex = tex;

	idle.frames = app->anim_bank->LoadFrames(anim_node.child("idle"));
	walk.frames = app->anim_bank->LoadFrames(anim_node.child("walk"));
	attack.frames = app->anim_bank->LoadFrames(anim_node.child("attack"));
	death.frames = app->anim_bank->LoadFrames(anim_node.child("death"));

	state = ENEMY_STATE::SPAWN;
	speed = suicidal_node.child("speed").attribute("num").as_float();
	detection_range = ((*app->render->cameras.begin())->screen_section.w / app->map->data.tile_width) * 1.33f;

	normal_draw_offset = { 33, 50 };
	draw_offset = normal_draw_offset;

	coll_w = 0.5f;
	coll_h = 0.5f;

	coll = app->collision->AddCollider(pos, coll_w, coll_h, Collider::TAG::ENEMY, 0.0f, this);
	coll->AddRigidBody(Collider::BODY_TYPE::DYNAMIC);
	coll->SetObjOffset({ -coll_w * 2.0f, -coll_h * 1.5f });

	attack_damage = suicidal_node.child("attack_damage").attribute("num").as_float();
	attack_range = suicidal_node.child("attack_range").attribute("num").as_float();
	attack_range_squared = attack_range * attack_range;
	attack_frequency = suicidal_node.child("attack_frequency").attribute("num").as_float();
	life = pow(suicidal_node.child("base_life").attribute("num").as_float(), app->scene->round);

	check_path_time = 2.0f;

	curr_anim = &idle;

	scale = 0.75f;
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
	if (life > 0 && app->scene->stat_of_wave != WaveStat::NO_TYPE)
	{
		if (target != nullptr
			&& target->coll->GetTag() == Collider::TAG::PLAYER
			&& pos_map.DistanceNoSqrt(target->pos_map) < attack_range_squared
			&& perf_timer.ReadMs() > (double)attack_frequency)
		{
			curr_anim = &attack;
			target->SetLife(target->GetLife() - attack_damage);
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
