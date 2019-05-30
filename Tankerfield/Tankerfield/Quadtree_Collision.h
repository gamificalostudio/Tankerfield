#ifndef _COLLISIONQUADTREE_H_
#define _COLLISIONQUADTREE_H_

#include <list>
#include "Rect.h"

class Collider;
class Camera;

class QuadTree_Collision
{
public:
	QuadTree_Collision (fRect quad_area, uint max_elements, QuadTree_Collision * prev = nullptr);

	~QuadTree_Collision();

	void Subdivide();

	void AddCollider(Collider* col);

	void DistrbuteColliders();

	void CheckCollisions();

	void CheckCollisionsSubdivisions(Collider * collider);

	void PlaceCollider(Collider* col);

	void CleanUp();

	void Draw(Camera* camera);

private:

	fRect						rect_area;

	QuadTree_Collision*			child_nodes[4]; 

	QuadTree_Collision*			parent_node = nullptr;

	std::list<Collider*>		colliders;				// Colliders that NOT intersect with more than one child node

	std::list<Collider*>		multi_check_colliders;  // Colliders that intersect with more than one child node

	bool is_divided = false;		

	uint max_elements = 10; 
};

#endif
