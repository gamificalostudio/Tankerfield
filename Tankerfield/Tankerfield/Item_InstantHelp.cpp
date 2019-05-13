#include "Item_InstantHelp.h"
#include "Object.h"
#include "Obj_Tank.h"
#include "Obj_Item.h"
#include "App.h"
#include "M_ObjManager.h"

Item_InstantHelp::Item_InstantHelp(fPoint pos):Obj_Item(pos)
{
}

bool Item_InstantHelp::Update(float dt)
{
	if(caster!=nullptr)
	{
		pos_map = caster->pos_map;
	}

	return true;
}

bool Item_InstantHelp::Use()
{
	fPoint casterdir = caster->GetShotDir();
	portal1=(Obj_Portal*)app->objectmanager->CreateObject(ObjectType::PORTAL, pos_map + casterdir *3);

	return true;
}

