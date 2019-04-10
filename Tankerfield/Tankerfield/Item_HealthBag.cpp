#include "Item_HealthBag.h"
#include "Object.h"
#include "Obj_Tank.h"

bool Item_HealthBag::Use()
{
	caster->SetLife(caster->GetMaxLife());

	to_remove = true;

	return true;
}
