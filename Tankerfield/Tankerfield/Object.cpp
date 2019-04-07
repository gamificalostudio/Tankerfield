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

bool Object::PostUpdate(float dt)
{
	fPoint screen_pos = app->map->MapToScreenF(pos_map);

	for (std::vector<Camera*>::iterator item_cam = app->render->camera.begin(); item_cam != app->render->camera.end(); item_cam++)
	{
		app->render->Blit(
			curr_tex,
			screen_pos.x - draw_offset.x,
			screen_pos.y - draw_offset.y,
			(*item_cam),
			&curr_anim->GetFrame(angle, dt));
	}

	return true;
}