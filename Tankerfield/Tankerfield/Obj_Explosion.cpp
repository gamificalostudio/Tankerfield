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

	coll_explosion = app->collision->AddCollider(pos_map, 3.f, 3.f, Collider::TAG::BULLET, 200.f, nullptr);
	coll_explosion->AddRigidBody(Collider::BODY_TYPE::SENSOR);
}

Obj_Explosion::~Obj_Explosion()
{

}

bool Obj_Explosion::Update(float dt)
{
	return true;
}
