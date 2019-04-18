#ifndef __OBJ_REWARDBOX_H__
#define __OBJ_REWARDBOX_H__

#include "Object.h"
#include "Obj_Tank.h"

class SpawnPoint;
class Obj_RewardBox : public Object
{
private:
	uint life							= 0u;
	uint max_life						= 0u;
	bool alive							= true;

public:
	SpawnPoint* my_spawn_point			= nullptr;

public:

	Obj_RewardBox(fPoint pos);

	~Obj_RewardBox();

	bool Awake(pugi::xml_node&) { return true; };

	bool Draw(float dt, Camera* camera);

	void OnTrigger(Collider * collider);

	void GetDamage(float damage);

	void Dead();
};

#endif // !__OBJ_REWARDBOX_H__
