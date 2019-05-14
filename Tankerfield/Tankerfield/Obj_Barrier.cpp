#include "Obj_Barrier.h"
#include "App.h"
#include "M_Collision.h"

Obj_Barrier::Obj_Barrier(fPoint pos):Object(pos)
{
	coll = app->collision->AddCollider(pos_map, 3, 1, Collider::TAG::WALL);
	coll->AddRigidBody(Collider::BODY_TYPE::STATIC);

	time.Start();
}

bool Obj_Barrier::Update(float dt)
{
	if (time.ReadMs() > 10000)
	{
		to_remove = true;
	}

	return true;
}
