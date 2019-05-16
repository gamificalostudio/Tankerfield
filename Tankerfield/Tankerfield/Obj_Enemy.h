#ifndef __OBJ_ENEMY_H__
#define __OBJ_ENEMY_H__

#include "Object.h"
#include "Circle.h"
#include "Timer.h"

enum class ENEMY_STATE
{
	SPAWN,
	IDLE,
	GET_PATH,
	MOVE,
	RECHEAD_POINT,
	GET_TELEPORT_POINT,
	TELEPORT_IN,
	TELEPORT_OUT,
	DEAD,
};

class Obj_Enemy : public Object
{
public:
	Obj_Enemy(fPoint pos);

	bool Update(float dt) ;

	virtual void ChangeTexture();

	void Attack();

	virtual void Movement(float &dt);

	virtual void Spawn(const float& dt) {};

	void RecheadPoint();

	void Dead();

	inline virtual void Idle();

	inline virtual int Move(float & dt);

	inline virtual void GetPath();

	inline virtual void GetTeleportPoint() {};

	inline virtual void TeleportIn(float & dt) {};

	inline virtual void TeleportOut(float & dt) {};

	void OnTriggerEnter(Collider * collider);

	void OnTrigger(Collider* collider);

	inline bool IsOnGoal(fPoint goal); //const?

	void DrawDebug(const Camera* camera)override;

	bool Draw(float dt, Camera* camera)override;

	bool CleanUp() override;

	void Oiled();

protected:
	inline void UpdateVelocity();

	int life = 0;
	float speed = 0.f;
	float original_speed = 0.f;
	SDL_Texture * tex = nullptr;
	SDL_Texture * tex_damaged = nullptr;
	SDL_Texture * oiled_tex = nullptr;

	ENEMY_STATE state = ENEMY_STATE::IDLE;

	Timer update_velocity_vec;
	Timer damaged_sprite_timer;
	Timer oiled_timer;
	int damaged_sprite_time = 0;

	bool oiled = false;

	Animation idle;
	Animation walk;
	Animation attack;
	Animation death;

	float scale = 0.f;

	float coll_w = 0.f;
	float coll_h = 0.f;

	uint sfx_attack = 0;
	uint sfx_spawn = 0;
	uint sfx_hit = 0;
	uint sfx_death = 0;

	Circle range_pos;

	Obj_Tank* target = nullptr;
	std::vector<iPoint> path;

	PerfTimer perf_timer;
	Timer path_timer;

	/* Attack properties */
	float attack_frequency = 0.f;
	float attack_range = 0.f;//Tile distance in which the enemy can attack
	float attack_range_squared = 0.f;
	int attack_damage = 0;

	fPoint move_vect = { 0.0f, 0.0f };
	float check_path_time = 0.f;
	fPoint next_pos = { 0.f, 0.f };
	float detection_range = 0.0f;
};

#endif
