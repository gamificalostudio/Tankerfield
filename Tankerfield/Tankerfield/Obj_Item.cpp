#include "Obj_Item.h"
#include "Object.h"

Obj_Item::Obj_Item(fPoint pos) : Object (pos)
{
}

bool Obj_Item::Use()
{
	return true;
}
