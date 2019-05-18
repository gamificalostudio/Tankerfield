#include "Obj_Healing_Area_Shot.h"
#include "WeaponInfo.h"
#include "Object.h"
#include "M_Collision.h"

Obj_Healing_Area_Shot::Obj_Healing_Area_Shot(fPoint pos) : Object(pos)
{
	time.Start();

	coll = app->collision->AddCollider(pos_map, 3, 3, TAG::HEALING_AREA_SHOT, BODY_TYPE::DYNAMIC);
	coll->is_sensor = true;
}

Obj_Healing_Area_Shot::~Obj_Healing_Area_Shot()
{
}

bool Obj_Healing_Area_Shot::Update(float dt)
{
	if (time.ReadMs() >= 3000)
	{
		to_remove = true;
	}

	return true;
}
