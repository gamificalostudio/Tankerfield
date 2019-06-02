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

void Item_InstantHelp::Teleport(Obj_Tank* tank_to_teleport, Obj_Portal * portal_from)
{
	if (portal_from == portal1)
	{
		tank_to_teleport->pos_map = portal2->pos_map;
	}
	else if (portal_from == portal2)
	{
		tank_to_teleport->pos_map = portal1->pos_map;
	}
}

bool Item_InstantHelp::Use()
{
	if (caster != nullptr)
	{
		pos_map = caster->pos_map;
	}

	fPoint tank_look = caster->GetShotDir();

	portal1 = (Obj_Portal*)app->objectmanager->CreateObject(ObjectType::PORTAL, pos_map + tank_look * 5);
	portal1->instant_help = this;

	portal2 = (Obj_Portal*)app->objectmanager->CreateObject(ObjectType::PORTAL, pos_map - tank_look * 5);
	portal2->instant_help = this;

	return true;
}




