#include <list>

//#include "Brofiler/Brofiler.h"
#include "PugiXml\src\pugixml.hpp"
#include "p2Point.h"

#include "j1App.h"
#include "Object.h"

#include "j1App.h"
#include "j1Textures.h"
#include "ObjectManager.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1Pathfinding.h"
#include "Tesla_Trooper.h"
#include "j1Input.h"
//#include "j1Map.h"
//#include "j1Collision.h"

TeslaTrooper::TeslaTrooper(float x, float y) : Object (x,y)
{
	position.x = x;
	position.y = y;

	current_animation = &idle;

	idle.PushBack({ 0,0,1343,1600 });
}

TeslaTrooper::~TeslaTrooper()
{
}

bool TeslaTrooper::PreUpdate(float dt)
{

	return false;
}

bool TeslaTrooper::Update(float dt)
{

	return true;
}





