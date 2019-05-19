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



class Obj_TeslaTrooper : public Obj_Enemy 
{
public:
	Obj_TeslaTrooper(fPoint pos);
	~Obj_TeslaTrooper();

	inline void TeleportOut(float & dt) override;

	inline void TeleportIn(float & dt) override;

	inline void Spawn(const float & dt) override;

	inline void Idle() override;

	inline int Move(float & dt);

	inline virtual void GetPath();

	bool Draw(float dt, Camera * camera) override;

private:

	inline void UpdateVelocity();

	Animation spawn_anim;
	bool draw							= true;

	
	SDL_Texture * explosion_apper_tex	= nullptr;

	


	float squared_detection_range		= 0.f;


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

