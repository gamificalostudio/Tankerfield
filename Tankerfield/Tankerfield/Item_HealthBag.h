#ifndef __ITEM_HEALTHBAG_H__
#define __ITEM_HEALTHBAG_H__

#include "Obj_Item.h"

class Item_HealthBag : public Obj_Item
{
	bool Use() override;
};

#endif