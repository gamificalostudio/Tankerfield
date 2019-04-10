#ifndef __ITEM_HEALTHBAG_H__
#define __ITEM_HEALTHBAG_H__

#include "Obj_Item.h"

class Item_HealthBag : public Obj_Item
{
public:
	Item_HealthBag(fPoint pos);

	bool Use() override;
};

#endif