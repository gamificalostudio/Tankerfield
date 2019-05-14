#ifndef __OBJ_ITEM_H__
#define __OBJ_ITEM_H__

#include "Object.h"

class Obj_Tank;

class Obj_Item : public Object
{
public:
	Obj_Item(fPoint pos);

	//Called a single time after the creation of the item
	virtual bool Use();

	virtual void OnTriggerEnter(Collider * collider) {}

	virtual void OnTrigger(Collider * collider, float dt) {}

public:
	Obj_Tank * caster = nullptr;
	ItemType type = ItemType::NO_TYPE;
};

#endif