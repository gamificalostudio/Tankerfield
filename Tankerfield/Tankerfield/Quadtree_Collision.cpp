#include "Quadtree_Collision.h"
#include "M_Collision.h"
#include "M_Render.h"
#include "M_Scene.h"
#include "M_Window.h"
#include "Brofiler\Brofiler.h"
#include "App.h"

QuadTree_Collision::QuadTree_Collision(fRect quad_area, uint max_elements, QuadTree_Collision * prev) :
	rect_area(quad_area), max_elements(max_elements) , parent_node(prev)
{ 
}

QuadTree_Collision::~QuadTree_Collision()
{
}

void QuadTree_Collision::AddCollider(Collider* collider)
{
	colliders.push_back(collider);

	if (colliders.size() >= max_elements)
	{
		Subdivide();
	}
} 

void QuadTree_Collision::Subdivide()
{
	//Create 4 childs that are divisions of the father.

	is_divided = true;

	child_nodes[0] = new QuadTree_Collision({ rect_area.GetLeft() , rect_area.GetTop(), rect_area.w * 0.5F, rect_area.h * 0.5F }, max_elements, this);																														                       
	child_nodes[1] = new QuadTree_Collision({ rect_area.GetLeft() + rect_area.w * 0.5F ,rect_area.GetTop(),rect_area.w * 0.5F, rect_area.h * 0.5F }, max_elements, this);
	child_nodes[2] = new QuadTree_Collision({ rect_area.GetLeft(), rect_area.GetTop() + rect_area.h * 0.5F , rect_area.w * 0.5F, rect_area.h * 0.5F }, max_elements, this);
	child_nodes[3] = new QuadTree_Collision({ rect_area.GetLeft() + rect_area.w * 0.5F ,rect_area.GetTop() + rect_area.h * 0.5F, rect_area.w * 0.5F,  rect_area.h * 0.5F }, max_elements, this);

	DistrbuteColliders();
}

void QuadTree_Collision::DistrbuteColliders()
{

	for (std::list<Collider*>::iterator iter = colliders.begin(); iter != colliders.end(); ++iter)
	{
		uint intersections = 0;
		int last_intersection = -1;

		for (uint i = 0; i < 4; ++i)
		{
			(*iter)->subdivision_intersection[i] = false;

			if (child_nodes[i]->rect_area.CheckIntersection((*iter)->GetRect()))
			{
				++intersections;
				(*iter)->subdivision_intersection[i] = true;
				last_intersection = i;
			}
		}

		if (intersections == 1)
		{
			child_nodes[last_intersection]->AddCollider(*iter);
		}
		else
		{
			multi_check_colliders.push_back(*iter);
		}
	}

	colliders.clear();
}

void QuadTree_Collision::PlaceCollider(Collider * col)
{
	BROFILER_CATEGORY("PutEntitiesinQuadtree", Profiler::Color::Red);

	if (is_divided)
	{
		uint intersections = 0;
		int last_intersection = -1;

		for (int i = 0; i < 4; ++i)
		{
			col->subdivision_intersection[i] = false;

			if (child_nodes[i]->rect_area.CheckIntersection(col->GetRect()))
			{
				col->subdivision_intersection[i] = true;
				++intersections;
				last_intersection = i;
			}
		}

		if (intersections == 1)
		{
			child_nodes[last_intersection]->PlaceCollider(col);
		}
		else
		{
			multi_check_colliders.push_back(col);
		}
	}
	else
	{
		AddCollider(col);
	}
		
}



void QuadTree_Collision::CheckCollisions()
{
	
	BROFILER_CATEGORY("QuadTreeCollisions", Profiler::Color::Red);

	Collider* collider_1 = nullptr;
	Collider* collider_2 = nullptr;

	if (is_divided)
	{
		for (std::list<Collider*>::iterator itr_1 = multi_check_colliders.begin(); itr_1 != multi_check_colliders.end(); ++itr_1)
		{
			collider_1 = (*itr_1);

			std::list<Collider*>::iterator itr_2 = itr_1;
			++itr_2;

			for (; itr_2 != multi_check_colliders.end(); ++itr_2)
			{
				collider_2 = *itr_2;

				++app->collision->collisions_per_frame;

				if (collider_1->CheckCollision(collider_2) == true)
				{
					app->collision->AddCollisionInfo(collider_1, collider_2);
				}
			}

			for (int i = 0; i < 4; ++i)
			{
				if (collider_1->subdivision_intersection[i] == true)
				{
					child_nodes[i]->CheckCollisionsSubdivisions(collider_1);
				}
			}
		}

		for (int i = 0; i < 4; ++i)
		{
			child_nodes[i]->CheckCollisions();
		}
	}
	else
	{
		for (std::list<Collider*>::iterator item = colliders.begin(); item != colliders.end(); ++item)
		{
			collider_1 = (*item);

			std::list<Collider*>::iterator item2 = ++item;
			--item;

			for (; item2 != colliders.end(); ++item2)
			{
				collider_2 = *item2;

				++app->collision->collisions_per_frame;

				if (collider_1->CheckCollision(collider_2) == true)
				{
					app->collision->AddCollisionInfo(collider_1, collider_2);
				}
			}
		}
	}
}

void QuadTree_Collision::CheckCollisionsSubdivisions(Collider* collider)
{
	if (is_divided)
	{
		for (std::list<Collider*>::iterator iter = multi_check_colliders.begin(); iter != multi_check_colliders.end(); ++iter)
		{
			++app->collision->collisions_per_frame;

			if ((*iter)->CheckCollision(collider) == true)
			{
				app->collision->AddCollisionInfo((*iter), collider);
			}
		}

		for (int i = 0; i < 4; ++i)
		{
			child_nodes[i]->CheckCollisionsSubdivisions(collider);
		}
	}
	else
	{
		for (std::list<Collider*>::iterator iter = colliders.begin(); iter != colliders.end(); ++iter)
		{
			++app->collision->collisions_per_frame;

			if ((*iter)->CheckCollision(collider) == true)
			{
				app->collision->AddCollisionInfo((*iter), collider);
			}
		}
	}
}


void QuadTree_Collision::CleanUp()
{
	if (is_divided)
	{
		for (int i = 0; i < 4; ++i)
		{
			child_nodes[i]->CleanUp();
			RELEASE(child_nodes[i]);
		}
	}
}


void QuadTree_Collision::Draw(Camera* camera)
{
	app->render->DrawIsometricQuad(rect_area.pos.x, rect_area.pos.y, rect_area.w, rect_area.h, { 0, 255, 0, 255 }, camera);

	if (is_divided)
	{
		for (int i = 0; i < 4; ++i)
		{
			child_nodes[i]->Draw(camera);
		}
	}
}
