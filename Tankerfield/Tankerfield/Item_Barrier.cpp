#include "Item_Barrier.h"
#include "M_Collision.h"
#include "App.h"
#include "Obj_Tank.h"

Item_Barrier::Item_Barrier(fPoint pos):Obj_Item(pos)
{
}

bool Item_Barrier::Update(float dt)
{
	if (caster != nullptr)
	{
		pos_map = caster->pos_map;
	}

	return true;
}

bool Item_Barrier::Use()
{
	fPoint caster_dir = caster->GetShotDir();
	app->objectmanager->CreateObject(ObjectType::BARRIER, pos_map+caster_dir*2-caster_dir.RotateDegree(90)*1.5);
	return true;
}
