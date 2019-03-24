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

bool Object::LoadRects(pugi::xml_node & node, SDL_Rect * rects)
{
	node = node.child("frame");
	int i = 0;

	while (node)
	{
		rects[i].x = node.attribute("x").as_int();
		rects[i].y = node.attribute("y").as_int();
		rects[i].w = node.attribute("w").as_int();
		rects[i].h = node.attribute("h").as_int();

		node = node.next_sibling("frame");
		++i;
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




