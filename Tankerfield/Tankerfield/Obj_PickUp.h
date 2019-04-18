#ifndef __OBJ_PICKUP_H__
#define __OBJ_PICKUP_H__

#include "Object.h"
#include "Obj_Tank.h"

class UI_InGameElement;

enum class PICKUP_TYPE {
	NO_TYPE = -1,
	ITEM,
	WEAPON,
	MAX_TYPES //must be in the last position of the types of pickUps.Needed from the creation of pickUps
};


class Obj_PickUp : public Object
{
private:

	UI_InGameElement* in_game_element = nullptr;

public:

	PICKUP_TYPE type_of_pick_up					= PICKUP_TYPE::NO_TYPE;
	WEAPON      type_of_weapon					= WEAPON::BASIC;
	ObjectType  type_of_item					= ObjectType::NO_TYPE;

	uint level_of_weapon						= NULL;

public:

	Obj_PickUp(fPoint pos);

	~Obj_PickUp();

	void GenerationOfPickUp(PICKUP_TYPE type_of_pick_up = PICKUP_TYPE::NO_TYPE, uint levels_to_add = 0);

	bool Update(float dt) override;

	bool Draw(float dt, Camera* camera);

	WEAPON RandomWeapon() ;

	ObjectType RandomItem() const;

	PICKUP_TYPE RandomPickUp() const;

	void DeletePickUp();
};

#endif // !__PICKUP_H__