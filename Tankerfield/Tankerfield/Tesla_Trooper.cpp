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

	walking = new Animation[8];
	walking[0].PushBack();
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

void TeslaTrooper::Draw(float dt, SDL_Texture * texture)
{

}




