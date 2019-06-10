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

void Object::DebugSpriteSorting(Camera* camera)
{
	//Draw frame
	SDL_Rect section = { pos_screen.x - draw_offset.x, pos_screen.y - draw_offset.y, frame.w, frame.h };
	app->render->DrawQuad(section, 255, 0, 0, 255, false);

	//Draw pivot
	int width = 4;
	SDL_Rect pivot_section = { pos_screen.x + pivot.x - width * 0.5f, pos_screen.y + pivot.y - width * 0.5f, width, width};
	app->render->DrawQuad(pivot_section, 0, 255, 0, 255, true);
}

bool Object::Desactivate()
{
	if (coll != nullptr)
	{
		coll->SetIsTrigger(false);
	}
	return true;
}

bool Object::Draw(Camera * camera)
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
	if (is_isometric)
	{
		pos_screen = app->map->MapToScreenF(pos_map);
	}

	if (curr_anim != nullptr) {
		frame = curr_anim->GetFrame(angle);
	}
}

void Object::SetMapPos(fPoint map_pos)
{
	this->pos_map = map_pos;
	this->pos_screen = app->map->MapToScreenF(pos_map);
}
