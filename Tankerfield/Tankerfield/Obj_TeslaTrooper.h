#ifndef __OBJ_TESLATROOPER_H__
#define __OBJ_TESLATROOPER_H__

#include "Circle.h"
#include "Point.h"
#include "Animation.h"
#include "Object.h"
#include "PerfTimer.h"

struct SDL_Texture;
class Timer;
class Obj_Tank;

enum class TROOPER_STATE
{
	GET_PATH,
	MOVE,
	RECHEAD_POINT
};

/* Enemy state */
enum class CURRENT_POS_STATE
{
	STATE_UNKNOWN = -1,

	STATE_WAITING,
	STATE_GOING_TO_WAIT,
};

class Obj_TeslaTrooper : public Object 
{
public:
	Obj_TeslaTrooper(fPoint pos);
	~Obj_TeslaTrooper();

	bool Update(float dt) override;

	void Attack();

	void Movement(float &dt);

	void DrawDebug(const Camera* camera) override;

	bool Awake(pugi::xml_node&) { return true; };

	void OnTrigger(Collider* collider);

private:
	inline bool IsOnGoal(fPoint goal);

	TROOPER_STATE state			= TROOPER_STATE::GET_PATH;
	fPoint move_vect			= { 0.0f, 0.0f };
	int life					= 100;
	int damage					= 100;
	bool death					= false;
	float follow_range			= 0.0f;
	float new_current_frame		= 0.0f;	
	float check_path_time		= 0.f;
	float speed					= 0.f;
	Timer timer;

	Obj_Tank* target = nullptr;
	std::vector<fPoint> path;

	fPoint next_pos;
	
	Circle range_pos;

	// ----------

	int enemy_width = 66;
	int enemy_height = 76;

	CURRENT_POS_STATE current_state = CURRENT_POS_STATE::STATE_WAITING;

	/* Attack properties */
	float attack_frequency = 3000.0f;
	float attack_range = 0.f;//Tile distance in which the enemy can attack
	int attack_damage = 0;
	PerfTimer perf_timer;

	Animation walk;
	SDL_Texture * tex = nullptr;

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

