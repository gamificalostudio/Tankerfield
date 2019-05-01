#include <list>
#include <assert.h>
#include <vector>
#include <tgmath.h>

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


Obj_Brute::Obj_Brute(fPoint pos) : Object(pos)
{
	pugi::xml_node brute_node = app->config.child("object").child("brute");

	tex = app->tex->Load("textures/Objects/brute-sheet.png");
	tex_damaged = app->tex->Load("textures/Objects/brute-sheet-white.png");
	spawn_tex = app->tex->Load("textures/Objects/spawn_brute.png");
	curr_tex = spawn_tex;

	idle.frames = app->anim_bank->LoadFrames(brute_node.child("animations").child("idle"));
	walk.frames = app->anim_bank->LoadFrames(brute_node.child("animations").child("walk"));
	attack.frames = app->anim_bank->LoadFrames(brute_node.child("animations").child("attack"));
	death.frames = app->anim_bank->LoadFrames(brute_node.child("animations").child("death"));
	spawn.frames = app->anim_bank->LoadFrames(brute_node.child("animations").child("spawn"));
	curr_anim = &spawn;

	sfx_hit = app->audio->LoadFx("audio/Fx/entities/enemies/brute/hit.wav", 50);
	sfx_death = app->audio->LoadFx("audio/Fx/entities/enemies/brute/death.wav", 50);
	sfx_attack = app->audio->LoadFx("audio/Fx/entities/enemies/brute/brute_attack.wav", 50);
	sfx_spawn = app->audio->LoadFx("audio/Fx/entities/enemies/brute/spawn.wav", 50);

	app->audio->PlayFx(sfx_spawn);

	state = BRUTE_STATE::SPAWN;
	speed = 1.5f;
	detection_range = 10.0f;
	range_pos.center = pos_map;
	range_pos.radius = 0.5f;
	check_path_time = 1.f;

	spawn_draw_offset = { 260, 274 };
	normal_draw_offset = { 132, 75 };
	draw_offset = spawn_draw_offset;

	angle = 180;//REMOVE

	timer.Start();
	attack_damage = 10;
	attack_range = 1;
	attack_range_squared = attack_range * attack_range;
	attack_frequency = 3000.0f;
  
	coll_w = 1.f;
	coll_h = 1.f;
  
	damaged_sprite_time = 150;
	life = 750* (log(app->scene->round)+2);
}

Obj_Brute::~Obj_Brute()
{
}


bool Obj_Brute::Update(float dt)
{
	Movement(dt);
	Attack();
	if (spawn.Finished() && damaged_sprite_timer.Read() > damaged_sprite_time)
	{
		curr_tex = tex;
	}
	return true;
}

void Obj_Brute::Attack()
{
	if (target != nullptr 
		&& target->coll->GetTag() == Collider::TAG::PLAYER
		&& pos_map.DistanceNoSqrt(target->pos_map) < attack_range_squared
		&& perf_timer.ReadMs() > (double)attack_frequency)
	{
		curr_anim = &attack;
		app->audio->PlayFx(sfx_attack);
		target->SetLife(target->GetLife() - attack_damage);
		perf_timer.Start();
	}

	if (curr_anim == &attack&&curr_anim->Finished())
	{
		curr_anim = &walk;
		attack.Reset();
	}

}

void Obj_Brute::Movement(float &dt)
{
	if ((state != BRUTE_STATE::DEAD)
		&& (state != BRUTE_STATE::SPAWN)
		&& (timer.ReadSec() >= check_path_time))
	{
		state = BRUTE_STATE::GET_PATH;
	}

	switch (state)
	{
	case BRUTE_STATE::SPAWN:
	{
		if (curr_anim->Finished())
		{
			curr_tex = tex;
			coll = app->collision->AddCollider(pos_map, coll_w, coll_h, Collider::TAG::ENEMY, 0.f, this);
			coll->AddRigidBody(Collider::BODY_TYPE::DYNAMIC);
			coll->SetObjOffset(fPoint(coll_w * 0.5f, coll_h * 0.5f));
			draw_offset = normal_draw_offset;
			curr_anim = &walk;
			state=BRUTE_STATE::GET_PATH;
		}
	}
	break;
	case BRUTE_STATE::GET_PATH:
	{
		path.clear();
		move_vect.SetToZero();
		//curr_anim = &idle;
		target = app->objectmanager->GetNearestTank(pos_map, detection_range);
		if (target != nullptr)
		{
			if (app->pathfinding->CreatePath((iPoint)pos_map, (iPoint)target->pos_map) != -1)
			{

				std::vector<iPoint> aux = *app->pathfinding->GetLastPath();
				for (std::vector<iPoint>::iterator iter = aux.begin(); iter != aux.end(); ++iter)
				{
					path.push_back({ (*iter).x + 0.5f,(*iter).y + 0.5f });
				}
			}


			state = BRUTE_STATE::RECHEAD_POINT;
		}

		timer.Start();
	}
	break;
	case BRUTE_STATE::MOVE:
	{
		if (IsOnGoal(next_pos))
		{
			path.erase(path.begin());
			state = BRUTE_STATE::RECHEAD_POINT;
		}
		pos_map += move_vect * speed * dt;
		range_pos.center = pos_map;
		curr_anim = &walk;
	}
	break;
	case BRUTE_STATE::RECHEAD_POINT:
	{
		if (path.size() > 0)
		{
			next_pos = (fPoint)(*path.begin());
			move_vect = (fPoint)(next_pos)-pos_map;
			move_vect.Normalize();
			//Change sprite direction
			angle = atan2(move_vect.y, -move_vect.x)  * RADTODEG /*+ ISO_COMPENSATION*/;
			state = BRUTE_STATE::MOVE;
		}
		else
			state = BRUTE_STATE::GET_PATH;
	}
	break;
	case BRUTE_STATE::DEAD:
	{
		if (curr_anim != &death)
		{
			curr_anim = &death;
			app->audio->PlayFx(sfx_death);
			if (coll != nullptr)
			{
				coll->Destroy();
				coll = nullptr;
			}
		}
		else if (curr_anim->Finished())
		{
			to_remove = true;
		}
	}
	default:
		assert(true && "A tesla trooper have no state");
		break;
	}
}

bool Obj_Brute::Draw(float dt, Camera * camera)
{
	app->render->BlitScaled(
		curr_tex,
		pos_screen.x - draw_offset.x,
		pos_screen.y - draw_offset.y,
		camera,
		&frame,
		2.f,
		2.f);

	return true;
}

void Obj_Brute::DrawDebug(const Camera* camera)
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

bool Obj_Brute::IsOnGoal(fPoint goal)
{
	return range_pos.IsPointIn(goal);
}

void Obj_Brute::OnTriggerEnter(Collider * collider)
{
	if (collider->GetTag() == Collider::TAG::BULLET_LASER)
	{

		life -= collider->damage;

		damaged_sprite_timer.Start();
		curr_tex = tex_damaged;

		if (life <= 0)
		{
			// DROP A PICK UP ITEM 
			app->pick_manager->PickUpFromEnemy(pos_map);
			state = BRUTE_STATE::DEAD;
		}
		else
		{
			app->audio->PlayFx(sfx_hit);
		}

	}
}

void Obj_Brute::OnTrigger(Collider* collider)
{
	if ((collider->GetTag() == Collider::TAG::BULLET) || (collider->GetTag() == Collider::TAG::FRIENDLY_BULLET))
	{
		life -= collider->damage;

		damaged_sprite_timer.Start();
		curr_tex = tex_damaged;
		collider->SetTag(Collider::TAG::NONE);

		if (life <= 0)
		{
			// DROP A PICK UP ITEM 
			app->pick_manager->PickUpFromEnemy(pos_map, PICKUP_TYPE::WEAPON);
			state = BRUTE_STATE::DEAD;
		}
		else
		{
			app->audio->PlayFx(sfx_hit);
		}

	}
}
