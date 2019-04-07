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
	return true;
}

//void Object::Draw() {
//	if (current_animation != nullptr)
//		app->render->Blit(data.tileset.texture, pos_map.x, pos_map.y, &current_animation->frames[current_animation->GetNumCurrentFrame()], 1.0F, true);
//	else
//		app->render->Blit(data.tileset.texture, pos_map.x, pos_map.y);
//}

//angle should be in degrees
uint Object::GetRotatedIndex(uint rect_num, float angle, ROTATION_DIR rot_dir, float fist_rect_dir)
{
	//Account for the spritesheet not starting at the 0 degree rotation
	angle -= fist_rect_dir;
	angle = ClampRotation(angle);
	float ind = (angle * rect_num) / 360;
	float remainder = fmod(ind, 1);
	ind -= remainder;
	//Select the current or the next frame if the remainder is more than or 0.5
	if (remainder >= 0.5f)
	{
		ind = ind + 1;
	}

	if (rot_dir == ROTATION_DIR::CLOCKWISE)
	{
		ind = rect_num - ind;
	}

	//If it's the last frame, start over again
	if (ind == rect_num)
	{
		ind = 0;
	}

	return (uint)ind;
}

float Object::ClampRotation(float angle)
{
	if (angle > 360)
	{
		angle = fmod(angle, 360);
	}
	else if (angle < -360)
	{
		angle = fmod(angle, -360);
	}
	if (angle < 0)
	{
		angle += 360;
	}
	return angle;
}

void Object::SetPivot(const float & x, const float & y)
{
	pivot.create(x, y);
}

void Object::SetRect(int x, int y, int w, int h)
{
	frame.x = x;
	frame.y = y;
	frame.w = w;
	frame.h = h;
}

bool Object::LoadRects(pugi::xml_node const & node, SDL_Rect * rects)
{
	//Inicialization
	int i = 0;
	pugi::xml_node frame_node = node.child("frame");

	while (frame_node)
	{
		//Body
		rects[i].x = frame_node.attribute("x").as_int();
		rects[i].y = frame_node.attribute("y").as_int();
		rects[i].w = frame_node.attribute("w").as_int();
		rects[i].h = frame_node.attribute("h").as_int();

		//Increment
		++i;
		frame_node = frame_node.next_sibling("frame");
	}
	return true;
}

bool Object::LoadAnimation(pugi::xml_node & node, Animation & anim)
{
	anim.speed = node.attribute("speed").as_float();
	for (node = node.child("frame"); node; node = node.next_sibling("sprite")) {
		SDL_Rect frame;
		frame.x = node.attribute("x").as_int();
		frame.y = node.attribute("y").as_int();
		frame.w = node.attribute("w").as_int();
		frame.h = node.attribute("h").as_int();
		anim.PushBack(frame);
	}
	return true;
}

void Object::DrawDebug()
{
	SDL_Rect section = { pos_screen.x - draw_offset.x, pos_screen.y - draw_offset.y, frame.w, frame.h };

	switch (type)
	{
	case ObjectType::TANK:
		app->render->DrawQuad(section, 255, 0, 0, 80);
		break;
	case ObjectType::STATIC:
		app->render->DrawQuad(section, 0, 255, 0, 80);
		break;
	case ObjectType::TESLA_TROOPER:
		app->render->DrawQuad(section, 0, 0, 255, 80);
		break;

		default:
			break;
	}

	app->render->DrawCircle(pos_screen.x + pivot.x, pos_screen.y + pivot.y, 3, 0, 255, 0);
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

