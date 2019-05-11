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

	bool Update(float dt) override;

	//void Attack();

	void Movement(float &dt);

	//void DrawDebug(const Camera* camera) override;

	bool Draw(float dt, Camera * camera) override;

	/*bool Awake(pugi::xml_node&) { return true; };

	void OnTriggerEnter(Collider * collider);

	void OnTrigger(Collider* collider);*/

private:
	//inline bool IsOnGoal(fPoint goal);
	
	float check_teleport_time	= 0.f;


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
		coll = app->collision->AddCollider(pos_map, 3, 3, Collider::TAG::REWARD_ZONE, 0.f,this);
		coll->AddRigidBody(Collider::BODY_TYPE::SENSOR);
		return true;
	}

	void OnTriggerEnter(Collider* c1)
	{

	}
	void OnTrigger(Collider* c1)
	{
		if (c1->GetTag() == Collider::TAG::PLAYER)
		{
			/*LOG("REWARD ZONE");*/
		}
	}
	void OnTriggerExit(Collider* c1)
	{

	}
};
#endif // !__TESLATROOPER_H__

