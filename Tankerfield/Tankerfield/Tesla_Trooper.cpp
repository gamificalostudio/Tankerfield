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
	walking[0].PushBack({ 528,0,66,76 });
	walking[0].PushBack({ 594,0,66,76 });
	walking[0].PushBack({ 660,0,66,76 });
	walking[0].PushBack({ 726,0,66,76 });
	walking[0].PushBack({ 792,0,66,76 });
	walking[0].PushBack({ 858,0,66,76 });
	walking[0].speed=0.1f;

	walking[1].PushBack({ 924,0,66,76 });
	walking[1].PushBack({ 990,0,66,76 });
	walking[1].PushBack({ 1056,0,66,76 });
	walking[1].PushBack({ 1122,0,66,76 });
	walking[1].PushBack({ 1188,0,66,76 });
	walking[1].PushBack({ 1254,0,66,76 });
	walking[1].speed = 0.1f;

	walking[2].PushBack({ 1320,0,66,76 });
	walking[2].PushBack({ 1386,0,66,76 });
	walking[2].PushBack({ 1452,0,66,76 });
	walking[2].PushBack({ 1518,0,66,76 });
	walking[2].PushBack({ 1584,0,66,76 });
	walking[2].PushBack({ 1650,0,66,76 });
	walking[2].speed = 0.1f;

	walking[3].PushBack({ 1716,0,66,76 });
	walking[3].PushBack({ 1782,0,66,76 });
	walking[3].PushBack({ 1848,0,66,76 });
	walking[3].PushBack({ 1914,0,66,76 });
	walking[3].PushBack({ 1980,0,66,76 });
	walking[3].PushBack({ 0,76,66,76 });
	walking[3].speed = 0.1f;
	//32
	walking[4].PushBack({ 66,76,66,76 });
	walking[4].PushBack({ 132,76,66,76 });
	walking[4].PushBack({ 198,76,66,76 });
	walking[4].PushBack({ 264,76,66,76 });
	walking[4].PushBack({ 330,76,66,76 });
	walking[4].PushBack({ 396,76,66,76 });
	walking[4].speed = 0.1f;
	//38
	walking[5].PushBack({ 462,76,66,76 });
	walking[5].PushBack({ 528,76,66,76 });
	walking[5].PushBack({ 594,76,66,76 });
	walking[5].PushBack({ 660,76,66,76 });
	walking[5].PushBack({ 726,76,66,76 });
	walking[5].PushBack({ 792,76,66,76 });
	walking[5].speed = 0.1f;
	//44
	walking[6].PushBack({ 858,76,66,76 });
	walking[6].PushBack({ 924,76,66,76 });
	walking[6].PushBack({ 990,76,66,76 });
	walking[6].PushBack({ 1056,76,66,76 });
	walking[6].PushBack({ 1122,76,66,76 });
	walking[6].PushBack({ 1188,76,66,76 });
	walking[6].speed = 0.1f;
	//50
	walking[7].PushBack({ 1254,76,66,76 });
	walking[7].PushBack({ 1320,76,66,76 });
	walking[7].PushBack({ 1386,76,66,76 });
	walking[7].PushBack({ 1452,76,66,76 });
	walking[7].PushBack({ 1518,76,66,76 });
	walking[7].PushBack({ 1584,76,66,76 });
	walking[7].speed = 0.1f;
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
	Animation* r_walking=GetRotatedAnimation(walking, 8, 90);
	(*r_walking)->GetCurrentFrame(dt);
	App->render->Blit(texture, position.x, position.y, );
}




