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
	app->objectmanager->CreateObject(ObjectType::BARRIER, pos_map+caster->GetShotDir()*3);
	return true;
}
