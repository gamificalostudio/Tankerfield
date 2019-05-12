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
struct SpawnPoint;

enum class TROOPER_STATE
{
	APPEAR,
	IDLE,
	GET_PATH,
	MOVE,
	GET_TELEPORT_POINT,
	TELEPORT_IN,
	TELEPORT_OUT,
	DEAD,
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

	bool Draw(float dt, Camera * camera) override;

	bool Awake(pugi::xml_node&) { return true; };

	void OnTriggerEnter(Collider * collider);

	void OnTrigger(Collider* collider);



private:
	inline bool IsOnGoal(fPoint goal);

	inline void UpdateVelocity();

	TROOPER_STATE state			= TROOPER_STATE::APPEAR;
	fPoint move_vect			= { 0.0f, 0.0f };

	int life					= 0;

	//move values -----------------
	float detection_range			= 0.0f;
	float squared_detection_range	= 0.f;
	float check_path_time			= 0.f;
	float speed = 0.f;
	Timer path_timer;
	Obj_Tank* target = nullptr;
	std::vector<iPoint> path;
	fPoint next_pos = { 0.f, 0.f };
	Circle range_pos;
	Timer update_velocity_vec;
	//teleport values ----------
	float check_teleport_time	= 0.f;
	uint teleport_enemies_max;


	

	

	// ----------

	/* Attack properties */
	float attack_frequency		= 0.f;
	float attack_range			= 0.f;//Tile distance in which the enemy can attack
	float attack_range_squared	= 0.f;
	int attack_damage			= 0;
	



	PerfTimer perf_timer;
	Timer	teleport_timer;
	Timer	teleport_anim_duration;
	bool to_hit = true;

	Animation idle;
	Animation walk;
	Animation attack;
	Animation death;
	Animation portal_animation;
	Animation portal_close_anim;
	Animation appear_anim;

	bool draw = true;
	Animation* in_portal				= nullptr;
	SDL_Texture * tex					= nullptr;
	SDL_Texture * tex_damaged			= nullptr;
	SDL_Texture * portal_tex			= nullptr;
	SDL_Texture * explosion_apper_tex			= nullptr;

	uint sfx_attack = 0;
	uint sfx_spawn = 0;
	uint sfx_hit = 0;
	uint sfx_death = 0;

	SpawnPoint* teleport_spawnpoint = nullptr;

	Timer damaged_sprite_timer;
	int damaged_sprite_time				= 0;
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

