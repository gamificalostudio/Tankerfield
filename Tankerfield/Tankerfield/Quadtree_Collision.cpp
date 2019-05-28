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
	CleanUp();
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

	child_nodes[0] = new QuadTree_Collision({ rect_area.GetLeft() , rect_area.GetBottom(), rect_area.w * 0.5F, rect_area.h * 0.5F }, max_elements, this);																														                       
	child_nodes[1] = new QuadTree_Collision({ rect_area.GetLeft() + rect_area.w * 0.5F ,rect_area.GetBottom(),rect_area.w * 0.5F, rect_area.h * 0.5F }, max_elements, this);																															                       
	child_nodes[2] = new QuadTree_Collision({ rect_area.GetLeft(), rect_area.GetBottom() + rect_area.h * 0.5F , rect_area.w * 0.5F, rect_area.h * 0.5F }, max_elements, this);
	child_nodes[3] = new QuadTree_Collision({ rect_area.GetLeft() + rect_area.w * 0.5F ,rect_area.GetBottom() + rect_area.h * 0.5F, rect_area.w * 0.5F,  rect_area.h * 0.5F }, max_elements, this);

	DistrbuteColliders();
}



void QuadTree_Collision::DistrbuteColliders()
{

	for (std::list<Collider*>::iterator iter = colliders.begin(); iter != colliders.end();)
	{
		uint intersections = 0;
		int last_intersection = -1;

		for (uint i = 0; i < 4; ++i)
		{
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
			if (child_nodes[i]->rect_area.CheckIntersection(col->GetRect()))
			{
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

	Collider* c1 = nullptr;
	Collider* c2 = nullptr;


	for (std::list<Collider*>::iterator item = colliders.begin(); item != colliders.end(); ++item)
	{
		c1 = (*item);

		std::list<Collider*>::iterator item2 = ++item;
		--item;
		
		for (; item2 != colliders.end(); ++item2)
		{
			c2 = *item2;
		
			if (c1->CheckCollision(c2) == true)
			{

			}
		}

		if (is_divided)
		{
			for (int i = 0; i < 4; ++i)
			{
				if (c1->subdivision_intersection[i] == true)
				{
					child_nodes[i]->CheckCollisionsSubdivisions(c1);
				}
			}
		}
	}

	if (is_divided)
	{
		for (int i = 0; i < 4; ++i)
		{
			child_nodes[i]->CheckCollisions();
		}
	}
}

void QuadTree_Collision::CheckCollisionsSubdivisions(Collider* collider)
{
	for (std::list<Collider*>::iterator iter = colliders.begin(); iter != colliders.end(); ++iter)
	{
		if ( (*iter)->CheckCollision(collider) == true)
		{

		}
	}

	if (is_divided)
	{
		for (std::list<Collider*>::iterator iter = multi_check_colliders.begin(); iter != multi_check_colliders.end(); ++iter)
		{
			if ((*iter)->CheckCollision(collider) == true)
			{

			}
		}

		for (int i = 0; i < 4; ++i)
		{
			child_nodes[i]->CheckCollisionsSubdivisions(collider);
		}
	}
}


void QuadTree_Collision::CleanUp()
{
	if (is_divided)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (child_nodes[i] != nullptr)
			{
				child_nodes[i]->CleanUp();
				RELEASE(child_nodes[i]);
			}
		}
		is_divided = false;
	}

	colliders.clear();
	multi_check_colliders.clear();
}
