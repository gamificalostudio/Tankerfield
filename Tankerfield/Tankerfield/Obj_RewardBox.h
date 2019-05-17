#ifndef __OBJ_REWARDBOX_H__
#define __OBJ_REWARDBOX_H__

#include "Object.h"
#include "Obj_Tank.h"
#include "Obj_PickUp.h"

class SDL_Texture;
class SpawnPoint;

class Obj_RewardBox : public Object
{
private:
	PICKUP_TYPE type = PICKUP_TYPE::NO_TYPE;

	//SOUND ===================================
	const char* reward_box_dead_sound_string;
	uint reward_box_dead_sound_int	= 0u;

	//IMAGE ==================================
	SDL_Texture* texture;
	iPoint draw_shadow_offset;
	SDL_Rect shadow_frame	= { 0, 0, 0, 0 };
	SDL_Rect frame_white	= { 0, 0, 0, 0 };
	SDL_Rect* curr_frame	= nullptr;
	
	//LOGIC ==================================
	uint hits_taken = 0;
	uint max_hits = 3;

	bool is_white;
	Timer timer_white;
	float max_time_in_white = 0.2;

public:
	SpawnPoint* my_spawn_point			= nullptr;

public:

	Obj_RewardBox(fPoint pos);

	~Obj_RewardBox();

	bool Update(float dt) override;

	void OnTrigger(Collider * collider);

	bool Draw(float dt, Camera * camera) override;

	bool DrawShadow(Camera* camera, float dt) override;

	void Dead();

	void SetTypeBox(PICKUP_TYPE type = PICKUP_TYPE::NO_TYPE);
};

#endif // !__OBJ_REWARDBOX_H__
