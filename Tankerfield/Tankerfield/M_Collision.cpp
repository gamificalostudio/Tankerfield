	
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

void Collider::ActiveOnTrigger( bool value)
{
	if (app->collision->is_updating)
	{
		app->collision->mod_on_trigger_colliders[this] = value;
	}
	else
	{
		active_on_trigger = value;
	}
}

void Collider::Destroy()
{
	to_destroy = true;
}


bool Collider::GetIsActivated() const
{
	return active_on_trigger;
}

bool Collider::CheckCollision(Collider*  coll) const
{
	return !(coll->position.x >= (position.x + width) || (coll->position.x + coll->width) <= position.x || coll->position.y >= (position.y + height) || (coll->position.y + coll->height) <= position.y);
}

M_Collision::M_Collision()
{
	name.assign("Module_Collision");

	for (int i = 0; i < (int)TAG::MAX; ++i)
	{
		for (int j = 0; j < (int)TAG::MAX; ++j)
		{
			on_trigger_matrix[i][j] = false;
		}
	}

	on_trigger_matrix[(int)TAG::PLAYER][(int)TAG::ENEMY] = true;
	on_trigger_matrix[(int)TAG::PLAYER][(int)TAG::FRIENDLY_BULLET] = true;
	on_trigger_matrix[(int)TAG::PLAYER][(int)TAG::PICK_UP] = true;
	on_trigger_matrix[(int)TAG::PLAYER][(int)TAG::ROAD] = true;
	on_trigger_matrix[(int)TAG::PLAYER][(int)TAG::PORTAL] = true;

	on_trigger_matrix[(int)TAG::ENEMY][(int)TAG::BULLET] = true;
	on_trigger_matrix[(int)TAG::ENEMY][(int)TAG::FRIENDLY_BULLET] = true;
	on_trigger_matrix[(int)TAG::ENEMY][(int)TAG::BULLET_LASER] = true;
	on_trigger_matrix[(int)TAG::ENEMY][(int)TAG::ELECTRO_SHOT] = true;
	on_trigger_matrix[(int)TAG::ENEMY][(int)TAG::BULLET_OIL] = true;
	on_trigger_matrix[(int)TAG::ENEMY][(int)TAG::OIL_POOL] = true;


	on_trigger_matrix[(int)TAG::BULLET][(int)TAG::WALL] = true;
	on_trigger_matrix[(int)TAG::BULLET][(int)TAG::ENEMY] = true;
	on_trigger_matrix[(int)TAG::BULLET][(int)TAG::REWARD_BOX] = true;
	on_trigger_matrix[(int)TAG::BULLET_OIL][(int)TAG::WALL] = true;
	on_trigger_matrix[(int)TAG::BULLET_OIL][(int)TAG::ENEMY] = true;
	on_trigger_matrix[(int)TAG::BULLET_OIL][(int)TAG::REWARD_BOX] = true;

	on_trigger_matrix[(int)TAG::REWARD_ZONE][(int)TAG::PLAYER] = true;

	on_trigger_matrix[(int)TAG::REWARD_BOX][(int)TAG::BULLET] = true;
	on_trigger_matrix[(int)TAG::REWARD_BOX][(int)TAG::FRIENDLY_BULLET] = true;
	on_trigger_matrix[(int)TAG::REWARD_BOX][(int)TAG::BULLET_LASER] = true;

	on_trigger_matrix[(int)TAG::FRIENDLY_BULLET][(int)TAG::WALL] = true;
	on_trigger_matrix[(int)TAG::FRIENDLY_BULLET][(int)TAG::ENEMY] = true; 
	on_trigger_matrix[(int)TAG::FRIENDLY_BULLET][(int)TAG::PLAYER] = true;
	on_trigger_matrix[(int)TAG::FRIENDLY_BULLET][(int)TAG::REWARD_BOX] = true;

	on_trigger_matrix[(int)TAG::PICK_UP][(int)TAG::PLAYER] = true;
	on_trigger_matrix[(int)TAG::BULLET_LASER][(int)TAG::WALL] = true;
	on_trigger_matrix[(int)TAG::GOD][(int)TAG::PICK_UP] = true;
	on_trigger_matrix[(int)TAG::GOD][(int)TAG::ROAD] = true;


	for (int i = 0; i < (int)TAG::MAX; ++i)
	{
		for (int j = 0; j < (int)TAG::MAX; ++j)
		{
			solve_overlap_matrix[i][j] = false;
		}
	}

	solve_overlap_matrix[(int)TAG::PLAYER][(int)TAG::PLAYER]      = true;
	solve_overlap_matrix[(int)TAG::PLAYER][(int)TAG::WALL]	      = true;
	solve_overlap_matrix[(int)TAG::PLAYER][(int)TAG::WATER]		  = true;
	solve_overlap_matrix[(int)TAG::PLAYER][(int)TAG::REWARD_BOX]  = true;
	solve_overlap_matrix[(int)TAG::PLAYER][(int)TAG::ENEMY]       = true;

	solve_overlap_matrix[(int)TAG::ENEMY][(int)TAG::ENEMY] = true;
	solve_overlap_matrix[(int)TAG::ENEMY][(int)TAG::PLAYER] = true;
	solve_overlap_matrix[(int)TAG::ENEMY][(int)TAG::WALL] = true;
	solve_overlap_matrix[(int)TAG::ENEMY][(int)TAG::WATER] = true;
	solve_overlap_matrix[(int)TAG::ENEMY][(int)TAG::REWARD_BOX] = true;

}

M_Collision::~M_Collision()
{}

bool M_Collision::CleanUp()
{
	LOG("Freeing all colliders");

	// Remove all colliders =====================

	colliders_to_add.clear();

	for (std::list<Collider*>::iterator item = dynamic_colliders.begin(); item != dynamic_colliders.end(); ++item)
	{
		if (*item != nullptr)
		{
			RELEASE(*item);
		}
	}

	dynamic_colliders.clear();

	for (std::list<Collider*>::iterator item = static_colliders.begin(); item != static_colliders.end(); ++item)
	{
		if (*item != nullptr)
		{
			RELEASE(*item);
		}
	}

	static_colliders.clear();

	return true;
}

bool M_Collision::Reset()
{

	CleanUp();

	return true;
}

bool M_Collision::Update(float dt)
{
	BROFILER_CATEGORY("M_CollisionUpdate", Profiler::Color::Orange);

	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) 
	{
		debug = !debug;
	}

	is_updating = true;

	// Fill body types lists ====================================================================

	if (! colliders_to_add.empty())
	{
		for (std::list<Collider*>::iterator itr = colliders_to_add.begin(); itr != colliders_to_add.end(); ++itr)
		{
			switch ((*itr)->body_type)
			{
			case BODY_TYPE::STATIC:
				static_colliders.push_back(*itr);
				break;
			case BODY_TYPE::DYNAMIC:
				dynamic_colliders.push_back(*itr);
				break;
			}
		}
		colliders_to_add.clear();
	}
	
	if (!mod_on_trigger_colliders.empty())
	{
		for (std::map<Collider*, bool>::iterator itr = mod_on_trigger_colliders.begin(); itr != mod_on_trigger_colliders.end(); ++itr)
		{
			(*itr).first->active_on_trigger = (*itr).second;
		}

		mod_on_trigger_colliders.clear();
	}

	// Check collisions =========================================================================

	std::list<Collider*>::iterator iterator;

	Collider* collider_1 = nullptr;
	Collider* collider_2 = nullptr;

	bool on_collision = false;

	// Dynamic VS Dynamic ========================================

	for (std::list<Collider*>::iterator itr_1 = dynamic_colliders.begin(); itr_1 != dynamic_colliders.end(); ++itr_1)
	{
		collider_1 = *itr_1;
		iterator = itr_1;
		++iterator;

		for (std::list<Collider*>::iterator itr_2 = iterator; itr_2 != dynamic_colliders.end(); ++itr_2)
		{
			collider_2 = *itr_2;

			if (collider_1->to_destroy == true || collider_2->to_destroy == true)
			{
				continue;
			}

			on_collision = collider_1->CheckCollision(collider_2);

			// Solve Overlap  ==================================================================

			if (on_collision && (solve_overlap_matrix[(int)collider_1->tag][(int)collider_2->tag] || solve_overlap_matrix[(int)collider_2->tag][(int)collider_1->tag]) && collider_1->is_sensor == false && collider_2->is_sensor == false)
			{
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

			// Do OnTrigger Functions ===========================================================

			if (collider_1->active_on_trigger == true && collider_2->active_on_trigger == true)
			{
				if (on_collision == true)
				{
					DoOnTrigger(collider_1, collider_2);
				}
				else
				{
					DoOnTriggerExit(collider_1, collider_2);
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

			if (collider_1->to_destroy == true || collider_2->to_destroy == true)
			{
				continue;
			}

			on_collision = collider_1->CheckCollision(collider_2);

			// Solve Overlap  ==================================================================

			if (on_collision && (solve_overlap_matrix[(int)collider_1->tag][(int)collider_2->tag] || solve_overlap_matrix[(int)collider_2->tag][(int)collider_1->tag]) && collider_2->is_sensor == false)
			{
				SolveOverlapDS(collider_2, collider_1);
			}

			// Do OnTrigger Functions ===========================================================

			if (collider_1->active_on_trigger == true && collider_2->active_on_trigger == true)
			{
				if (on_collision == true)
				{
					DoOnTrigger(collider_1, collider_2);
				}
				else
				{
					DoOnTriggerExit(collider_1, collider_2);
				}
			}
		}
	}

	// Destroy Colliders =============================================

	DestroyColliders();

	is_updating = false;

	return true;
}


bool M_Collision::PostUpdate(float dt)
{
	BROFILER_CATEGORY("M_CollisionPostUpdate", Profiler::Color::Orange);

	if (debug == false)
	{
		return true;
	}

	for (std::vector<Camera*>::iterator item_cam = app->render->cameras.begin(); item_cam != app->render->cameras.end(); ++item_cam)
	{
		for (std::list<Collider*>::iterator item = static_colliders.begin(); item != static_colliders.end(); ++item)
		{
			SDL_RenderSetClipRect(app->render->renderer, &(*item_cam)->screen_section);

			app->render->DrawIsometricQuad((*item)->position.x, (*item)->position.y, (*item)->width, (*item)->height, { 255, 0, 0 , 255 }, (*item_cam));

			SDL_RenderSetClipRect(app->render->renderer, nullptr);
		}

		for (std::list<Collider*>::iterator item = dynamic_colliders.begin(); item != dynamic_colliders.end(); ++item)
		{
			SDL_RenderSetClipRect(app->render->renderer, &(*item_cam)->screen_section);

			app->render->DrawIsometricQuad((*item)->position.x, (*item)->position.y, (*item)->width, (*item)->height, { 255, 0, 255 , 255 }, (*item_cam));

			SDL_RenderSetClipRect(app->render->renderer, nullptr);
		}

	}
	return true;
}

void M_Collision::DestroyColliders()
{
	std::list<Collider*>::iterator iterator = static_colliders.begin();

	while (iterator != static_colliders.end())
	{
		if ((*iterator)->to_destroy == true)
		{
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
			iterator = static_colliders.erase(iterator);

			continue;
		}
		++iterator;
	}

	iterator = dynamic_colliders.begin();

	while (iterator != dynamic_colliders.end())
	{
		if ((*iterator)->to_destroy == true)
		{
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
			iterator = dynamic_colliders.erase(iterator);

			continue;
		}
		++iterator;
	}
}

Collider * M_Collision::AddCollider(fPoint pos, float width, float height, TAG tag, BODY_TYPE body, float damage, Object* object)
{
	Collider* collider = new Collider(pos, width, height, damage, tag, body, object);
	colliders_to_add.push_back(collider);
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
		if (c1->object != nullptr && on_trigger_matrix[(int)c1->tag][(int)c2->tag] && c1->to_destroy == false && c2->to_destroy == false)
		{
			c1->object->OnTrigger(c2);
		}
	}
	else
	{
		c1->collisions_list.push_back(c2);

		if (c1->object != nullptr && on_trigger_matrix[(int)c1->tag][(int)c2->tag] && c1->to_destroy == false && c2->to_destroy == false)
		{
			c1->object->OnTriggerEnter(c2);
		}
	}

	std::swap(c1, c2);

	if (std::find(c1->collisions_list.begin(), c1->collisions_list.end(), c2) != c1->collisions_list.end())
	{
		if (c1->object != nullptr && on_trigger_matrix[(int)c1->tag][(int)c2->tag] && c1->to_destroy == false && c2->to_destroy == false)
		{
			c1->object->OnTrigger(c2);
		}
	}
	else
	{
		c1->collisions_list.push_back(c2);

		if (c1->object != nullptr && on_trigger_matrix[(int)c1->tag][(int)c2->tag] && c1->to_destroy == false && c2->to_destroy == false)
		{
			c1->object->OnTriggerEnter(c2);
		}
	}


}

inline void M_Collision::DoOnTriggerExit(Collider * c1, Collider * c2)
{
	std::list<Collider*>::iterator iter = std::find(c1->collisions_list.begin(), c1->collisions_list.end(), c2);

	if (c1->collisions_list.empty() == false)
	{
		if (iter != c1->collisions_list.end())
		{
			c1->collisions_list.erase(iter);

			if (c1->object != nullptr)
			{
				c1->object->OnTriggerExit(c2);
			}
		}
	}

	std::swap(c1, c2);

	iter = std::find(c1->collisions_list.begin(), c1->collisions_list.end(), c2);

	if (c1->collisions_list.empty() == false)
	{
		if (iter != c1->collisions_list.end())
		{
			c1->collisions_list.erase(iter);

			if (c1->object != nullptr)
			{
				c1->object->OnTriggerExit(c2);
			}
		}
	}
}


