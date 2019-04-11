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

Obj_Explosion::Obj_Explosion(fPoint pos)
{
	pugi::xml_node explosion_node = app->config.child("object").child("explosion");

	time = SDL_GetTicks();
	if (explosion_tex == nullptr)
	{
		Obj_Explosion::explosion_tex = app->tex->Load(explosion_node.child("tex").child("base").text().as_string());
	}
	if (explosion_anim == nullptr)
	{
		explosion_anim = new Animation;
		explosion_anim->LoadAnimation(explosion_node.child("animations").child("explosion"));
	}
	curr_anim = explosion_anim;
	pos_map = pos;
	coll_explosion = app->collision->AddCollider(pos_map, 3.f, 3.f, Collider::TAG::BULLET, 200.f, nullptr);
	coll_explosion->AddRigidBody(Collider::BODY_TYPE::SENSOR);
}

Obj_Explosion::~Obj_Explosion()
{
	coll_explosion->to_destroy = true;
}

bool Obj_Explosion::Update(float dt)
{
	if (SDL_GetTicks()-time>100) 
	{
		to_remove = true;
	}

	return true;
}
