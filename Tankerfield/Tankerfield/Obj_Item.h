#ifndef __OBJ_ITEM_H__
#define __OBJ_ITEM_H__

#include "Object.h"

class Obj_Tank;

class Obj_Item : public Object
{
public:
	Obj_Item(fPoint pos);
	virtual bool Use();

public:
	Obj_Tank * caster = nullptr;
};

#endif