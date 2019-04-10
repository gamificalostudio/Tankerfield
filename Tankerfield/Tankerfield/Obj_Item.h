#ifndef __OBJ_ITEM_H__
#define __OBJ_ITEM_H__

class Obj_Tank;

class Obj_Item
{
public:
	bool Use();

public:
	Obj_Tank * caster = nullptr;
};

#endif