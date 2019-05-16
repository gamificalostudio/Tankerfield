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
	pugi::xml_node brute_node = app->config.child("object").child("brute");

	tex = app->tex->Load("textures/Objects/enemies/brute-sheet.png");
	tex_damaged = app->tex->Load("textures/Objects/enemies/brute-sheet-white-1.png");
	spawn_tex = app->tex->Load("textures/Objects/enemies/spawn_brute.png");
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

	state = ENEMY_STATE::SPAWN;
	speed = 7.5f;
	detection_range = 10.0f;


	spawn_draw_offset = { 260, 274 };
	normal_draw_offset = { 132, 75 };
	draw_offset = spawn_draw_offset;

	angle = 180;//REMOVE

	attack_damage = INT_MAX; // We don't really need a value for attack damage, as the suicidal will one shot the target.
	attack_range = 1;
	attack_range_squared = attack_range * attack_range;
	attack_frequency = 3000.0f;

	coll_w = 1.f;
	coll_h = 1.f;

	damaged_sprite_time = 150;
	life = 750 * (log(app->scene->round) + 2);
	app->audio->PlayFx(sfx_spawn);
}

Obj_Suicidal::~Obj_Suicidal()
{

}

bool Obj_Suicidal::Update(float dt)
{
	Movement(dt);
	Obj_Enemy::Update(dt);

	return true;
}

void Obj_Suicidal::ChangeTexture()
{
	if (spawn.Finished() && damaged_sprite_timer.Read() > damaged_sprite_time)
	{
		curr_tex = tex;
	}
}

void Obj_Suicidal::Movement(float &dt)
{

	if ((state != ENEMY_STATE::DEAD)
		&& (state != ENEMY_STATE::SPAWN)
		&& (path_timer.ReadSec() >= check_path_time))
	{
		state = ENEMY_STATE::GET_PATH;
	}
}

void Obj_Suicidal::Spawn()
{
	if (curr_anim->Finished())
	{
		curr_tex = tex;
		coll = app->collision->AddCollider(pos_map, coll_w, coll_h, TAG::ENEMY, BODY_TYPE::DYNAMIC, 0.f, this);
		coll->SetObjOffset(fPoint(coll_w * 0.5f, coll_h * 0.5f));
		draw_offset = normal_draw_offset;
		curr_anim = &walk;
		state = ENEMY_STATE::GET_PATH;
	}
}


