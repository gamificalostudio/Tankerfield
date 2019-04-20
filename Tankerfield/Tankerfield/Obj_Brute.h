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
	IDLE,
	GET_PATH,
	MOVE,
	RECHEAD_POINT
};

class Obj_Brute : public Object
{
public:
	Obj_Brute(fPoint pos);
	~Obj_Brute();

	bool Update(float dt) override;


	void Attack();

	void Movement(float &dt);

	void DrawDebug(const Camera* camera) override;

	void OnTrigger(Collider* collider);

private:
	inline bool IsOnGoal(fPoint goal);

	BRUTE_STATE state = BRUTE_STATE::IDLE;
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

	int enemy_width = 66;
	int enemy_height = 76;

	/* Attack properties */
	float attack_frequency = 0.f;
	float attack_range = 0.f;//Tile distance in which the enemy can attack
	float attack_range_squared = 0.f;
	int attack_damage = 0;
	PerfTimer perf_timer;

	Animation walk;
	Animation attack;
	SDL_Texture * tex = nullptr;

};

#endif