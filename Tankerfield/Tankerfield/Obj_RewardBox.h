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

	const char* reward_box_dead_sound_string;
	uint reward_box_dead_sound_int	= 0u;

public:
	SpawnPoint* my_spawn_point			= nullptr;

public:

	Obj_RewardBox(fPoint pos);

	~Obj_RewardBox();

	bool Start();

	bool Draw(float dt, Camera* camera) override;

	void OnTrigger(Collider * collider);

	void GetDamage(float damage);

	void Dead();
};

#endif // !__OBJ_REWARDBOX_H__
