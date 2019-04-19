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
//SDL_Rect * Object::GetRect()
//{
//	fPoint screen_pos = app->map->MapToScreenF(pos_map);

//	SDL_Rect rect = curr_anim->GetFrame(angle);
//	rect.x = screen_pos.x - draw_offset.x;
//	rect.y = screen_pos.y - draw_offset.y;

//	return &rect;
//}

//void Object::Draw() {
//	if (current_animation != nullptr)
//		app->render->Blit(data.tileset.texture, pos_map.x, pos_map.y, &current_animation->frames[current_animation->GetNumCurrentFrame()], 1.0F, true);
//	else
//		app->render->Blit(data.tileset.texture, pos_map.x, pos_map.y);
//}

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
	pos_screen = app->map->MapToScreenF(pos_map);

	if (curr_anim != nullptr) {
		frame = curr_anim->GetFrame(angle);
	}
}
