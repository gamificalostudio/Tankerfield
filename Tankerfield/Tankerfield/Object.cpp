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

Object::Object(int x, int y) : position(x, y)
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


