#include "Item_HealthBag.h"
#include "Object.h"
#include "Obj_Tank.h"
#include "Obj_Item.h"

Item_HealthBag::Item_HealthBag(fPoint pos) : Obj_Item(pos)
{
}

bool Item_HealthBag::Use()
{
	if (caster != nullptr)
	{
		caster->SetLife(caster->GetMaxLife());
		to_remove = true;
	}
	return true;
}