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

Object::Object()
{
}

Object::Object(fPoint pos) : pos_map(pos)
{
}

Object::~Object()
{

}

const Collider * Object::GetCollider() const
{
	return coll;
}

bool Object::Update(float dt)
{
	return true;
}

bool Object::Draw(float dt, Camera * camera)
{
	app->render->Blit(
		curr_tex,
		pos_screen.x - draw_offset.x,
		pos_screen.y - draw_offset.y,
		camera,
		&frame);

	return true;
}

void Object::SetDamage(float damage)
{
	if (coll != nullptr)
	{
		coll->damage = damage;
	}
	else
	{
		LOG("Collider is nullptr");
	}
}

void Object::CalculateDrawVariables()
{
	if (is_isometric == true)
	{
		pos_screen = app->map->MapToScreenF(pos_map);
	}

	if (curr_anim != nullptr) {
		frame = curr_anim->GetFrame(angle);
	}
}
