#ifndef __OBJ_PICKUP_H__
#define __OBJ_PICKUP_H__

#include "Object.h"
#include <map>

enum class PICKUP_TYPE : int {
	NO_TYPE = -1,
	ITEM,
	WEAPON
};

enum class WEAPON;

class Obj_PickUp : public Object
{
private:
	PICKUP_TYPE type_of_pick_up;
	WEAPON type_of_weapon;
	ObjectType type_of_item;

	std::map<ObjectType, Object*> elements;


public:

	Obj_PickUp(fPoint pos);
	~Obj_PickUp();

	bool Awake(pugi::xml_node&) { return true; };

	bool Update(float dt) override;

	void OnTrigger(Collider* collider);

	bool Draw(float dt, Camera* camera);

	WEAPON RandomWeapon() const;

	ObjectType RandomItem() const;

	PICKUP_TYPE RandomPickUp();

};

#endif // !__PICKUP_H__