#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "Module_Collision.h"
#include "p2Defs.h"
#include "p2Log.h"

using namespace std;

ModuleCollision::ModuleCollision()
{
	matrix[(int)Collider::TYPE::WALL][(int)Collider::TYPE::WALL] = false;
	matrix[(int)Collider::TYPE::WALL][(int)Collider::TYPE::PLAYER] = false;
	matrix[(int)Collider::TYPE::WALL][(int)Collider::TYPE::ENEMY] = false;
	matrix[(int)Collider::TYPE::WALL][(int)Collider::TYPE::GOD] = false;

	matrix[(int)Collider::TYPE::PLAYER][(int)Collider::TYPE::WALL] = true;
	matrix[(int)Collider::TYPE::PLAYER][(int)Collider::TYPE::PLAYER] = true;
	matrix[(int)Collider::TYPE::PLAYER][(int)Collider::TYPE::ENEMY] = true;
	matrix[(int)Collider::TYPE::PLAYER][(int)Collider::TYPE::GOD] = true;

	matrix[(int)Collider::TYPE::ENEMY][(int)Collider::TYPE::WALL] = false;
	matrix[(int)Collider::TYPE::ENEMY][(int)Collider::TYPE::PLAYER] = false;
	matrix[(int)Collider::TYPE::ENEMY][(int)Collider::TYPE::ENEMY] = false;
	matrix[(int)Collider::TYPE::ENEMY][(int)Collider::TYPE::GOD] = false;

	matrix[(int)Collider::TYPE::GOD][(int)Collider::TYPE::WALL] = false;
	matrix[(int)Collider::TYPE::GOD][(int)Collider::TYPE::PLAYER] = false;
	matrix[(int)Collider::TYPE::GOD][(int)Collider::TYPE::ENEMY] = false;
	matrix[(int)Collider::TYPE::GOD][(int)Collider::TYPE::GOD] = false;
}

// Destructor

ModuleCollision::~ModuleCollision()
{}

bool ModuleCollision::Update(float dt)
{
	// Collision detection and callbacks ============================================== 

	list<Collider*>::iterator iterator_1;
	list<Collider*>::iterator iterator_2;

	Collider* c1 = nullptr;
	Collider* c2 = nullptr;

	for (list<Collider*>::iterator item_1 = colliders.begin(); item_1 != colliders.end(); ++item_1)
	{
		c1 = *item_1;
		iterator_1 = item_1;
		++iterator_1;

		for (list<Collider*>::iterator item_2 = iterator_1; item_2 != colliders.end(); ++item_2)
		{
			c2 = *item_2;
			iterator_2 = item_2;
			++iterator_2;

			if (c1->CheckCollision(c2->rect) == true)
			{
				if (matrix[(int)c1->type][(int)c2->type] && c1->callback)
				{
					c1->callback->OnCollision(c1, c2);
				}
				if (matrix[(int)c2->type][(int)c1->type] && c2->callback)
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
		
	if (debug == false)
		return true;

	for (list<Collider*>::iterator item = colliders.begin(); item != colliders.end(); ++item)
	{
		switch ((*item)->type)
		{
		case Collider::TYPE::NONE: // white
			break;
		case Collider::TYPE::WALL: // blue
			break;
		case Collider::TYPE::PLAYER: // green
			break;
		case Collider::TYPE::GOD: // orange
			break;
		case Collider::TYPE::ENEMY: // violet
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

Collider * ModuleCollision::AddCollider(SDL_Rect rect, Collider::TYPE type, j1Module* callback)
{
	Collider* collider = new Collider(rect, type, callback);
	colliders.push_back(collider);
	return  collider;
}

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return !(r.x >= (rect.x + rect.w) || (r.x + r.w) <= rect.x || r.y >= (rect.y + rect.h) || (r.y + r.h) <= rect.y);
}

bool ModuleCollision::CheckOverlap(list<Collider::OFFSET_DIR> &directions , Collider *dynamic_col, Collider::TYPE type, fPoint &position, fPoint &velocity)
{
	for (std::list<Collider*>::iterator item = colliders.begin(); item != colliders.end() ; ++item)
	{
		if (type != (*item)->type  || (*item) == dynamic_col || matrix[(int)dynamic_col->type][(int)(*item)->type] == false)
		{
			continue;
		}

		if (dynamic_col->CheckCollision((*item)->rect))
		{
			//directions.push_back(SolveOverlap(dynamic_col, *item, position, velocity));
		}
	}

	return true;
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
