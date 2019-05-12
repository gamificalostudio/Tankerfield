#include "Obj_OilPool.h"
#include "M_Collision.h"
#include "App.h"

Obj_OilPool::Obj_OilPool(fPoint pos) : Object(pos)
{
	time.Start();

	coll=app->collision->AddCollider(pos_map, 5, 5, Collider::TAG::OIL_POOL);
	coll->AddRigidBody(Collider::BODY_TYPE::SENSOR);
}

Obj_OilPool::~Obj_OilPool()
{
}

bool Obj_OilPool::Update(float dt)
{
	if (time.ReadMs() >= 5000)
	{
		to_remove = true;
	}

	return true;
}
