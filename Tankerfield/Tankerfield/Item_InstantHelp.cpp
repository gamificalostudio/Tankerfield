#include "Item_InstantHelp.h"
#include "Object.h"
#include "Obj_Tank.h"
#include "Obj_Item.h"
#include "App.h"
#include "M_ObjManager.h"
#include "M_Collision.h"

Item_InstantHelp::Item_InstantHelp(fPoint pos):Obj_Item(pos)
{
}

bool Item_InstantHelp::Update(float dt)
{
	if(caster!=nullptr)
	{
		pos_map = caster->pos_map;
	}

	if (tank_to_tp != nullptr)
	{
		if (current_portal == portal1)
		{
			tank_to_tp->pos_map = portal2->pos_map;
		}

		else if (current_portal == portal2)
		{
			tank_to_tp->pos_map = portal1->pos_map;
		}
		
	}

	return true;
}

bool Item_InstantHelp::Use()
{
	fPoint tank_look = caster->GetShotDir();

	portal1 = (Obj_Portal*)app->objectmanager->CreateObject(ObjectType::PORTAL, pos_map + tank_look * 5);

	portal2 = (Obj_Portal*)app->objectmanager->CreateObject(ObjectType::PORTAL, pos_map - tank_look * 5);

	return true;
}




