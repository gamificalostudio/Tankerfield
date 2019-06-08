#include "Brofiler/Brofiler.h"

#include "Log.h"
#include "Defs.h"
#include <math.h>
#include <algorithm>


#include "App.h"
#include "M_Map.h"
#include "M_Input.h"
#include "M_Render.h"
#include "M_Collision.h"

#include "Object.h"
#include "Quadtree_Collision.h"

void Collider::SetPosToObj()
{
	if (object != nullptr)
	{
		rect.pos = object->pos_map + obj_offset;
	}
}

void Collider::SetIsTrigger( bool value)
{
	is_trigger = value;
}

bool Collider::GetIsTrigger() const
{
	return is_trigger;
}

void Collider::Destroy()
{
	if (to_destroy == false)
	{
		to_destroy = true;
		app->collision->colliders_to_destroy.push_back(this);
	}
}

M_Collision::M_Collision()
{
	name.assign("Module_Collision");

	for (int i = 0; i < (int)TAG::MAX; ++i)
	{
		for (int j = 0; j < (int)TAG::MAX; ++j)
		{
			trigger_matrix[i][j] = false;
			physics_matrix[i][j] = false;
		}
	}

	// Set trigger compatinility =======================================================

	trigger_matrix[(int)TAG::PLAYER][(int)TAG::ENEMY] = true;
	trigger_matrix[(int)TAG::PLAYER][(int)TAG::FRIENDLY_BULLET] = true;
	trigger_matrix[(int)TAG::PLAYER][(int)TAG::PICK_UP] = true;
	trigger_matrix[(int)TAG::PLAYER][(int)TAG::ROAD] = true;
	trigger_matrix[(int)TAG::PLAYER][(int)TAG::PORTAL] = true;
	trigger_matrix[(int)TAG::PLAYER][(int)TAG::HEALING_AREA_SHOT] = true;
	trigger_matrix[(int)TAG::PLAYER][(int)TAG::WALL] = true;
	trigger_matrix[(int)TAG::PLAYER][(int)TAG::WATER] = true;

	trigger_matrix[(int)TAG::ENEMY][(int)TAG::BULLET] = true;
	trigger_matrix[(int)TAG::ENEMY][(int)TAG::FRIENDLY_BULLET] = true;
	trigger_matrix[(int)TAG::ENEMY][(int)TAG::BULLET_LASER] = true;
	trigger_matrix[(int)TAG::ENEMY][(int)TAG::ELECTRO_SHOT] = true;
	trigger_matrix[(int)TAG::ENEMY][(int)TAG::BULLET_OIL] = true;
	trigger_matrix[(int)TAG::ENEMY][(int)TAG::OIL_POOL] = true;
	trigger_matrix[(int)TAG::ENEMY][(int)TAG::FLAMETHROWER] = true;
	trigger_matrix[(int)TAG::ENEMY][(int)TAG::REWARD_BOX] = true;

	trigger_matrix[(int)TAG::BULLET][(int)TAG::WALL] = true;
	trigger_matrix[(int)TAG::BULLET][(int)TAG::ENEMY] = true;
	trigger_matrix[(int)TAG::BULLET][(int)TAG::REWARD_BOX] = true;
	trigger_matrix[(int)TAG::BULLET_OIL][(int)TAG::WALL] = true;
	trigger_matrix[(int)TAG::BULLET_OIL][(int)TAG::ENEMY] = true;
	trigger_matrix[(int)TAG::BULLET_OIL][(int)TAG::REWARD_BOX] = true;

	trigger_matrix[(int)TAG::REWARD_ZONE][(int)TAG::PLAYER] = true;

	trigger_matrix[(int)TAG::REWARD_BOX][(int)TAG::BULLET] = true;
	trigger_matrix[(int)TAG::REWARD_BOX][(int)TAG::FRIENDLY_BULLET] = true;
	trigger_matrix[(int)TAG::REWARD_BOX][(int)TAG::BULLET_LASER] = true;
	trigger_matrix[(int)TAG::REWARD_BOX][(int)TAG::ELECTRO_SHOT] = true;
	trigger_matrix[(int)TAG::REWARD_BOX][(int)TAG::BULLET_OIL] = true;
	trigger_matrix[(int)TAG::REWARD_BOX][(int)TAG::FLAMETHROWER] = true;

	trigger_matrix[(int)TAG::FRIENDLY_BULLET][(int)TAG::WALL] = true;
	trigger_matrix[(int)TAG::FRIENDLY_BULLET][(int)TAG::ENEMY] = true; 
	trigger_matrix[(int)TAG::FRIENDLY_BULLET][(int)TAG::PLAYER] = true;
	trigger_matrix[(int)TAG::FRIENDLY_BULLET][(int)TAG::REWARD_BOX] = true;
	trigger_matrix[(int)TAG::PICK_UP][(int)TAG::PLAYER] = true;
	trigger_matrix[(int)TAG::BULLET_LASER][(int)TAG::WALL] = true;
	trigger_matrix[(int)TAG::GOD][(int)TAG::PICK_UP] = true;
	trigger_matrix[(int)TAG::GOD][(int)TAG::ROAD] = true;

	trigger_matrix[(int)TAG::BULLET_ENEMY][(int)TAG::WALL] = true;
	trigger_matrix[(int)TAG::BULLET_ENEMY][(int)TAG::PLAYER] = true;
	trigger_matrix[(int)TAG::PLAYER][(int)TAG::BULLET_ENEMY] = true;
	trigger_matrix[(int)TAG::BULLET_ENEMY][(int)TAG::ENEMY] = false;
	trigger_matrix[(int)TAG::BULLET_ENEMY][(int)TAG::REWARD_BOX] = false;

	// Set rigid body physics compatinility =============================================

	physics_matrix[(int)TAG::PLAYER][(int)TAG::PLAYER]      = true;
	physics_matrix[(int)TAG::PLAYER][(int)TAG::WALL]	      = true;
	physics_matrix[(int)TAG::PLAYER][(int)TAG::WATER]		  = true;
	physics_matrix[(int)TAG::PLAYER][(int)TAG::REWARD_BOX]  = true;
	physics_matrix[(int)TAG::PLAYER][(int)TAG::ENEMY]       = true;

	physics_matrix[(int)TAG::ENEMY][(int)TAG::ENEMY] = true;
	physics_matrix[(int)TAG::ENEMY][(int)TAG::PLAYER] = true;
	physics_matrix[(int)TAG::ENEMY][(int)TAG::WALL] = true;
	physics_matrix[(int)TAG::ENEMY][(int)TAG::WATER] = true;
	physics_matrix[(int)TAG::ENEMY][(int)TAG::REWARD_BOX] = true;

	physics_matrix[(int)TAG::PICK_UP][(int)TAG::PICK_UP] = true;

}

bool M_Collision::CleanUp()
{
	LOG("Freeing all colliders");

	if (quad_tree_collision != nullptr)
	{
		quad_tree_collision->CleanUp();
		RELEASE(quad_tree_collision);
		quad_tree_collision = nullptr;
	}

	// Remove all colliders =====================

	collisions_info.clear();
	colliders_to_add.clear();
	colliders_to_destroy.clear();

	for (std::list<Collider*>::iterator item = colliders_list.begin(); item != colliders_list.end(); ++item)
	{
		if (*item != nullptr)
		{
			RELEASE(*item);
		}
	}

	colliders_list.clear();

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

	if (app->map->MapIsLoaded() == false)
	{
		return true;
	}

	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		debug = !debug;
	}

	// Destroy Colliders =====================================================

	DestroyColliders();

	// Add colliders ========================================================

	if (!colliders_to_add.empty())
	{
		for (std::list<Collider*>::iterator itr = colliders_to_add.begin(); itr != colliders_to_add.end(); ++itr)
		{
			std::list<Collider*>::iterator new_iterator = colliders_list.emplace(colliders_list.begin(), (*itr));
			(*itr)->iterator = new_iterator;
		}

		colliders_to_add.clear();
	}

	// Create Quadtree Collision =============================================

	quad_tree_collision = DBG_NEW QuadTree_Collision(fRect(0, 0, app->map->data.columns, app->map->data.rows), 8, nullptr);

	for (std::list<Collider*>::iterator itr = colliders_list.begin(); itr != colliders_list.end(); ++itr)
	{
		quad_tree_collision->PlaceCollider(*itr);
	}

	quad_tree_collision->CheckCollisions();

	// Triggers ==============================================================

	for (std::list<Collider*>::iterator collider = colliders_list.begin(); collider != colliders_list.end(); ++collider)
	{
		if ((*collider)->collisions_list.empty() == true && (*collider)->triggers_list.empty() == true)
		{
			continue;
		}

		std::list< Collider*>::iterator trigger_found;

		for (std::list<Collider*>::iterator iter = (*collider)->collisions_list.begin(); iter != (*collider)->collisions_list.end(); ++iter)
		{
			trigger_found = std::find( (*collider)->triggers_list.begin() , (*collider)->triggers_list.end() , (*iter));

			if (trigger_found == (*collider)->triggers_list.end())
			{
				if ((*iter)->object != nullptr && trigger_matrix[(int)(*iter)->tag][(int)(*collider)->tag] && (*iter)->to_destroy == false && (*collider)->to_destroy == false)
				{
					(*iter)->object->OnTriggerEnter((*collider), dt);
				}

				(*collider)->triggers_list.push_back(*iter);
			}
			else
			{
				if ((*iter)->object != nullptr && trigger_matrix[(int)(*iter)->tag][(int)(*collider)->tag] && (*iter)->to_destroy == false && (*collider)->to_destroy == false)
				{
					(*iter)->object->OnTrigger((*collider), dt);
				}
			}
		}

		std::list< Collider*>::iterator collider_found;

		for (std::list<Collider*>::iterator trigger = (*collider)->triggers_list.begin(); trigger != (*collider)->triggers_list.end(); )
		{
			collider_found = std::find((*collider)->collisions_list.begin(), (*collider)->collisions_list.end(), (*trigger));

			if (collider_found == (*collider)->collisions_list.end())
			{
				if ((*trigger)->object != nullptr && trigger_matrix[(int)(*trigger)->tag][(int)(*collider)->tag] && (*trigger)->to_destroy == false && (*collider)->to_destroy == false)
				{
					(*trigger)->object->OnTriggerExit((*collider));
				}
				trigger = (*collider)->triggers_list.erase(trigger);
			}
			else
			{
				++trigger;
			}
		}
		(*collider)->collisions_list.clear();
	}

	// Solve Overlaps =========================================================

	std::sort(collisions_info.begin(),collisions_info.end(), [](Collision_Info a, Collision_Info b){ return a.flag > b.flag; });

	for (std::vector<Collision_Info>::iterator info = collisions_info.begin(); info != collisions_info.end(); ++info)
	{
		if ((*info).collider_1->to_destroy == true || (*info).collider_2->to_destroy == true)
		{
			continue;
		}

		if ((*info).flag == 1)
		{
			if ((int)(*info).collider_1->tag > (int)(*info).collider_2->tag)
			{
				SolveOverlapDS((*info).collider_2, (*info).collider_1);
			}
			else if ((int)(*info).collider_1->tag < (int)(*info).collider_2->tag)
			{
				SolveOverlapDS((*info).collider_1, (*info).collider_2);
			}
			else
			{
				SolveOverlapDD((*info).collider_1, (*info).collider_2);
			}
		}
		if ((*info).flag == 2)
		{
			if ((*info).collider_1->body_type == BODY_TYPE::STATIC)
			{
				SolveOverlapDS((*info).collider_2, (*info).collider_1);
			}
			else
			{
				SolveOverlapDS((*info).collider_1, (*info).collider_2);
			}

		}
	}

	collisions_info.clear();

	//// Destroy Colliders =====================================================

	DestroyColliders();

	// =========================================================================

	return true;
}

bool M_Collision::PostUpdate(float dt)
{
	BROFILER_CATEGORY("M_CollisionPostUpdate", Profiler::Color::Orange);

	if (debug == true  && app->map->MapIsLoaded() == true)
	{
		for (std::vector<Camera*>::iterator camera = app->render->cameras.begin(); camera != app->render->cameras.end(); ++camera)
		{
			SDL_RenderSetClipRect(app->render->renderer, &(*camera)->screen_section);

			for (std::list<Collider*>::iterator collider = colliders_list.begin(); collider != colliders_list.end(); ++collider)
			{


				if ((*collider)->body_type == BODY_TYPE::DYNAMIC)
				{
					app->render->DrawIsometricQuad((*collider)->rect.pos.x, (*collider)->rect.pos.y, (*collider)->rect.w, (*collider)->rect.h, { 255, 0, 255 , 255 }, (*camera));
				}
				else
				{
					app->render->DrawIsometricQuad((*collider)->rect.pos.x, (*collider)->rect.pos.y, (*collider)->rect.w, (*collider)->rect.h, { 255, 0, 0 , 255 }, (*camera));
				}
			}

			quad_tree_collision->Draw((*camera));

			SDL_RenderSetClipRect(app->render->renderer, nullptr);
		}

		LOG("%i", collisions_per_frame);
		collisions_per_frame = 0;
	}



	if (quad_tree_collision != nullptr)
	{
		quad_tree_collision->CleanUp();
		RELEASE(quad_tree_collision);
		quad_tree_collision = nullptr;
	}

	return true;
}

Collider * M_Collision::AddCollider(fPoint pos, float width, float height, TAG tag, BODY_TYPE body, float damage, Object* object)
{
	Collider* collider = new Collider(pos, width, height, damage, tag, body, object);
	colliders_to_add.push_back(collider);
	return  collider;
}

void M_Collision::AddCollisionInfo(Collider * collider_1, Collider * collider_2)
{
	// Add collider to its collisions list ============================================

	if (collider_1->body_type == BODY_TYPE::STATIC && collider_2->body_type == BODY_TYPE::STATIC)
	{
		return;
	}

	if (collider_1->is_trigger == true && collider_2->is_trigger == true)
	{

		collider_1->collisions_list.push_back(collider_2);
		collider_2->collisions_list.push_back(collider_1);
	}

	// Add info to Collision Info list =================================================

	if ( (physics_matrix[(int)collider_1->tag] [(int)collider_2->tag] || physics_matrix[(int)collider_2->tag][(int)collider_1->tag]) && collider_1->is_sensor == false && collider_2->is_sensor == false)
	{
		if (collider_1->body_type == BODY_TYPE::DYNAMIC && collider_2->body_type == BODY_TYPE::DYNAMIC)
		{
			collisions_info.push_back(Collision_Info(collider_1, collider_2, 1));
		}
		else if (collider_1->body_type == BODY_TYPE::STATIC && collider_2->body_type == BODY_TYPE::DYNAMIC)
		{
			collisions_info.push_back(Collision_Info(collider_1, collider_2, 2));
		}
		else if (collider_2->body_type == BODY_TYPE::STATIC && collider_1->body_type == BODY_TYPE::DYNAMIC)
		{
			collisions_info.push_back(Collision_Info(collider_1, collider_2, 2));
		}
	}
}

void M_Collision::DestroyColliders()
{
	for (std::list<Collider*>::iterator collider_to_destroy = colliders_to_destroy.begin(); collider_to_destroy != colliders_to_destroy.end(); ++collider_to_destroy)
	{
		for (std::list<Collider*>::iterator itr = (*collider_to_destroy)->triggers_list.begin(); itr != (*collider_to_destroy)->triggers_list.end(); ++itr)
		{
			std::list<Collider*>::iterator to_destroy = std::find((*itr)->triggers_list.begin(), (*itr)->triggers_list.end(), (*collider_to_destroy));

			if (to_destroy != (*itr)->triggers_list.end())
			{
				(*itr)->triggers_list.erase(to_destroy);
			}
		}

		// Destroy ==================================================

		colliders_list.erase((*collider_to_destroy)->iterator);
		RELEASE(*collider_to_destroy);
	}

	colliders_to_destroy.clear();

}

void M_Collision::SolveOverlapDS(Collider * dynamic_col, Collider * static_col)
{
	// Calculate between colliders overlap ============================================
	float distances[(int)Collider::OVERLAP_DIR::MAX];
	distances[(int)Collider::OVERLAP_DIR::LEFT] = dynamic_col->rect.pos.x + dynamic_col->rect.w - static_col->rect.pos.x;
	distances[(int)Collider::OVERLAP_DIR::RIGHT] = static_col->rect.pos.x + static_col->rect.w - dynamic_col->rect.pos.x;
	distances[(int)Collider::OVERLAP_DIR::UP] = dynamic_col->rect.pos.y + dynamic_col->rect.h - static_col->rect.pos.y;
	distances[(int)Collider::OVERLAP_DIR::DOWN] = static_col->rect.pos.y + static_col->rect.h - dynamic_col->rect.pos.y;

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
		dynamic_col->rect.pos.x = static_col->rect.pos.x - dynamic_col->rect.w;
		break;
	case Collider::OVERLAP_DIR::RIGHT:
		dynamic_col->rect.pos.x = static_col->rect.pos.x + static_col->rect.w;
		break;
	case Collider::OVERLAP_DIR::UP:
		dynamic_col->rect.pos.y = static_col->rect.pos.y - dynamic_col->rect.h;
		break;
	case Collider::OVERLAP_DIR::DOWN:
		dynamic_col->rect.pos.y = static_col->rect.pos.y + static_col->rect.h;
		break;
	}

	dynamic_col->object->pos_map = dynamic_col->rect.pos - dynamic_col->obj_offset;
	dynamic_col->last_overlap = (Collider::OVERLAP_DIR)overlap_dir;

}


void M_Collision::SolveOverlapDD(Collider * c1, Collider * c2)
{
	// Calculate between colliders overlap (c1 reference collider) ========================

	float distances[(int)Collider::OVERLAP_DIR::MAX];
	distances[(int)Collider::OVERLAP_DIR::RIGHT] = c1->rect.pos.x + c1->rect.w - c2->rect.pos.x;
	distances[(int)Collider::OVERLAP_DIR::LEFT] = c2->rect.pos.x + c2->rect.w - c1->rect.pos.x;
	distances[(int)Collider::OVERLAP_DIR::UP] = c1->rect.pos.y + c1->rect.h - c2->rect.pos.y;
	distances[(int)Collider::OVERLAP_DIR::DOWN] = c2->rect.pos.y + c2->rect.h - c1->rect.pos.y;

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
		c1->rect.pos.x -= distances[(int)Collider::OVERLAP_DIR::RIGHT] * 0.5f;
		c2->rect.pos.x += distances[(int)Collider::OVERLAP_DIR::RIGHT] * 0.5f;

		break;
	case Collider::OVERLAP_DIR::LEFT:
		c1->rect.pos.x += distances[(int)Collider::OVERLAP_DIR::LEFT] * 0.5f;
		c2->rect.pos.x -= distances[(int)Collider::OVERLAP_DIR::LEFT] * 0.5f;
		break;
	case Collider::OVERLAP_DIR::UP:
		c1->rect.pos.y -= distances[(int)Collider::OVERLAP_DIR::UP] * 0.5f;
		c2->rect.pos.y += distances[(int)Collider::OVERLAP_DIR::UP] * 0.5f;
		break;
	case Collider::OVERLAP_DIR::DOWN:
		c1->rect.pos.y += distances[(int)Collider::OVERLAP_DIR::DOWN] * 0.5f;
		c2->rect.pos.y -= distances[(int)Collider::OVERLAP_DIR::DOWN] * 0.5f;
		break;
	}

	c1->object->pos_map = c1->rect.pos - c1->obj_offset;
	c2->object->pos_map = c2->rect.pos - c2->obj_offset;
}