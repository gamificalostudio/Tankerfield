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
#include "Obj_Brute.h"
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


Obj_Brute::Obj_Brute(fPoint pos) : Obj_Enemy(pos)
{
	pugi::xml_node brute_node = app->config.child("object").child("enemies").child("brute");

	fire_tex = app->tex->Load(app->anim_bank->animations_xml_node.child("fires").child("animations").child("fire3").attribute("texture").as_string(""));
	tex = app->tex->Load("textures/Objects/enemies/brute-sheet.png");
	tex_damaged = app->tex->Load("textures/Objects/enemies/brute-sheet-white-1.png");
	oiled_tex = app->tex->Load("textures/Objects/enemies/brute-sheet_oiled.png");
	spawn_tex = app->tex->Load("textures/Objects/enemies/spawn_brute.png");
	curr_tex = spawn_tex;
	last_texture = tex;

	pugi::xml_node animation_node = app->anim_bank->animations_xml_node.child("brute").child("animation");
	fire3.frames = app->anim_bank->LoadFrames(app->anim_bank->animations_xml_node.child("fires").child("animations").child("fire3"));
	idle.frames = app->anim_bank->LoadFrames(animation_node.child("idle"));
	walk.frames = app->anim_bank->LoadFrames(animation_node.child("walk"));
	attack.frames = app->anim_bank->LoadFrames(animation_node.child("attack"));
	death.frames = app->anim_bank->LoadFrames(animation_node.child("death"));
	spawn.frames = app->anim_bank->LoadFrames(animation_node.child("spawn"));
	curr_anim = &spawn;


	sfx_hit = app->audio->LoadFx("audio/Fx/entities/enemies/brute/hit.wav", 50);
	sfx_death = app->audio->LoadFx("audio/Fx/entities/enemies/brute/death.wav", 50);
	sfx_attack = app->audio->LoadFx("audio/Fx/entities/enemies/brute/brute_attack.wav", 50);
	sfx_spawn = app->audio->LoadFx("audio/Fx/entities/enemies/brute/spawn.wav", 50);

	state = ENEMY_STATE::SPAWN; 
	detection_range = app->objectmanager->brute_info.detection_range;
	original_speed= speed = app->objectmanager->brute_info.speed;
	range_pos.radius = 1.f;

	spawn_draw_offset = { 260, 274 };
	normal_draw_offset = { 132, 75 };
	electrocuted_draw_offset = { 60,28 };
	draw_offset = spawn_draw_offset;

	angle = 180;//REMOVE
  
	attack_damage = app->objectmanager->brute_info.attack_damage;
	attack_range = app->objectmanager->brute_info.attack_range;
	attack_range_squared = attack_range * attack_range;
	attack_frequency = app->objectmanager->brute_info.attack_frequency;
  
	coll_w = 0.5f;
	coll_h = 0.5f;
  
	damaged_sprite_time = 75;
	life = app->objectmanager->brute_info.life_multiplier * pow(app->objectmanager->brute_info.life_exponential_base, app->scene->round - 1);

	scale = 2.f;
	app->audio->PlayFx(sfx_spawn);
}

Obj_Brute::~Obj_Brute()
{
	if (life_collider != nullptr)
	{
		life_collider->to_destroy = true;
		life_collider = nullptr;
	}
}


void Obj_Brute::ChangeTexture()
{
	if (spawn.Finished() &&
		damaged_sprite_timer.Read() > damaged_sprite_time &&
		curr_tex != tex &&
		state != ENEMY_STATE::STUNNED &&
		state != ENEMY_STATE::STUNNED_CHARGED &&
		oiled == false &&
		bool_electro_dead == false)
	{
		curr_tex = tex;
		damaged = false;
	}
}

void Obj_Brute::Spawn(const float& dt)
{
	if (curr_anim->Finished())
	{
		curr_tex = tex;
		coll = app->collision->AddCollider(pos_map, coll_w, coll_h, TAG::NONE, BODY_TYPE::DYNAMIC, 0.f, this);
		coll->SetObjOffset(fPoint(coll_w * 0.5f, coll_h * 0.5f));
		life_collider = app->collision->AddCollider(pos_map, 2, 2, TAG::ENEMY, BODY_TYPE::DYNAMIC, 0.f, this);
		life_collider->is_sensor = true;
		life_collider->SetObjOffset(fPoint(-1.f, -1.f));
		draw_offset = normal_draw_offset;
		curr_anim = &walk;
		state = ENEMY_STATE::GET_PATH;
	}
}

void Obj_Brute::Burn(const float& dt)
{
	if (burn_fist_enter)
	{
		fire_damage = life / 5;
		curr_anim = &walk;
		
	}
	if (burn_fist_enter || timer_change_direction.ReadSec() >= max_time_change_direction)
	{

		int max_rand = 101;
		int max_rand_double = max_rand * 2;
		float one_divided_by_100 = 0.01f;

		move_vect = { ((rand() % max_rand_double) - max_rand)* one_divided_by_100 ,((rand() % max_rand_double) - max_rand)*one_divided_by_100 };
		move_vect.Normalize();

		angle = atan2(move_vect.y, -move_vect.x)  * RADTODEG - ISO_COMPENSATION;

		timer_change_direction.Start();

		max_time_change_direction = (rand() % max_rand)*one_divided_by_100;
		max_time_change_direction += 0.5f;

		if (burn_fist_enter)
			burn_fist_enter = false;

		life -= fire_damage;
		if (life <= 0)
		{
			state = ENEMY_STATE::DEAD;
		}
	}
	else
	{
		UpdatePos(dt);
	}
	fire3.NextFrame(dt);
}

bool Obj_Brute::Draw(float dt, Camera * camera)
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
	if(draw)
		app->render->BlitScaled(
			curr_tex,
			pos_screen.x - draw_offset.x,
			pos_screen.y - draw_offset.y,
			camera,
			&frame,
			scale,
			scale);

	if (state == ENEMY_STATE::BURN)
	{
		SDL_Rect fire_frame = fire3.GetFrame(0);
		app->render->Blit(fire_tex, pos_screen.x - fire_frame.w*0.5f, pos_screen.y, camera, &fire_frame);
	}
	return true;
}

void Obj_Brute::Attack()
{
	if (life > 0 && app->scene->game_state != GAME_STATE::NO_TYPE)
	{
		if (curr_anim == &attack
			&& curr_anim->Finished())
		{
			target->ReduceLife(attack_damage);
			curr_anim = &idle;//walk?
			attack.Reset();
		}
		else if (target != nullptr
			&& pos_map.DistanceNoSqrt(target->pos_map) <= attack_range_squared
			&& perf_timer.ReadMs() > (double)attack_frequency)
		{
			curr_anim = &attack;
			perf_timer.Start();
			app->audio->PlayFx(sfx_attack);
		}
	}
}

void Obj_Brute::Dead()
{
	if (curr_anim != &death)
	{
		app->pick_manager->CreatePickUp(pos_map, PICKUP_TYPE::WEAPON);
		curr_anim = &death;
		app->audio->PlayFx(sfx_death);
		if (coll != nullptr)
		{
			coll->Destroy();
			coll = nullptr;
		}
		if (life_collider != nullptr)
		{
			life_collider->Destroy();
			life_collider = nullptr;
		}
	}
	else
	{
		if (death.Finished())
		{
			to_remove = true;

		}
	}
	
}

void Obj_Brute::ElectroDead()
{
	if (curr_anim != &electro_dead)
	{
		app->pick_manager->CreatePickUp(pos_map, PICKUP_TYPE::WEAPON);
		bool_electro_dead = true;
		curr_tex = tex_electro_dead;
		curr_anim = &electro_dead;
		app->audio->PlayFx(sfx_death);
		draw_offset = electrocuted_draw_offset;
		if (coll != nullptr)
		{
			coll->Destroy();
			coll = nullptr;
		}
	}
	else
	{
		if (electro_dead.Finished())
		{
			to_remove = true;
			app->audio->PauseFx(channel_electrocuted);

		}
	}
	
}
