#ifndef __OBJ_BRUTE_H__
#define __OBJ_BRUTE_H__

#include "Circle.h"
#include "Point.h"
#include "Animation.h"
#include "Object.h"
#include "PerfTimer.h"

struct SDL_Texture;
class Timer;

enum class BRUTE_STATE
{
	GET_PATH,
	MOVE,
	RECHEAD_POINT
};

/* Enemy state */
enum class CURRENT_BRUTE_POS_STATE
{
	STATE_UNKNOWN = -1,

	STATE_WAITING,
	STATE_GOING_TO_ATTACK,
	STATE_GOING_TO_WAIT,
	STATE_ATTACKING
};

class Obj_Brute : public Object
{
public:
	Obj_Brute(fPoint pos);
	~Obj_Brute();

	bool Update(float dt) override;

	bool Awake(pugi::xml_node&) { return true; };

	void OnTrigger(Collider* collider);

private:
	inline bool IsOnGoal(fPoint goal);

	BRUTE_STATE state = BRUTE_STATE::GET_PATH;
	fPoint move_vect = { 0.0f, 0.0f };
	int life = 500;
	int damage = 200;
	bool death = false;
	float follow_range = 0.0f;
	float new_current_frame = 0.0f;
	float check_path_time = 0.f;
	float speed = 0.f;
	Timer timer;

	Object* target = nullptr;
	std::vector<fPoint> path;

	fPoint next_pos;

	Circle range_pos;

	// ----------

	int enemy_width = 66;
	int enemy_height = 76;

	CURRENT_BRUTE_POS_STATE current_state = CURRENT_BRUTE_POS_STATE::STATE_WAITING;

	bool BruteCanAttack(const fPoint& enemy_screen_pos, const fPoint& target_screen_pos) const;

	/* Attack properties */
	float attack_frequency = 3000.0f;
	iPoint attack_range = { 60, 30 };
	bool attack_available = false;
	PerfTimer perf_timer;

	static Animation* walk;
	static SDL_Texture * tex;

};

#endif