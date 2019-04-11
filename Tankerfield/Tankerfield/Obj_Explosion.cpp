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

Obj_Explosion::Obj_Explosion(fPoint pos):Object(pos)
{
	pugi::xml_node explosion_node = app->config.child("object").child("explosion");

	if (explosion_tex == nullptr)
	{
		explosion_tex = app->tex->Load(explosion_node.child("tex").attribute("path").as_string());
	}
	if (explosion_anim == nullptr)
	{
		explosion_anim = new Animation;
		explosion_anim->LoadAnimation(explosion_node.child("animations").child("explosion"));
	}
	curr_tex = explosion_tex;
	curr_anim = explosion_anim;


	draw_offset.x = 99;
	draw_offset.y = 10;

	coll_explosion = app->collision->AddCollider(pos_map, 3.f, 3.f, Collider::TAG::BULLET, 200.f, nullptr);
	coll_explosion->AddRigidBody(Collider::BODY_TYPE::SENSOR);
}

Obj_Explosion::~Obj_Explosion()
{
	coll_explosion->to_destroy = true;
}

bool Obj_Explosion::Update(float dt)
{
	if (curr_anim != nullptr&&curr_anim->Finished())
	{
		to_remove = true;
	}

	return true;
}
