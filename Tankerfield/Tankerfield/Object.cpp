#include <list>

//#include "Brofiler/Brofiler.h"

#include "j1App.h"
#include "Object.h"

#include "ObjectManager.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1Pathfinding.h"
#include "p2Log.h"
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

bool Object::PostUpdate()
{
	return true;
}

uint Object::GetRotatedIndex(uint rect_num, float angle, ROTATION_DIR rot_dir, float fist_rect_dir)
{
	//Account for the spritesheet not starting at the 0 degree rotation
	angle -= fist_rect_dir;
	angle = ClampRotation(angle);
	float num_pos = angle * rect_num / 360;
	float remainder = fmod(num_pos, 1);
	num_pos -= remainder;
	//Select the current or the next frame if the remainder is more than or 0.5
	if (remainder >= 0.5f)
	{
		num_pos = num_pos + 1;
	}
	//If it's the last frame, start over again
	if (num_pos == rect_num)
	{
		num_pos = 0;
	}
	return (uint)num_pos;
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

//sprites in SDL_Rect * rect must be ordered in counter clock-wise direction
//angle should be in degrees
//int rect_num is the number of rectangles that are inside SDL_Rect * rect array
SDL_Rect* Object::GetRotatedSprite(SDL_Rect* rect, int rect_num, float angle, float fist_rect_dir)
{
	angle -= fist_rect_dir;

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

	float angle_part = 360 / rect_num;

	float num_pos = angle / angle_part;

	float remainder = fmod(num_pos, 1);

	if (remainder >= 0.5f)
	{
		num_pos = num_pos - remainder + 1;
	}
	else
	{
		num_pos = num_pos - remainder;
	}

	if (num_pos == rect_num)
	{
		num_pos = 0;
	}

	return &rect[(int)num_pos];
}


//sprites in SDL_Rect * rect must be ordered in counter clock-wise direction
//angle should be in degrees
//int rect_num is the number of rectangles that are inside SDL_Rect * rect array
Animation* Object::GetRotatedAnimation(Animation* animations, int num_animations, float angle, float fist_rect_dir)
{
	angle -= fist_rect_dir;

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

	float angle_part = 360 / num_animations;

	float num_pos = angle / angle_part;

	float remainder = fmod(num_pos, 1);

	if (remainder >= 0.5f)
	{
		num_pos = num_pos - remainder + 1;
	}
	else
	{
		num_pos = num_pos - remainder;
	}

	if (num_pos == num_animations)
	{
		num_pos = 0;
	}

	return &animations[(int)num_pos];
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