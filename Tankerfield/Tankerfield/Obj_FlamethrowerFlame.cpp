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
#include "Obj_FlamethrowerFlame.h"
#include "Animation.h"
#include "M_AnimationBank.h"
#include "M_Audio.h"
#include "Obj_Tank.h"
#include "WeaponInfo.h"

Obj_FlamethrowerFlame::Obj_FlamethrowerFlame(fPoint pos) :Object(pos)
{
	pugi::xml_node explosion_node = app->config.child("object").child("explosion");

	anim.frames = app->anim_bank->LoadFrames(explosion_node.child("animations").child("explosion"));
	curr_anim = &anim;

	tex = app->tex->Load(explosion_node.child("tex").attribute("path").as_string());
	curr_tex = tex;

	draw_offset.x = 99;
	draw_offset.y = 75;

	coll_w = 2.f;
	coll_h = 2.f;

	coll = app->collision->AddCollider(
		pos_map - fPoint(coll_w*0.5f, coll_h*0.5f),
		coll_w,
		coll_h,
		Collider::TAG::BULLET,
		200 + app->scene->round * 100,
		this);
	coll->AddRigidBody(Collider::BODY_TYPE::SENSOR);
	coll->SetObjOffset({ -coll_w * 0.5f, -coll_h * 0.5f });

	app->audio->PlayFx(app->audio->GetExplosionFx());
}

Obj_FlamethrowerFlame::~Obj_FlamethrowerFlame()
{
}

bool Obj_FlamethrowerFlame::Update(float dt)
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
