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
#include "Obj_Brute.h"
#include "M_Textures.h"
#include "M_ObjManager.h"
#include "M_Render.h"
#include "M_Scene.h"
#include "M_Pathfinding.h"
#include "M_Input.h"
#include "Animation.h"
#include "M_Map.h"
#include "M_Collision.h"
#include "WeaponInfo.h"
#include "M_AnimationBank.h"
#include "Obj_Tank.h"

Obj_Brute::Obj_Brute(fPoint pos) : Object(pos)
{
	pugi::xml_node brute_node = app->config.child("object").child("brute");

	tex = app->tex->Load("textures/Objects/brute-sheet.png");
	curr_tex = tex;

	walk.frames = app->anim_bank->LoadFrames(brute_node.child("animations").child("walk"));
	curr_anim = &walk;

	speed = 1.F;
	range_pos.center = pos_map;
	range_pos.radius = 0.5f;
	follow_range = 10.0f;
	check_path_time = 1.f;
	coll = app->collision->AddCollider(pos, 0.7f, 0.7f, Collider::TAG::ENEMY, 0.f, this);
	coll->AddRigidBody(Collider::BODY_TYPE::DYNAMIC);
	draw_offset = { 70, 35 };
	timer.Start();
}

Obj_Brute::~Obj_Brute()
{
}

bool Obj_Brute::Update(float dt)
{
	switch (state)
	{
	case BRUTE_STATE::GET_PATH:
		path.clear();
		target = (Obj_Tank*)app->objectmanager->GetNearestTank(pos_map);
		if (target != nullptr && pos_map.DistanceManhattan(target->pos_map) <= follow_range)
			if (app->pathfinding->CreatePath((iPoint)pos_map, (iPoint)target->pos_map) != -1)
			{
				std::vector<iPoint> aux = *app->pathfinding->GetLastPath();
				for (std::vector<iPoint>::iterator iter = aux.begin(); iter != aux.end(); ++iter)
				{
					path.push_back({ (*iter).x + 0.5f,(*iter).y + 0.5f });
				}
				state = BRUTE_STATE::RECHEAD_POINT;
			}
		timer.Start();
		break;
	case BRUTE_STATE::MOVE:

		if (IsOnGoal(next_pos))
		{
			path.erase(path.begin());
			state = BRUTE_STATE::RECHEAD_POINT;
		}
		pos_map += move_vect * speed * dt;
		range_pos.center = pos_map;
		break;
	case BRUTE_STATE::RECHEAD_POINT:
	{
		if (timer.ReadSec() >= check_path_time)
			state = BRUTE_STATE::GET_PATH;

		else if (path.size() > 0)
		{
			next_pos = (fPoint)(*path.begin());
			move_vect = (fPoint)(next_pos)-pos_map;
			move_vect.Normalize();

			//Change sprite direction
			angle = atan2(move_vect.y, -move_vect.x)  * RADTODEG /*+ ISO_COMPENSATION*/;
			state = BRUTE_STATE::MOVE;
		}
		else
			state = BRUTE_STATE::GET_PATH;

	}
	break;
	default:
		assert(true && "A brute have no state");
		break;
	}

	if (target != nullptr)
	{
		fPoint enemy_screen_pos = app->map->MapToScreenF(fPoint(this->pos_map.x, this->pos_map.y));
		fPoint target_screen_pos = app->map->MapToScreenF(fPoint(target->pos_map.x, target->pos_map.y));

		if (!attack_available)
		{
			if (BruteCanAttack(enemy_screen_pos, target_screen_pos))
			{
				current_state = CURRENT_BRUTE_POS_STATE::STATE_ATTACKING;
				attack_available = true;
				perf_timer.Start();
			}
			else
			{
				current_state = CURRENT_BRUTE_POS_STATE::STATE_WAITING;
				attack_available = false;
			}
		}

		if (current_state == CURRENT_BRUTE_POS_STATE::STATE_ATTACKING)
		{
			if (perf_timer.ReadMs() > (double)attack_frequency)
			{
				uint target_life = target->GetLife();
				target->SetLife(target_life - 25/*brute damage*/);
				attack_available = false;
			}
		}
	}

	return true;
}


bool Obj_Brute::IsOnGoal(fPoint goal)
{
	return range_pos.IsPointIn(goal);
}

void Obj_Brute::OnTrigger(Collider* collider)
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

bool Obj_Brute::BruteCanAttack(const fPoint& enemy_screen_pos, const fPoint& target_screen_pos) const
{
	return ((enemy_screen_pos.x > target_screen_pos.x && enemy_screen_pos.x < target_screen_pos.x + (float)attack_range.x)
		|| (enemy_screen_pos.x < target_screen_pos.x && enemy_screen_pos.x > target_screen_pos.x - (float)attack_range.x)
		|| (enemy_screen_pos.y > target_screen_pos.y && enemy_screen_pos.y < target_screen_pos.y + (float)attack_range.y)
		|| (enemy_screen_pos.y < target_screen_pos.y && enemy_screen_pos.y > target_screen_pos.y - (float)attack_range.y));
}