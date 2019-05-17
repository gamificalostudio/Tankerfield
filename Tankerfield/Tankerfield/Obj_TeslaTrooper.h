#ifndef __OBJ_TESLATROOPER_H__
#define __OBJ_TESLATROOPER_H__

#include "Circle.h"
#include "Point.h"
#include "Animation.h"
#include "Object.h"
#include "PerfTimer.h"

#include "Obj_Enemy.h"

struct SDL_Texture;
class Timer;
class Obj_Tank;
struct SpawnPoint;


class Obj_TeslaTrooper : public Obj_Enemy 
{
public:
	Obj_TeslaTrooper(fPoint pos);
	~Obj_TeslaTrooper();

	inline void TeleportOut(float & dt) override;

	inline void TeleportIn(float & dt) override;

	inline void GetTeleportPoint() override;

	inline void Spawn(const float & dt) override;

	inline void Idle() override;

	inline int Move(float & dt);


	inline virtual void GetPath();

	bool Draw(float dt, Camera * camera) override;




private:

	inline void UpdateVelocity();




	Timer	teleport_timer;
	Timer	teleport_anim_duration;

	
	Animation portal_animation;
	Animation portal_close_anim;
	Animation spawn_anim;
	

	bool draw = true;
	Animation* in_portal				= nullptr;
	

	SDL_Texture * portal_tex			= nullptr;
	SDL_Texture * explosion_apper_tex			= nullptr;

	SpawnPoint* teleport_spawnpoint = nullptr;

	//teleport values ----------
	float check_teleport_time = 0.f;
	uint teleport_enemies_max;

	float squared_detection_range = 0.f;


};

// TODO REMOVE IT 
#include "M_Collision.h"

class Reward_Zone : public Object
{
public:

	Reward_Zone(fPoint pos): Object(pos) { }

	~Reward_Zone()
	{
	}

	bool Start()
	{
		coll = app->collision->AddCollider(pos_map, 3, 3, TAG::REWARD_ZONE, BODY_TYPE::DYNAMIC , 0.f,this);
		coll->is_sensor = true;

		return true;
	}

	void OnTriggerEnter(Collider* c1)
	{

	}
	void OnTrigger(Collider* c1)
	{
		if (c1->GetTag() == TAG::PLAYER)
		{
			/*LOG("REWARD ZONE");*/
		}
	}
	void OnTriggerExit(Collider* c1)
	{

	}
};
#endif // !__TESLATROOPER_H__

