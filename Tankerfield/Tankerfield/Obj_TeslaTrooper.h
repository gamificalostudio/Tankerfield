#ifndef __OBJ_TESLATROOPER_H__
#define __OBJ_TESLATROOPER_H__

#include "Circle.h"
#include "Point.h"
#include "Animation.h"
#include "Object.h"

struct SDL_Texture;
class Timer;

class Obj_TeslaTrooper : public Object 
{
public:
	Obj_TeslaTrooper(fPoint pos);

public:
	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() { return true; };
	bool Awake(pugi::xml_node&) { return true; };


private:
	inline bool IsOnGoal(fPoint goal);
	int life = 100;
	int damage = 100;
	float speed = 1.0f;

	bool death = false;

	static Animation* walk;
	float angle = 0.0f;
	float new_current_frame = 0.0f;
	static SDL_Texture* tex;
	Timer timer;

	Object* target = nullptr;
	std::vector<iPoint> path;

	fPoint next_pos;
	
	Circle range_pos;
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
		coll = app->collision->AddCollider(pos_map, 3, 3, Collider::TAG::REWARD_ZONE, this);
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
			LOG("REWARD ZONE");
		}
	}
	void OnTriggerExit(Collider* c1)
	{

	}
};
#endif // !__TESLATROOPER_H__

