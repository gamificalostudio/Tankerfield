#ifndef __OBJ_ITEM_H__
#define __OBJ_ITEM_H__

#include "Object.h"

enum class ITEM
{
	HEALTH_BAG,
	MAX_TYPE
};

class Obj_Tank;

class Obj_Item : public Object
{
public:
	virtual bool Use();

public:
	Obj_Tank * caster = nullptr;
};

#endif