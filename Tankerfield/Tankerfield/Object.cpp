#include <list>

//#include "Brofiler/Brofiler.h"

#include "App.h"
#include "Object.h"

#include "M_ObjManager.h"
#include "M_Render.h"
#include "M_Scene.h"
#include "M_Pathfinding.h"
#include "Log.h"
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

bool Object::PostUpdate(float dt, Camera * camera)
{
	fPoint screen_pos = app->map->MapToScreenF(pos_map);

	app->render->Blit(
		curr_tex,
		screen_pos.x - draw_offset.x,
		screen_pos.y - draw_offset.y,
		camera,
		&curr_anim->GetFrame(angle));

	return true;
}

//Returns the rect to be drawn
SDL_Rect * Object::GetRect()
{
	fPoint screen_pos = app->map->MapToScreenF(pos_map);

	SDL_Rect rect;
	rect.x = screen_pos.x - draw_offset.x;
	rect.x = screen_pos.y - draw_offset.y;
	curr_anim->GetFrame(angle);

	return &rect;
}