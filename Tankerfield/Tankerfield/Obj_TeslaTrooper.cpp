#include <list>

//#include "Brofiler/Brofiler.h"
#include "PugiXml\src\pugixml.hpp"

#include "Point.h"
#include "Log.h"
#include "Animation.h"

#include "App.h"
#include "Object.h"
#include "M_Textures.h"
#include "M_ObjManager.h"
#include "M_Render.h"
#include "M_Scene.h"
#include "M_Pathfinding.h"
#include "Obj_TeslaTrooper.h"
#include "M_Input.h"
#include "M_Map.h"



//Static variables inicialization
SDL_Texture * Obj_TeslaTrooper::tex = nullptr;

Obj_TeslaTrooper::Obj_TeslaTrooper(fPoint pos) : Object (pos)
{
	//Inicialize animations
	walking = new Animation[8] { {8}, {8}, {8}, {8}, {8}, {8}, {8}, {8} };
	walking[0].PushBack({ 528,0,66,76 });//8
	walking[0].PushBack({ 594,0,66,76 });
	walking[0].PushBack({ 660,0,66,76 });
	walking[0].PushBack({ 726,0,66,76 });
	walking[0].PushBack({ 792,0,66,76 });
	walking[0].PushBack({ 858,0,66,76 });
	walking[0].speed=5.0f;

	walking[1].PushBack({ 924,0,66,76 });//14
	walking[1].PushBack({ 990,0,66,76 });
	walking[1].PushBack({ 1056,0,66,76 });
	walking[1].PushBack({ 1122,0,66,76 });
	walking[1].PushBack({ 1188,0,66,76 });
	walking[1].PushBack({ 1254,0,66,76 });
	walking[1].speed = 5.0f;

	walking[2].PushBack({ 1320,0,66,76 });//20
	walking[2].PushBack({ 1386,0,66,76 });
	walking[2].PushBack({ 1452,0,66,76 });
	walking[2].PushBack({ 1518,0,66,76 });
	walking[2].PushBack({ 1584,0,66,76 });
	walking[2].PushBack({ 1650,0,66,76 });
	walking[2].speed = 5.0f;

	walking[3].PushBack({ 1716,0,66,76 });//26
	walking[3].PushBack({ 1782,0,66,76 });
	walking[3].PushBack({ 1848,0,66,76 });
	walking[3].PushBack({ 1914,0,66,76 });
	walking[3].PushBack({ 1980,0,66,76 });
	walking[3].PushBack({ 0,76,66,76 });
	walking[3].speed = 5.0f;
	//32
	walking[4].PushBack({ 66,76,66,76 });
	walking[4].PushBack({ 132,76,66,76 });
	walking[4].PushBack({ 198,76,66,76 });
	walking[4].PushBack({ 264,76,66,76 });
	walking[4].PushBack({ 330,76,66,76 });
	walking[4].PushBack({ 396,76,66,76 });
	walking[4].speed = 5.0f;
	//38
	walking[5].PushBack({ 462,76,66,76 });
	walking[5].PushBack({ 528,76,66,76 });
	walking[5].PushBack({ 594,76,66,76 });
	walking[5].PushBack({ 660,76,66,76 });
	walking[5].PushBack({ 726,76,66,76 });
	walking[5].PushBack({ 792,76,66,76 });
	walking[5].speed = 5.0f;
	//44
	walking[6].PushBack({ 858,76,66,76 });
	walking[6].PushBack({ 924,76,66,76 });
	walking[6].PushBack({ 990,76,66,76 });
	walking[6].PushBack({ 1056,76,66,76 });
	walking[6].PushBack({ 1122,76,66,76 });
	walking[6].PushBack({ 924,76,66,76 });
	walking[6].speed = 5.0f;
	//49
	walking[7].PushBack({ 1188,76,66,76 });
	walking[7].PushBack({ 1254,76,66,76 });
	walking[7].PushBack({ 1320,76,66,76 });
	walking[7].PushBack({ 1386,76,66,76 });
	walking[7].PushBack({ 1452,76,66,76 });
	walking[7].PushBack({ 1518,76,66,76 });
	walking[7].speed = 5.0f;

	if (tex == nullptr)
	{
		tex = app->tex->Load("textures/Objects/shk-sheet.png");
	}
}

Obj_TeslaTrooper::~Obj_TeslaTrooper()
{
}

bool Obj_TeslaTrooper::Start()
{
	timer.Start();
	return true;
}

bool Obj_TeslaTrooper::PreUpdate()
{
	return true;
}

bool Obj_TeslaTrooper::Update(float dt)
{
	if(app->input->GetKey(SDL_SCANCODE_W)==KEY_DOWN)
	{
		angle += 45;
	}
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
	{
		angle -= 45;
	}

	//if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
	//{
	//	life -= 100;
	//	LOG("life: %i", life);
	//}

	//if (life <= 0)
	//{
	//	death = true;
	//	to_remove = true;
	//}

	if (timer.ReadSec() >= 5)
	{
		target = app->objectmanager->GetNearestTank(pos_map);
		if (target != nullptr)
		{
			//iPoint origin = app->map->ScreenToMapI(pos_map.x, pos_map.y);
			//iPoint destination = app->map->ScreenToMapI(target->pos_map.x, target->pos_map.y);
			if (app->pathfinding->CreatePath((iPoint)pos_map, (iPoint)target->pos_map) != -1)
			{
				path = *app->pathfinding->GetLastPath();
			}
		}
		timer.Start();
	}
	return true;
}

bool Obj_TeslaTrooper::PostUpdate(float dt)
{
	uint ind = GetRotatedIndex(8, angle);
	SDL_Rect rect = walking[ind].GetCurrentFrame(dt, new_current_frame);
	iPoint pos= app->map->MapToScreenI(pos_map.x, pos_map.y);
	app->render->Blit(tex, pos.x, pos.y, &rect);
	SDL_Rect frame = { pos_map.x,pos_map.y,10,10 };
	app->render->DrawQuad(frame,255,0,0,255);
	if (path.size() > 0 && app->scene->path_tex!=nullptr)
	{
		for (std::vector<iPoint>::iterator iter = path.begin(); iter != path.end(); ++iter)
		{
			iPoint pos = app->map->MapToScreenI((*iter).x, (*iter).y);
			app->render->Blit(app->scene->path_tex, pos.x, pos.y);
		}
	}

	return true;
}






