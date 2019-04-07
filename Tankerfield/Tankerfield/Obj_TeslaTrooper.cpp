#include <list>
#include <assert.h>
#include <vector>

#include "Brofiler/Brofiler.h"
#include "PugiXml\src\pugixml.hpp"

#include "Point.h"
#include "Log.h"
#include "Animation.h"

#include "App.h"
#include "Object.h"
#include "Obj_TeslaTrooper.h"
#include "M_Textures.h"
#include "M_ObjManager.h"
#include "M_Render.h"
#include "M_Scene.h"
#include "M_Pathfinding.h"
#include "M_Input.h"
#include "Animation.h"
#include "M_Map.h"
#include "M_Collision.h"
#include "Weapon.h"

//Static variables inicialization
SDL_Texture * Obj_TeslaTrooper::tex = nullptr;

Obj_TeslaTrooper::Obj_TeslaTrooper(fPoint pos) : Object (pos)
{
	//Inicialize animations
	animation = new Animation[8] { {8}, {8}, {8}, {8}, {8}, {8}, {8}, {8} };
	animation[0].PushBack({ 528,0,66,76 });//8
	animation[0].PushBack({ 594,0,66,76 });
	animation[0].PushBack({ 660,0,66,76 });
	animation[0].PushBack({ 726,0,66,76 });
	animation[0].PushBack({ 792,0,66,76 });
	animation[0].PushBack({ 858,0,66,76 });
	animation[0].speed=5.0f;

	animation[1].PushBack({ 924,0,66,76 });//14
	animation[1].PushBack({ 990,0,66,76 });
	animation[1].PushBack({ 1056,0,66,76 });
	animation[1].PushBack({ 1122,0,66,76 });
	animation[1].PushBack({ 1188,0,66,76 });
	animation[1].PushBack({ 1254,0,66,76 });
	animation[1].speed = 5.0f;

	animation[2].PushBack({ 1320,0,66,76 });//20
	animation[2].PushBack({ 1386,0,66,76 });
	animation[2].PushBack({ 1452,0,66,76 });
	animation[2].PushBack({ 1518,0,66,76 });
	animation[2].PushBack({ 1584,0,66,76 });
	animation[2].PushBack({ 1650,0,66,76 });
	animation[2].speed = 5.0f;

	animation[3].PushBack({ 1716,0,66,76 });//26
	animation[3].PushBack({ 1782,0,66,76 });
	animation[3].PushBack({ 1848,0,66,76 });
	animation[3].PushBack({ 1914,0,66,76 });
	animation[3].PushBack({ 1980,0,66,76 });
	animation[3].PushBack({ 0,76,66,76 });
	animation[3].speed = 5.0f;
	//32
	animation[4].PushBack({ 66,76,66,76 });
	animation[4].PushBack({ 132,76,66,76 });
	animation[4].PushBack({ 198,76,66,76 });
	animation[4].PushBack({ 264,76,66,76 });
	animation[4].PushBack({ 330,76,66,76 });
	animation[4].PushBack({ 396,76,66,76 });
	animation[4].speed = 5.0f;
	//38
	animation[5].PushBack({ 462,76,66,76 });
	animation[5].PushBack({ 528,76,66,76 });
	animation[5].PushBack({ 594,76,66,76 });
	animation[5].PushBack({ 660,76,66,76 });
	animation[5].PushBack({ 726,76,66,76 });
	animation[5].PushBack({ 792,76,66,76 });
	animation[5].speed = 5.0f;
	//44
	animation[6].PushBack({ 858,76,66,76 });
	animation[6].PushBack({ 924,76,66,76 });
	animation[6].PushBack({ 990,76,66,76 });
	animation[6].PushBack({ 1056,76,66,76 });
	animation[6].PushBack({ 1122,76,66,76 });
	animation[6].PushBack({ 924,76,66,76 });
	animation[6].speed = 5.0f;
	//49
	animation[7].PushBack({ 1188,76,66,76 });
	animation[7].PushBack({ 1254,76,66,76 });
	animation[7].PushBack({ 1320,76,66,76 });
	animation[7].PushBack({ 1386,76,66,76 });
	animation[7].PushBack({ 1452,76,66,76 });
	animation[7].PushBack({ 1518,76,66,76 });
	animation[7].speed = 5.0f;

	if (tex == nullptr)
	{
		tex = app->tex->Load("textures/Objects/shk-sheet.png");
	}


	speed				= 1.5F;
	range_pos.center	= pos_map;
	range_pos.radius	= 0.5f;
	follow_range		= 10.0f;
	check_path_time		= 1.f;
	coll				= app->collision->AddCollider(pos, 0.5f, 0.5f, Collider::TAG::ENEMY,0.f, this);
	coll->AddRigidBody(Collider::BODY_TYPE::DYNAMIC);
}

Obj_TeslaTrooper::~Obj_TeslaTrooper()
{
}

bool Obj_TeslaTrooper::Start()
{

	SetRect(0, 0, 66, 76);
	SetPivot(0, 0);
	draw_offset = { 32, 38 };
	coll->SetObjOffset({ -.25f, -.25f });

	timer.Start();

	return true;
}

bool Obj_TeslaTrooper::PreUpdate()
{
	return true;
}

bool Obj_TeslaTrooper::Update(float dt)
{
	switch (state)
	{
	case TROOPER_STATE::GET_PATH:
		path.clear();
		target = app->objectmanager->GetNearestTank(pos_map);
		if (target != nullptr && pos_map.DistanceManhattan(target->pos_map) <= follow_range)
			if (app->pathfinding->CreatePath((iPoint)pos_map, (iPoint)target->pos_map) != -1)
			{
				std::vector<iPoint> aux = *app->pathfinding->GetLastPath();
				for (std::vector<iPoint>::iterator iter = aux.begin(); iter != aux.end(); ++iter)
				{
					path.push_back({ (*iter).x + 0.5f,(*iter).y + 0.5f });
				}
				state = TROOPER_STATE::RECHEAD_POINT;
			}
		timer.Start();
		break;
	case TROOPER_STATE::MOVE:

			if (IsOnGoal(next_pos))
			{
				path.erase(path.begin());
				state = TROOPER_STATE::RECHEAD_POINT;
			}
			pos_map += move_vect * speed * dt;
			range_pos.center = pos_map;
		break;
	case TROOPER_STATE::RECHEAD_POINT:
		{
			if (timer.ReadSec() >= check_path_time)
				state = TROOPER_STATE::GET_PATH;

			else if (path.size() > 0)
			{
				next_pos = (fPoint)(*path.begin());
				move_vect = (fPoint)(next_pos)-pos_map;
				move_vect.Normalize();

				//Change sprite direction
				angle = atan2(move_vect.y, -move_vect.x)  * RADTODEG + ISO_COMPENSATION;
				state = TROOPER_STATE::MOVE;
			}
			else
				state = TROOPER_STATE::GET_PATH;

		}
		break;
	default:
		assert(true && "A tesla trooper have no state");
		break;
	}

	return true;
}

bool Obj_TeslaTrooper::PostUpdate(float dt)
{

	uint ind = GetRotatedIndex(8, angle);
	SDL_Rect rect = animation[ind].GetCurrentFrame(dt, new_current_frame);
	app->render->Blit(
    tex,
    pos_screen.x - draw_offset.x,
    pos_screen.y - draw_offset.y,
    &rect);
  
	//Draw actual postion
	//SDL_Rect frame = { pos_screen.x,pos_screen.y,10,10 };
	//app->render->DrawQuad(frame,255,0,0,255);
	//if (path.size() >= 2)
	//{
	//	for (std::vector<fPoint>::iterator iter = path.begin(); iter != path.end()-1; ++iter)
	//	{
	//		app->render->DrawIsometricLine((*iter), (*(iter + 1)), {255,255,255,255});
	//	}
	//}


	//fPoint pivot_pos = app->map->MapToScreenF(pivot);
	//SDL_Rect debug_pivot = { pivot.x - 3, pivot.y - 3, 6, 6 };
	//app->render->DrawQuad(debug_pivot, 0, 255, 150);
	return true;
}

bool Obj_TeslaTrooper::IsOnGoal(fPoint goal)
{
	return range_pos.IsPointIn(goal);
}

void Obj_TeslaTrooper::OnTrigger(Collider* collider)
{
	if (collider->GetTag() == Collider::TAG::BULLET)
	{
		
		life -= collider->damage;
		if (life <= 0)
		{
			to_remove = true;
		}
	}
}
