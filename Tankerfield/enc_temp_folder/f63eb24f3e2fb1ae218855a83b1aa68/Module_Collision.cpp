#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "Module_Collision.h"
#include "p2Defs.h"
#include "p2Log.h"


// TODO REMOVE IR

#include "UI_Test.h"

// =============

using namespace std;

bool Collider::CheckCollision(Collider*  coll) const
{
	return !(coll->position.x >= (position.x + width) || (coll->position.x + coll->width) <= position.x || coll->position.y >= (position.y + height) || (coll->position.y + coll->height) <= position.y);
}


ModuleCollision::ModuleCollision()
{
	name.assign("Module_Collision");

	matrix[(int)Collider::TAG::WALL][(int)Collider::TAG::WALL] = false;
	matrix[(int)Collider::TAG::WALL][(int)Collider::TAG::PLAYER] = false;
	matrix[(int)Collider::TAG::WALL][(int)Collider::TAG::ENEMY] = false;
	matrix[(int)Collider::TAG::WALL][(int)Collider::TAG::GOD] = false;

	matrix[(int)Collider::TAG::PLAYER][(int)Collider::TAG::WALL] = true;
	matrix[(int)Collider::TAG::PLAYER][(int)Collider::TAG::PLAYER] = true;
	matrix[(int)Collider::TAG::PLAYER][(int)Collider::TAG::ENEMY] = true;
	matrix[(int)Collider::TAG::PLAYER][(int)Collider::TAG::GOD] = true;

	matrix[(int)Collider::TAG::ENEMY][(int)Collider::TAG::WALL] = false;
	matrix[(int)Collider::TAG::ENEMY][(int)Collider::TAG::PLAYER] = false;
	matrix[(int)Collider::TAG::ENEMY][(int)Collider::TAG::ENEMY] = false;
	matrix[(int)Collider::TAG::ENEMY][(int)Collider::TAG::GOD] = false;

	matrix[(int)Collider::TAG::GOD][(int)Collider::TAG::WALL] = false;
	matrix[(int)Collider::TAG::GOD][(int)Collider::TAG::PLAYER] = false;
	matrix[(int)Collider::TAG::GOD][(int)Collider::TAG::ENEMY] = false;
	matrix[(int)Collider::TAG::GOD][(int)Collider::TAG::GOD] = false;
}

// Destructor

ModuleCollision::~ModuleCollision()
{}

bool ModuleCollision::Update(float dt)
{
	// Collision detection and callbacks ============================================== 

	std::list<Collider*>::iterator iterator_1;
	std::list<Collider*>::iterator iterator_2;

	Collider* c1 = nullptr;
	Collider* c2 = nullptr;

	for (std::list<Collider*>::iterator item_1 = colliders.begin(); item_1 != colliders.end(); ++item_1)
	{
		c1 = *item_1;
		iterator_1 = item_1;
		++iterator_1;

		for (std::list<Collider*>::iterator item_2 = iterator_1; item_2 != colliders.end(); ++item_2)
		{
			c2 = *item_2;
			iterator_2 = item_2;
			++iterator_2;

			if (c1->CheckCollision(c2) == true)
			{
				if (matrix[(int)c1->tag][(int)c2->tag] && c1->callback)
				{
					c1->callback->OnCollision(c1, c2);

					if (c1->type == Collider::TYPE::DYNAMIC)
					{
						SolveOverlap(c1, c2);
					}
				}
				if (matrix[(int)c2->tag][(int)c1->tag] && c2->callback)
				{
					c2->callback->OnCollision(c2, c1);
				}
				
			}
		}
	}

	return true;
}

// Called before render is available
bool ModuleCollision::PostUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
		debug = !debug;
	}
		
	if (debug == true)
		return true;

	for (list<Collider*>::iterator item = colliders.begin(); item != colliders.end(); ++item)
	{
		App->ui_test->DrawIsometricQuad((*item)->position.x, (*item)->position.y, (*item)->width , (*item)->height);

		switch ((*item)->tag)
		{
		case Collider::TAG::NONE: // white
			break;
		case Collider::TAG::WALL: // blue
			break;
		case Collider::TAG::PLAYER: // green
			break;
		case Collider::TAG::GOD: // orange
			break;
		case Collider::TAG::ENEMY: // violet
			break;
		}
	}

	return true;
}

// Called before quitting
bool ModuleCollision:: CleanUp()
{
	LOG("Freeing all colliders");

	// Remove all colliders =====================

	for (std::list<Collider*>::iterator item = colliders.begin(); item != colliders.end() ; ++item)
	{
		if (*item != nullptr)
		{
			RELEASE(*item);
		}
	}

	colliders.clear();

	return true;
}

Collider * ModuleCollision::AddCollider(fPoint pos , float width , float height, Collider::TAG type, j1Module* callback, Object* object)
{
	Collider* collider = new Collider(pos, width, height, type, object, callback);
	colliders.push_back(collider);
	return  collider;
}



bool ModuleCollision::DeleteCollider(Collider * collider)
{
	list<Collider*>::iterator collider_to_delete;

	collider_to_delete = std::find(colliders.begin(), colliders.end(), collider);

	if (collider_to_delete == colliders.end())
	{
		LOG("Collider to delete not found");
		return false;
	}

	RELEASE(*collider_to_delete);
	colliders.erase(collider_to_delete);

}

void ModuleCollision::SolveOverlap(Collider * c1, Collider * c2)
{
	// Check velocity directions ================================================

	bool directions[(int)Collider::OVERLAP_DIR::MAX];
	directions[(int)Collider::OVERLAP_DIR::LEFT] =		c1->object->velocity.x < 0;
	directions[(int)Collider::OVERLAP_DIR::RIGHT] =		c1->object->velocity.x > 0;
	directions[(int)Collider::OVERLAP_DIR::UP] =        c1->object->velocity.y > 0;
	directions[(int)Collider::OVERLAP_DIR::DOWN] =      c1->object->velocity.y < 0;

	// Calculate between colliders overlap =======================================

	float distances[(int)Collider::OVERLAP_DIR::MAX];
	distances[(int)Collider::OVERLAP_DIR::RIGHT] =	abs(c1->position.x + c1->width - c2->position.x);
	distances[(int)Collider::OVERLAP_DIR::LEFT] =   abs(c2->position.x + c2->width - c1->position.x);
	distances[(int)Collider::OVERLAP_DIR::UP] =     abs(c1->position.y + c1->height - c2->position.y);    
	distances[(int)Collider::OVERLAP_DIR::DOWN] =   abs(c2->position.y + c2->height - c1->position.y);

	int overlap_dir = -1;

	for (uint i = 0; i < (int)Collider::OVERLAP_DIR::MAX; ++i)
	{
		if (directions[i])
		{
			if (overlap_dir == -1)
				overlap_dir = i;
			else if (distances[i] < distances[(int)overlap_dir])
				overlap_dir = i;
		}
	}

	switch ((Collider::OVERLAP_DIR)overlap_dir)
	{
	case Collider::OVERLAP_DIR::RIGHT:
		c1->object->position.x = c2->position.x - c1->width;
		LOG("RIGHT");
		break;
	case Collider::OVERLAP_DIR::LEFT:
		c1->object->position.x = c2->position.x + c2->width;
		LOG("LEFT");
		break;
	case Collider::OVERLAP_DIR::UP:
		c1->object->position.y = c2->position.y - c1->height;
		LOG("UP");
		break;
	case Collider::OVERLAP_DIR::DOWN:
		c1->object->position.y = c2->position.y + c2->height;
		LOG("DOWN");
		break;
	}

	c1->SetPos(c1->object->position.x, c1->object->position.y);
}

//Collider::OFFSET_DIR ModuleCollision::SolveOverlap(Collider *dynamic_col, Collider *static_col, fPoint &position , fPoint &velocity)
//{
//	SDL_Rect dynamic = dynamic_col->rect;
//	SDL_Rect rigid = static_col->rect;
//
//	bool directions[(uint)Direction::unknown];
//	directions[(uint)Direction::left] = velocity.x < 0;
//	directions[(uint)Direction::right] = velocity.x > 0;
//	directions[(uint)Direction::up] = velocity.y < 0;
//	directions[(uint)Direction::down] = velocity.y > 0;
//
//	uint distances[(uint)Direction::unknown];
//	distances[(uint)Direction::right] = dynamic.x + dynamic.w - rigid.x;
//	distances[(uint)Direction::left] = rigid.x + rigid.w - dynamic.x;
//	distances[(uint)Direction::up] = rigid.y + rigid.h - dynamic.y;
//	distances[(uint)Direction::down] = dynamic.y + dynamic.h - rigid.y;
//
//	int offset_direction = -1;
//
//	for (uint i = 0; i < (uint)Direction::unknown; ++i)
//	{
//		if (directions[i]) {
//
//			if (offset_direction == -1)
//				offset_direction = i;
//			else if (distances[i] < distances[(uint)offset_direction])
//				offset_direction = i;
//		}
//	}
//
//	if (offset_direction == -1)
//	{
//		return Direction::unknown;
//	}
//
//	switch ((Direction)offset_direction)
//	{
//	case Direction::right:
//		position.x = (float)rigid.x - (float)dynamic.w / 2.0f;
//		break;
//	case Direction::left:
//		position.x = (float)rigid.x + (float)rigid.w + (float)dynamic.w / 2.0f;
//		break;
//	case Direction::up:
//		position.y = (float)rigid.y + (float)rigid.h + (float)dynamic.h / 2.0f;
//		break;
//	case Direction::down:
//		position.y = (float)rigid.y - (float)dynamic.h / 2.0f;
//		break;
//	}
//	return (Direction)offset_direction;
//}
