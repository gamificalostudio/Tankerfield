#include <list>

//#include "Brofiler/Brofiler.h"

#include "App.h"
#include "Object.h"

#include "M_ObjManager.h"
#include "M_Render.h"
#include "M_Scene.h"
#include "M_Pathfinding.h"
#include "Log.h"
#include "M_Collision.h"
#include "M_Map.h"
#include "Obj_Explosion.h"
#include "Animation.h"
#include "M_AnimationBank.h"
#include "M_Audio.h"
#include "Obj_Tank.h"
#include "WeaponInfo.h"

Obj_Explosion::Obj_Explosion(fPoint pos):Object(pos)
{
	pugi::xml_node explosion_node = app->config.child("object").child("explosion");

	anim.frames = app->anim_bank->LoadFrames(explosion_node.child("animations").child("explosion"));
	curr_anim = &anim;

	tex = app->tex->Load(explosion_node.child("tex").attribute("path").as_string());
	curr_tex = tex;

	draw_offset.x = 99;
	draw_offset.y = 75;

	frame_damage = 0;

	app->audio->PlayFx(app->audio->GetExplosionFx());
}

bool Obj_Explosion::Update(float dt)
{
	//Create a collider during one frame which deals damage equivalent to int explosion_damage
	if (frame_num == frame_damage)
	{
		float coll_w = 5.f;
		float coll_h = 5.f;

		coll = app->collision->AddCollider(
			pos_map - fPoint(coll_w * 0.5f, coll_h * 0.5f),
			coll_w,
			coll_h,
			TAG::BULLET,
			BODY_TYPE::DYNAMIC,
			explosion_damage,
			nullptr);

		coll->is_sensor = true;
		coll->SetObjOffset(fPoint(coll_w*0.5f, coll_h*0.5f));

	}
	else if (coll != nullptr && frame_num == frame_damage + 1)
	{
		coll->Destroy();
		coll = nullptr;
	}
	frame_num++;

	//Delete this object when the animation has finished
	if (curr_anim != nullptr
		&& curr_anim->Finished())
	{
		to_remove = true;
	}
	return true;
}

void Obj_Explosion::SetExplosionDamage(float damage)
{
	explosion_damage = damage;
}