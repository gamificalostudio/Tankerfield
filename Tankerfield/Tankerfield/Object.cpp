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
//#include "ModuleCollision.h"

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




