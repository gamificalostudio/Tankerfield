#include "Item_InstantHelp.h"
#include "Object.h"
#include "Obj_Tank.h"
#include "Obj_Item.h"
#include "App.h"
#include "M_ObjManager.h"
#include "M_Collision.h"
#include "Obj_Tank.h"

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
	portal1 = (Obj_Portal*)app->objectmanager->CreateObject(ObjectType::PORTAL, pos_map + caster->GetShotDir()  * 5);

	portal2 = (Obj_Portal*)app->objectmanager->CreateObject(ObjectType::PORTAL, pos_map - caster->GetShotDir() * 5);

	return true;
}


void Item_InstantHelp::OnTriggerEnter(Collider* collider)
{
	if (collider->GetTag() == Collider::TAG::PLAYER)
	{
		Obj_Tank* tank = (Obj_Tank*)collider->GetObj();
		
		tank->pos_map = portal2->pos_map;
	}
}


