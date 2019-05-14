#ifndef __OBJ_BRUTE_H__
#define __OBJ_BRUTE_H__

#include "Circle.h"
#include "Point.h"
#include "Animation.h"
#include "Object.h"
#include "PerfTimer.h"

struct SDL_Texture;
class Timer;
class Obj_Tank;

enum class BRUTE_STATE
{
	SPAWN,
	IDLE,
	GET_PATH,
	MOVE,
	RECHEAD_POINT,
	DEAD
};

class Obj_Brute : public Object
{
public:
	Obj_Brute(fPoint pos);
	~Obj_Brute();

	bool Update(float dt) override;

	bool Draw(float dt, Camera * camera) override;

	void Attack();

	void Movement(float &dt);

	void DrawDebug(const Camera* camera) override;

	void OnTrigger(Collider* collider, float dt);

	void OnTriggerEnter(Collider* collider);

private:
	inline bool IsOnGoal(fPoint goal);

	BRUTE_STATE state = BRUTE_STATE::SPAWN;
	fPoint move_vect = { 0.0f, 0.0f };
	int life = 0;
	float detection_range = 0.0f;
	float check_path_time = 0.f;
	float speed = 0.f;
	Timer timer;

	Obj_Tank* target = nullptr;
	std::vector<fPoint> path;

	fPoint next_pos;

	Circle range_pos;

	// ----------
	iPoint spawn_draw_offset	= { 0,0 };
	iPoint normal_draw_offset	= { 0, 0 };

	/* Attack properties */
	float attack_frequency = 0.f;
	float attack_range = 0.f;//Tile distance in which the enemy can attack
	float attack_range_squared = 0.f;
	int attack_damage = 0;
	PerfTimer perf_timer;

	float coll_w = 0.f;
	float coll_h = 0.f;

	Animation idle;
	Animation walk;
	Animation attack;
	Animation death;
	Animation spawn;
	SDL_Texture * tex = nullptr;
	SDL_Texture * tex_damaged = nullptr;
	SDL_Texture * spawn_tex = nullptr;
	Timer damaged_sprite_timer;
	int damaged_sprite_time = 0;
	bool to_hit = true;

	uint sfx_hit = 0;
	uint sfx_death = 0;
	uint sfx_attack = 0;
	uint sfx_spawn = 0;
};

#endif