#ifndef __OBJ_PICKUP_H__
#define __OBJ_PICKUP_H__

#include "Object.h"
#include "Obj_Tank.h"

enum class PICKUP_TYPE {
	NO_TYPE = -1,
	ITEM,
	WEAPON
};


class Obj_PickUp : public Object
{
public:
	PICKUP_TYPE type_of_pick_up					= PICKUP_TYPE::NO_TYPE;
	WEAPON type_of_weapon						= WEAPON::BASIC;
	ObjectType type_of_item						= ObjectType::NO_TYPE;



public:

	Obj_PickUp(fPoint pos);

	~Obj_PickUp();

	bool Awake(pugi::xml_node&) { return true; };

	bool Update(float dt) override;

	void OnTrigger(Collider * collider);

	bool Draw(float dt, Camera* camera);

	WEAPON RandomWeapon() const;

	ObjectType RandomItem() const;

	PICKUP_TYPE RandomPickUp();

	void DeletePickUp();

	void SetPickUp(Obj_Tank* player);

};

#endif // !__PICKUP_H__