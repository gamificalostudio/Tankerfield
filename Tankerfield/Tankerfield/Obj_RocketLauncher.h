#ifndef __OBJ_ROCKETLAUNCHER_H__
#define __OBJ_ROCKETLAUNCHER_H__

#include "Object.h"
#include "Obj_Enemy.h"
#include "Animation.h"
#include "Point.h"
#include "Circle.h"
#include "Timer.h"
#include "PerfTimer.h"

struct SDL_Texture;
class Obj_Tank;

enum class ROCKETLAUNCHER_STATE
{
	SPAWN,
	IDLE,
	GET_PATH,
	MOVE,
	RECHEAD_POINT,
	DEAD
};

class Obj_RocketLauncher : public Obj_Enemy
{
public:
	Obj_RocketLauncher(fPoint pos);
	~Obj_RocketLauncher();
	
	void Spawn(const float& dt) override;
	void Attack() override;
	void Move(const float & dt) override;

	void SetStats(int level) override;

private:
	void ShootMissile();

	uint distance_to_player = 0;
	Timer timer_check_distance;
	float deltatime_to_check_distance = 0.f;
	bool fist_enter_to_move = true;



};

#endif /* __OBJ_ROCKETLAUNCHER_H__ */