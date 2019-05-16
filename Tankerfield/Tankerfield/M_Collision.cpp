	
#include "Brofiler/Brofiler.h"

#include "Log.h"
#include "Defs.h"

#include "App.h"
#include "M_Input.h"
#include "M_Render.h"
#include "M_Collision.h"
#include "Object.h"
#include "Defs.h"
#include "Log.h"
#include "Brofiler/Brofiler.h"
#include <math.h>

void Collider::SetPosToObj()
{
	if (object != nullptr)
	{
		position = object->pos_map + obj_offset;
	}
}

void Collider::Destroy()
{
	to_destroy = true;
}

bool Collider::CheckCollision(Collider*  coll) const
{
	return !(coll->position.x >= (position.x + width) || (coll->position.x + coll->width) <= position.x || coll->position.y >= (position.y + height) || (coll->position.y + coll->height) <= position.y);
}

M_Collision::M_Collision()
{
	name.assign("Module_Collision");

	for (int i = 0; i < (int)Collider::TAG::MAX; ++i)
	{
		for (int j = 0; j < (int)Collider::TAG::MAX; ++j)
		{
			matrix[i][j] = false;
		}
	}

	matrix[(int)Collider::TAG::PLAYER][(int)Collider::TAG::WALL] = true;
	matrix[(int)Collider::TAG::PLAYER][(int)Collider::TAG::WATER] = true;
	matrix[(int)Collider::TAG::PLAYER][(int)Collider::TAG::ENEMY] = true;
	matrix[(int)Collider::TAG::PLAYER][(int)Collider::TAG::FRIENDLY_BULLET] = true;
	matrix[(int)Collider::TAG::PLAYER][(int)Collider::TAG::PICK_UP] = true;
	matrix[(int)Collider::TAG::PLAYER][(int)Collider::TAG::ROAD] = true;
	matrix[(int)Collider::TAG::PLAYER][(int)Collider::TAG::PORTAL] = true;

	matrix[(int)Collider::TAG::BULLET][(int)Collider::TAG::WALL] = true;
	matrix[(int)Collider::TAG::BULLET][(int)Collider::TAG::ENEMY] = true;
	matrix[(int)Collider::TAG::BULLET][(int)Collider::TAG::REWARD_BOX] = true;


	matrix[(int)Collider::TAG::ENEMY][(int)Collider::TAG::BULLET] = true;
	matrix[(int)Collider::TAG::ENEMY][(int)Collider::TAG::FRIENDLY_BULLET] = true;
	matrix[(int)Collider::TAG::ENEMY][(int)Collider::TAG::BULLET_LASER] = true;

	matrix[(int)Collider::TAG::REWARD_ZONE][(int)Collider::TAG::PLAYER] = true;

	matrix[(int)Collider::TAG::REWARD_BOX][(int)Collider::TAG::BULLET] = true;
	matrix[(int)Collider::TAG::REWARD_BOX][(int)Collider::TAG::FRIENDLY_BULLET] = true;
	matrix[(int)Collider::TAG::REWARD_BOX][(int)Collider::TAG::BULLET_LASER] = true;

	matrix[(int)Collider::TAG::FRIENDLY_BULLET][(int)Collider::TAG::WALL] = true;
	matrix[(int)Collider::TAG::FRIENDLY_BULLET][(int)Collider::TAG::ENEMY] = true; 
	matrix[(int)Collider::TAG::FRIENDLY_BULLET][(int)Collider::TAG::PLAYER] = true;
	matrix[(int)Collider::TAG::FRIENDLY_BULLET][(int)Collider::TAG::REWARD_BOX] = true;

	matrix[(int)Collider::TAG::PICK_UP][(int)Collider::TAG::PLAYER] = true;

	matrix[(int)Collider::TAG::BULLET_LASER][(int)Collider::TAG::WALL] = true;

	matrix[(int)Collider::TAG::GOD][(int)Collider::TAG::PICK_UP] = true;
	matrix[(int)Collider::TAG::GOD][(int)Collider::TAG::ROAD] = true;



}

M_Collision::~M_Collision()
{}

bool M_Collision::CleanUp()
{
	LOG("Freeing all colliders");

	// Remove all colliders =====================

	for (std::list<Collider*>::iterator item = colliders.begin(); item != colliders.end(); ++item)
	{
		if (*item != nullptr)
		{
			RELEASE(*item);
		}
	}

	colliders.clear();

	return true;
}

bool M_Collision::Reset()
{

	for (std::list<Collider*>::iterator iter = colliders.begin(); iter != colliders.end(); )
	{
		RELEASE(*iter);
		iter = colliders.erase(iter);
	}

	return true;
}

bool M_Collision::Update(float dt)
{
	BROFILER_CATEGORY("M_CollisionUpdate", Profiler::Color::Orange);

	std::list<Collider*> static_colliders;
	std::list<Collider*> dynamic_colliders;
	std::list<Collider*> sensor_colliders;
	std::list<Collider*> merged_colliders;

	std::list<Collider*>::iterator iterator;

	Collider* collider_1 = nullptr;
	Collider* collider_2 = nullptr;

	// Destroy Colliders =============================================

	DestroyColliders();

	// Fill body types lists & Destroy colliders =====================

	for (std::list<Collider*>::iterator itr = colliders.begin(); itr != colliders.end(); ++itr)
	{
		switch ((*itr)->body_type)
		{
		case Collider::BODY_TYPE::STATIC:
			static_colliders.push_back(*itr);
			merged_colliders.push_back(*itr);
			break;
		case Collider::BODY_TYPE::DYNAMIC:
			dynamic_colliders.push_back(*itr);
			merged_colliders.push_back(*itr);
			break;
		case Collider::BODY_TYPE::SENSOR:
			sensor_colliders.push_back(*itr);
			break;
		}
	}

	// Dynamic VS Dynamic ========================================

	for (std::list<Collider*>::iterator itr_1 = dynamic_colliders.begin(); itr_1 != dynamic_colliders.end(); ++itr_1)
	{
		collider_1 = *itr_1;
		iterator = itr_1;
		++iterator;

		for (std::list<Collider*>::iterator itr_2 = iterator; itr_2 != dynamic_colliders.end(); ++itr_2)
		{
			collider_2 = *itr_2;

			if (collider_1->CheckCollision(collider_2)  &&  collider_1->to_destroy == false && collider_2->to_destroy == false)
			{
				DoOnTrigger(collider_1, collider_2);
				/*DoOnTrigger(collider_2, collider_1);*/
				

				if ((int)collider_1->tag > (int)collider_2->tag)
				{
					SolveOverlapDS(collider_2, collider_1);
				}
				else if ((int)collider_1->tag < (int)collider_2->tag)
				{
					SolveOverlapDS(collider_1, collider_2);
				}
				else
				{
					SolveOverlapDD(collider_1, collider_2);
				}
			}
			else
			{
				if (collider_1->collisions_list.empty() == false)
				{
					DoOnTriggerExit(collider_1, collider_2);
				}
				if (collider_2->collisions_list.empty() == false)
				{
					DoOnTriggerExit(collider_2, collider_1);
				}
			}
		}
	}

	// Dynamic VS Static ========================================

	for (std::list<Collider*>::iterator itr_1 = static_colliders.begin(); itr_1 != static_colliders.end(); ++itr_1)
	{
		collider_1 = (*itr_1);

		for (std::list<Collider*>::iterator itr_2 = dynamic_colliders.begin(); itr_2 != dynamic_colliders.end(); ++itr_2)
		{
			collider_2 = (*itr_2);

			if (collider_1->CheckCollision(collider_2) && collider_1->to_destroy == false && collider_2->to_destroy == false)
			{
				DoOnTrigger(collider_1, collider_2);
		/*		DoOnTrigger(collider_2, collider_1);*/

				SolveOverlapDS(collider_2, collider_1);
			}
			else
			{
				if (collider_1->collisions_list.empty() == false)
				{
					DoOnTriggerExit(collider_1, collider_2);
				}
				if (collider_2->collisions_list.empty() == false)
				{
					DoOnTriggerExit(collider_2, collider_1);
				}
			}
		}
	}

	// Dynamic & Static VS Sensors ================================

	for (std::list<Collider*>::iterator itr_1 = sensor_colliders.begin(); itr_1 != sensor_colliders.end(); ++itr_1)
	{
		collider_1 = (*itr_1);

		for (std::list<Collider*>::iterator itr_2 = merged_colliders.begin(); itr_2 != merged_colliders.end(); ++itr_2)
		{
			collider_2 = (*itr_2);
	
			if (collider_1->CheckCollision(collider_2) && collider_1->to_destroy == false && collider_2->to_destroy == false)
			{
                 DoOnTrigger(collider_1, collider_2);

			}
			else
			{
				if (collider_1->collisions_list.empty() == false)
				{
					DoOnTriggerExit(collider_1, collider_2);
				}
				if (collider_2->collisions_list.empty() == false)
				{
					DoOnTriggerExit(collider_2, collider_1);
				}
			}
		}
	}

	return true;
}


bool M_Collision::PostUpdate(float dt)
{
	BROFILER_CATEGORY("M_CollisionPostUpdate", Profiler::Color::Orange);

	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
		debug = !debug;
	}

	if (debug == false)
	{
		return true;
	}
	for (std::vector<Camera*>::iterator item_cam = app->render->cameras.begin(); item_cam != app->render->cameras.end(); ++item_cam)
	{
		for (std::list<Collider*>::iterator item = colliders.begin(); item != colliders.end(); ++item)
		{
			SDL_RenderSetClipRect(app->render->renderer, &(*item_cam)->screen_section);

			if ((*item)->to_destroy == true)
			{
				continue;
			}

			switch ((*item)->body_type)
			{
			case Collider::BODY_TYPE::SENSOR:
				app->render->DrawIsometricQuad((*item)->position.x, (*item)->position.y, (*item)->width, (*item)->height, { 0, 255, 0 , 255 }, (*item_cam));
				break;
			case Collider::BODY_TYPE::DYNAMIC:
				app->render->DrawIsometricQuad((*item)->position.x, (*item)->position.y, (*item)->width, (*item)->height, { 255, 0, 0 , 255 }, (*item_cam));
				break;
			case Collider::BODY_TYPE::STATIC:
				app->render->DrawIsometricQuad((*item)->position.x, (*item)->position.y, (*item)->width, (*item)->height, { 255, 0, 255 , 255 }, (*item_cam));
				break;
			}

			SDL_RenderSetClipRect(app->render->renderer, nullptr);
		}
	}
	return true;
}

void M_Collision::DestroyColliders()
{
	std::list<Collider*>::iterator iterator = colliders.begin();;

	while (iterator != colliders.end())
	{
		if ((*iterator)->to_destroy == true)
		{
			//switch ((*iterator)->tag)
			//{
			//case Collider::TAG::BULLET:
			//	LOG("Destroyed Bullet Collider");
			//	break;
			//case Collider::TAG::BULLET_LASER:
			//	LOG("Destroyed Bullet Laser Collider");
			//	break;
			//case Collider::TAG::ENEMY:
			//	LOG("Destroyed Enemy Collider");
			//	break;
			//case Collider::TAG::FRIENDLY_BULLET:
			//	LOG("Destroyed Friendly Bullet Collider");
			//	break;
			//case Collider::TAG::GOD:
			//	LOG("Destroyed God Collider");
			//	break;
			//case Collider::TAG::PICK_UP:
			//	LOG("Destroyed PickUp Collider");
			//	break;
			//case Collider::TAG::PLAYER:
			//	LOG("Destroyed Player Collider");
			//	break;
			//case Collider::TAG::REWARD_BOX:
			//	LOG("Destroyed Reward Box Collider");
			//	break;
			//case Collider::TAG::REWARD_ZONE:
			//	LOG("Destroyed Reward Zone Collider");
			//	break;
			//case Collider::TAG::ROAD:
			//	LOG("Destroyed Road Collider");
			//	break;
			//case Collider::TAG::WATER:
			//	LOG("Destroyed Water Collider");
			//	break;
			//case Collider::TAG::NONE:
			//	LOG("Destroyed None Collider");
			//	break;
			//}

			// Destroy from current colliders on collision ==============

			for (std::list<Collider*>::iterator itr = (*iterator)->collisions_list.begin(); itr != (*iterator)->collisions_list.end(); ++itr)
			{
				std::list<Collider*>::iterator to_destroy = std::find((*itr)->collisions_list.begin(), (*itr)->collisions_list.end(), (*iterator));

				if (to_destroy != (*itr)->collisions_list.end())
				{
					(*itr)->collisions_list.erase(to_destroy);
				}
			}

			// Destroy ==================================================

			RELEASE(*iterator);
			iterator = colliders.erase(iterator);

			continue;
		}
		++iterator;
	}
}

Collider * M_Collision::AddCollider(fPoint pos, float width, float height, Collider::TAG type, float damage, Object* object)
{
	Collider* collider = new Collider(pos, width, height, damage, type, object);
	colliders.push_back(collider);
	return  collider;
}

Collider * M_Collision::AddCollider(float x, float y, float width, float height, Collider::TAG type, float damage, Object * object)
{
	fPoint pos(x, y);
	Collider* collider = new Collider(pos, width, height, damage, type, object);
	colliders.push_back(collider);
	return  collider;
}

void M_Collision::SolveOverlapDS(Collider * dynamic_col, Collider * static_col)
{
	// Calculate between colliders overlap ============================================
	float distances[(int)Collider::OVERLAP_DIR::MAX];
	distances[(int)Collider::OVERLAP_DIR::LEFT] = dynamic_col->position.x + dynamic_col->width - static_col->position.x;
	distances[(int)Collider::OVERLAP_DIR::RIGHT] = static_col->position.x + static_col->width - dynamic_col->position.x;
	distances[(int)Collider::OVERLAP_DIR::UP] = dynamic_col->position.y + dynamic_col->height - static_col->position.y;
	distances[(int)Collider::OVERLAP_DIR::DOWN] = static_col->position.y + static_col->height - dynamic_col->position.y;

	int overlap_dir = -1;

	for (int i = 0; i < (int)Collider::OVERLAP_DIR::MAX; ++i)
	{
		if (overlap_dir == -1)
		{
			overlap_dir = i;
		}
		else if (distances[i] == distances[(int)overlap_dir])
		{
			if ((Collider::OVERLAP_DIR)i == dynamic_col->last_overlap)
			{
				overlap_dir = i;
			}
		}
		else if (distances[i] < distances[(int)overlap_dir])
		{
			overlap_dir = i;
		}
	}

	switch ((Collider::OVERLAP_DIR)overlap_dir)
	{
	case Collider::OVERLAP_DIR::LEFT:
		dynamic_col->position.x = static_col->position.x - dynamic_col->width;
		break;
	case Collider::OVERLAP_DIR::RIGHT:
		dynamic_col->position.x = static_col->position.x + static_col->width;
		break;
	case Collider::OVERLAP_DIR::UP:
		dynamic_col->position.y = static_col->position.y - dynamic_col->height;
		break;
	case Collider::OVERLAP_DIR::DOWN:
		dynamic_col->position.y = static_col->position.y + static_col->height;
		break;
	}

	dynamic_col->object->pos_map = dynamic_col->position - dynamic_col->obj_offset;
	dynamic_col->last_overlap = (Collider::OVERLAP_DIR)overlap_dir;

}


void M_Collision::SolveOverlapDD(Collider * c1, Collider * c2)
{
	// Calculate between colliders overlap (c1 reference collider) ========================

	float distances[(int)Collider::OVERLAP_DIR::MAX];
	distances[(int)Collider::OVERLAP_DIR::RIGHT] = c1->position.x + c1->width - c2->position.x;
	distances[(int)Collider::OVERLAP_DIR::LEFT] = c2->position.x + c2->width - c1->position.x;
	distances[(int)Collider::OVERLAP_DIR::UP] = c1->position.y + c1->height - c2->position.y;
	distances[(int)Collider::OVERLAP_DIR::DOWN] = c2->position.y + c2->height - c1->position.y;

	int overlap_dir = -1;

	for (uint i = 0; i < (int)Collider::OVERLAP_DIR::MAX; ++i)
	{
		if (overlap_dir == -1)
			overlap_dir = i;
		else if (distances[i] < distances[(int)overlap_dir])
			overlap_dir = i;
	}

	switch ((Collider::OVERLAP_DIR)overlap_dir)
	{
	case Collider::OVERLAP_DIR::RIGHT:
		c1->position.x -= distances[(int)Collider::OVERLAP_DIR::RIGHT] * 0.5f;
		c2->position.x += distances[(int)Collider::OVERLAP_DIR::RIGHT] * 0.5f;

		break;
	case Collider::OVERLAP_DIR::LEFT:
		c1->position.x += distances[(int)Collider::OVERLAP_DIR::LEFT] * 0.5f;
		c2->position.x -= distances[(int)Collider::OVERLAP_DIR::LEFT] * 0.5f;
		break;
	case Collider::OVERLAP_DIR::UP:
		c1->position.y -= distances[(int)Collider::OVERLAP_DIR::UP] * 0.5f;
		c2->position.y += distances[(int)Collider::OVERLAP_DIR::UP] * 0.5f;
		break;
	case Collider::OVERLAP_DIR::DOWN:
		c1->position.y += distances[(int)Collider::OVERLAP_DIR::DOWN] * 0.5f;
		c2->position.y -= distances[(int)Collider::OVERLAP_DIR::DOWN] * 0.5f;
		break;
	}

	c1->object->pos_map = c1->position - c1->obj_offset;
	c2->object->pos_map = c2->position - c2->obj_offset;
}

inline void M_Collision::DoOnTrigger(Collider * c1, Collider * c2)
{
	
	if (std::find(c1->collisions_list.begin(), c1->collisions_list.end(), c2) != c1->collisions_list.end())
	{
		if (c1->object != nullptr && matrix[(int)c1->tag][(int)c2->tag] && c1->to_destroy == false && c2->to_destroy == false)
		{
			c1->object->OnTrigger(c2);
		}
	}
	else
	{
		c1->collisions_list.push_back(c2);

		if (c1->object != nullptr && matrix[(int)c1->tag][(int)c2->tag] && c1->to_destroy == false && c2->to_destroy == false)
		{
			c1->object->OnTriggerEnter(c2);
		}
	}

	std::swap(c1, c2);

	if (std::find(c1->collisions_list.begin(), c1->collisions_list.end(), c2) != c1->collisions_list.end())
	{
		if (c1->object != nullptr && matrix[(int)c1->tag][(int)c2->tag] && c1->to_destroy == false && c2->to_destroy == false)
		{
			c1->object->OnTrigger(c2);
		}
	}
	else
	{
		c1->collisions_list.push_back(c2);

		if (c1->object != nullptr && matrix[(int)c1->tag][(int)c2->tag] && c1->to_destroy == false && c2->to_destroy == false)
		{
			c1->object->OnTriggerEnter(c2);
		}
	}


}

inline void M_Collision::DoOnTriggerExit(Collider * c1, Collider * c2)
{
	std::list<Collider*>::iterator iter = std::find(c1->collisions_list.begin(), c1->collisions_list.end(), c2);

	if (iter != c1->collisions_list.end())
	{
		c1->collisions_list.erase(iter);

		if (c1->object != nullptr)
		{
			c1->object->OnTriggerExit(c2);
		}
	}
}
