#include <list>

//#include "Brofiler/Brofiler.h"

#include "j1App.h"
#include "Object.h"

#include "ObjectManager.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1Pathfinding.h"
//#include "j1Map.h"
//#include "Player.h"
//#include "j1Collision.h"

Object::Object()
{
}

Object::Object(int x, int y) : pos(x, y)
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

void Object::Draw(float dt)
{
	

}

bool Object::LoadRects(pugi::xml_node const & node, SDL_Rect * rects)
{
	//Inicialization
	int i = 0;
	pugi::xml_node frame_node = node.child("frame");

	while (node)
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




