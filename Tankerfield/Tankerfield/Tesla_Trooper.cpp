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

	tesla = App->tex->Load("Assets/Sprites/tesla.png");

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

void TeslaTrooper::Draw(float dt)
{
	iPoint mouseposition;
	App->input->GetMousePosition(mouseposition.x, mouseposition.y);
	App->render->Blit(tesla, mouseposition.x, mouseposition.y,current_animation->GetCurrentFrame);
}



