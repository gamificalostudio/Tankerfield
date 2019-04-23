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

	coll_w = 3.f;
	coll_h = 3.f;

	coll = app->collision->AddCollider(
		pos_map - fPoint(coll_w, coll_h),
		coll_w,
		coll_h,
		Collider::TAG::BULLET,
		200 + app->scene->round * 100,
		nullptr);
	coll->AddRigidBody(Collider::BODY_TYPE::SENSOR);

	app->audio->PlayFx(app->audio->GetExplosionFx());
}

Obj_Explosion::~Obj_Explosion()
{
}

bool Obj_Explosion::Update(float dt)
{
	if (curr_anim != nullptr
		&& curr_anim->Finished())
	{
		to_remove = true;
	}
	if (frame_explosion == 2)
	{
		coll->to_destroy = true;
		coll = nullptr;
	}

	frame_explosion++;
	return true;
}

//void Obj_Explosion::SetExplosionDamage(WeaponInfo info)
//{
//	if (coll != nullptr)
//	{
//		coll->damage = original_damage * info.level_weapon;
//		// TODO HARCODE 
//		if (coll->damage == 0)
//		{
//			coll->damage = original_damage;
//		}
//	}
//	
//}