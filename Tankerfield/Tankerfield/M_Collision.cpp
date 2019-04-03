#include "App.h"
#include "M_Input.h"
#include "M_Render.h"
#include "M_Collision.h"
#include "Object.h"
#include "Defs.h"
#include "Log.h"
#include "Brofiler/Brofiler.h"
#include <math.h>

bool Collider::CheckCollision(Collider*  coll) const
{
	return !(coll->position.x >= (position.x + width) || (coll->position.x + coll->width) <= position.x || coll->position.y >= (position.y + height) || (coll->position.y + coll->height) <= position.y);
}


M_Collision::M_Collision()
{
	name.assign("Module_Collision");

	matrix[(int)Collider::TAG::WALL][(int)Collider::TAG::WALL] = false;
	matrix[(int)Collider::TAG::WALL][(int)Collider::TAG::PLAYER] = false;
	matrix[(int)Collider::TAG::WALL][(int)Collider::TAG::ENEMY] = false;
	matrix[(int)Collider::TAG::WALL][(int)Collider::TAG::GOD] = false;
	matrix[(int)Collider::TAG::WALL][(int)Collider::TAG::BULLET] = false;

	matrix[(int)Collider::TAG::PLAYER][(int)Collider::TAG::WALL] = true;
	matrix[(int)Collider::TAG::PLAYER][(int)Collider::TAG::PLAYER] = true;
	matrix[(int)Collider::TAG::PLAYER][(int)Collider::TAG::ENEMY] = true;
	matrix[(int)Collider::TAG::PLAYER][(int)Collider::TAG::GOD] = true;
	matrix[(int)Collider::TAG::PLAYER][(int)Collider::TAG::BULLET] = false;

	matrix[(int)Collider::TAG::ENEMY][(int)Collider::TAG::WALL] = false;
	matrix[(int)Collider::TAG::ENEMY][(int)Collider::TAG::PLAYER] = false;
	matrix[(int)Collider::TAG::ENEMY][(int)Collider::TAG::ENEMY] = false;
	matrix[(int)Collider::TAG::ENEMY][(int)Collider::TAG::GOD] = false;
	matrix[(int)Collider::TAG::ENEMY][(int)Collider::TAG::BULLET] = false;

	matrix[(int)Collider::TAG::GOD][(int)Collider::TAG::WALL] = false;
	matrix[(int)Collider::TAG::GOD][(int)Collider::TAG::PLAYER] = false;
	matrix[(int)Collider::TAG::GOD][(int)Collider::TAG::ENEMY] = false;
	matrix[(int)Collider::TAG::GOD][(int)Collider::TAG::GOD] = false;
	matrix[(int)Collider::TAG::GOD][(int)Collider::TAG::BULLET] = false;

	matrix[(int)Collider::TAG::BULLET][(int)Collider::TAG::WALL] = true;
	matrix[(int)Collider::TAG::BULLET][(int)Collider::TAG::PLAYER] = false;
	matrix[(int)Collider::TAG::BULLET][(int)Collider::TAG::ENEMY] = false;
	matrix[(int)Collider::TAG::BULLET][(int)Collider::TAG::GOD] = false;
	matrix[(int)Collider::TAG::BULLET][(int)Collider::TAG::BULLET] = false;

}

//Destructor

M_Collision::~M_Collision()
{}

bool M_Collision::Update(float dt)
{
	BROFILER_CATEGORY("M_CollisionUpdate", Profiler::Color::Orange);

	std::list<Collider*> static_colliders;
	std::list<Collider*> dynamic_colliders;
	std::list<Collider*> sensor_colliders;
	std::list<Collider*>::iterator iterator_1;
	std::list<Collider*>::iterator iterator_2;
	Collider* collider_1 = nullptr;
	Collider* collider_2 = nullptr;

	bool do_overlap = false;

	// Destroy colliders ==========================================

	iterator_1 = colliders.begin();

	while (iterator_1 != colliders.end())
	{
		if ((*iterator_1)->to_destroy == true)
		{
			iterator_2 = iterator_1;
			++iterator_2;

			for (std::list<Collider*>::iterator itr = (*iterator_1)->collisions_list.begin(); itr != (*iterator_1)->collisions_list.end(); ++itr)
			{
				std::list<Collider*>::iterator to_destroy = std::find((*itr)->collisions_list.begin(), (*itr)->collisions_list.end(), (*iterator_1));

				if (to_destroy != (*itr)->collisions_list.end())
				{
					LOG("DESTROYED");
					(*itr)->collisions_list.erase(to_destroy);
					break;
				}
			}

			// Destroy =============================

			RELEASE(*iterator_1);
			colliders.erase(iterator_1);

			iterator_1 = iterator_2;

			continue;
		}

		++iterator_1;
	}

	// Fill body types  lists ============================================== 

	for (std::list<Collider*>::iterator itr = colliders.begin(); itr != colliders.end(); ++itr)
	{
		switch ((*itr)->body_type)
		{
		case Collider::BODY_TYPE::STATIC:
			static_colliders.push_back(*itr);
			break;
		case Collider::BODY_TYPE::DYNAMIC:
			dynamic_colliders.push_back(*itr);
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
		iterator_1 = itr_1;
		++iterator_1;

		for (std::list<Collider*>::iterator itr_2 = iterator_1; itr_2 != dynamic_colliders.end(); ++itr_2)
		{
			collider_2 = *itr_2;

			if (collider_1->CheckCollision(collider_2))
			{
				if (matrix[(int)collider_1->tag][(int)collider_2->tag] && collider_1->object)
				{
					collider_1->object->OnTrigger(collider_1);
					do_overlap = true;
				}
				if (matrix[(int)collider_2->tag][(int)collider_1->tag] && collider_2->object)
				{
					collider_2->object->OnTrigger(collider_2);
					do_overlap = true;
				}
				if (do_overlap == true)
				{
					SolveOverlapDD(collider_1, collider_2);
				}
			}

			do_overlap = false;
		}
	}

	// Dynamic VS Static ========================================

	for (std::list<Collider*>::iterator itr_1 = static_colliders.begin(); itr_1 != static_colliders.end(); ++itr_1)
	{
		collider_1 = (*itr_1);

		for (std::list<Collider*>::iterator itr_2 = dynamic_colliders.begin(); itr_2 != dynamic_colliders.end(); ++itr_2)
		{
			collider_2 = (*itr_2);

			if (collider_1->CheckCollision(collider_2))
			{
				if (matrix[(int)collider_1->tag][(int)collider_2->tag] && collider_1->object)
				{
					collider_1->object->OnTrigger(collider_1);
					do_overlap = true;
				}
				if (matrix[(int)collider_2->tag][(int)collider_1->tag] && collider_2->object)
				{
					collider_2->object->OnTrigger(collider_2);
					do_overlap = true;
				}
				if (do_overlap == true)
				{
					SolveOverlapDS(collider_2, collider_1);
				}

				do_overlap = false;
			}
		}
	}

	// Dynamic VS Sensors ========================================

	for (std::list<Collider*>::iterator itr_1 = sensor_colliders.begin(); itr_1 != sensor_colliders.end(); ++itr_1)
	{
		collider_1 = (*itr_1);

		for (std::list<Collider*>::iterator itr_2 = dynamic_colliders.begin(); itr_2 != dynamic_colliders.end(); ++itr_2)
		{
			collider_2 = (*itr_2);

			if (collider_1->CheckCollision(collider_2))
			{
				DoOnTrigger(collider_1, collider_2);
				DoOnTrigger(collider_2, collider_1);
			}
			else
			{
				if (collider_1->collisions_list.empty() == false)
					DoOnTriggerExit(collider_1, collider_2);
				if (collider_1->collisions_list.empty() == false)
					DoOnTriggerExit(collider_2, collider_1);
			}
		}
	}

	return true;
}

// Called before render is available

bool M_Collision::PostUpdate()
{
	BROFILER_CATEGORY("M_CollisionPostUpdate", Profiler::Color::Orange)
		if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
			debug = !debug;
		}

	if (debug == true)
		return true;

	for (std::list<Collider*>::iterator item = colliders.begin(); item != colliders.end(); ++item)
	{
		switch ((*item)->body_type)
		{
		case Collider::BODY_TYPE::SENSOR:
			app->render->DrawIsometricQuad((*item)->position.x, (*item)->position.y, (*item)->width, (*item)->height, { 0, 255, 0 , 255 });
			break;
		case Collider::BODY_TYPE::DYNAMIC:
			app->render->DrawIsometricQuad((*item)->position.x, (*item)->position.y, (*item)->width, (*item)->height, { 255, 0, 0 , 255 });
			break;
		case Collider::BODY_TYPE::STATIC:
			app->render->DrawIsometricQuad((*item)->position.x, (*item)->position.y, (*item)->width, (*item)->height, { 255, 0, 255 , 255 });
			break;
		}
	}

	return true;
}

// Called before quitting
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

Collider * M_Collision::AddCollider(fPoint pos, float width, float height, Collider::TAG type, Object* object)
{
	Collider* collider = new Collider(pos, width, height, type, object);
	colliders.push_back(collider);
	return  collider;
}

void M_Collision::SolveOverlapDS(Collider * dynamic_col, Collider * static_col)
{
	// Calculate directions ===========================================================
	bool directions[(int)Collider::OVERLAP_DIR::MAX];
	directions[(int)Collider::OVERLAP_DIR::LEFT] = dynamic_col->object->velocity.x > 0;
	directions[(int)Collider::OVERLAP_DIR::RIGHT] = dynamic_col->object->velocity.x < 0;
	directions[(int)Collider::OVERLAP_DIR::UP] = dynamic_col->object->velocity.y > 0;
	directions[(int)Collider::OVERLAP_DIR::DOWN] = dynamic_col->object->velocity.y < 0;

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
		dynamic_col->object->pos_map.x = static_col->position.x - dynamic_col->width;
		break;
	case Collider::OVERLAP_DIR::RIGHT:
		dynamic_col->object->pos_map.x = static_col->position.x + static_col->width;
		break;
	case Collider::OVERLAP_DIR::UP:
		dynamic_col->object->pos_map.y = static_col->position.y - dynamic_col->height;
		break;
	case Collider::OVERLAP_DIR::DOWN:
		dynamic_col->object->pos_map.y = static_col->position.y + static_col->height;
		break;
	}

	dynamic_col->last_overlap = (Collider::OVERLAP_DIR)overlap_dir;
	dynamic_col->SetPos(dynamic_col->object->pos_map.x, dynamic_col->object->pos_map.y);
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
		c1->object->pos_map.x -= distances[(int)Collider::OVERLAP_DIR::RIGHT] * 0.5f;
		c2->object->pos_map.x += distances[(int)Collider::OVERLAP_DIR::RIGHT] * 0.5f;

		break;
	case Collider::OVERLAP_DIR::LEFT:
		c1->object->pos_map.x += distances[(int)Collider::OVERLAP_DIR::LEFT] * 0.5f;
		c2->object->pos_map.x -= distances[(int)Collider::OVERLAP_DIR::LEFT] * 0.5f;
		break;
	case Collider::OVERLAP_DIR::UP:
		c1->object->pos_map.y -= distances[(int)Collider::OVERLAP_DIR::UP] * 0.5f;
		c2->object->pos_map.y += distances[(int)Collider::OVERLAP_DIR::UP] * 0.5f;
		break;
	case Collider::OVERLAP_DIR::DOWN:
		c1->object->pos_map.y += distances[(int)Collider::OVERLAP_DIR::DOWN] * 0.5f;
		c2->object->pos_map.y -= distances[(int)Collider::OVERLAP_DIR::DOWN] * 0.5f;
		break;
	}

	c1->SetPos(c1->object->pos_map.x, c1->object->pos_map.y);
	c2->SetPos(c2->object->pos_map.x, c2->object->pos_map.y);
}

inline void M_Collision::DoOnTrigger(Collider * c1, Collider * c2)
{
	if (std::find(c1->collisions_list.begin(), c1->collisions_list.end(), c2) != c1->collisions_list.end())
	{
		c1->object->OnTrigger(c2);
	}
	else
	{
		c1->collisions_list.push_back(c2);
		c1->object->OnTriggerEnter(c2);
	}
}

inline void M_Collision::DoOnTriggerExit(Collider * c1, Collider * c2)
{
	std::list<Collider*>::iterator iter = std::find(c1->collisions_list.begin(), c1->collisions_list.end(), c2);

	if (iter != c1->collisions_list.end())
	{
		c1->object->OnTriggerExit(c2);
		c1->collisions_list.erase(iter);
	}
}

