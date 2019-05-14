#ifndef __OBJ_BRUTE_H__
#define __OBJ_BRUTE_H__

#include "Circle.h"
#include "Point.h"
#include "Animation.h"
#include "Object.h"
#include "PerfTimer.h"

#include "Obj_Enemy.h"

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

class Obj_Brute : public Obj_Enemy
{
public:
	Obj_Brute(fPoint pos);
	~Obj_Brute();

	bool Update(float dt) override;

	void ChangeTexture();

	void Movement(float &dt);

	void Spawn();


private:



	// ----------
	iPoint spawn_draw_offset	= { 0,0 };
	iPoint normal_draw_offset	= { 0, 0 };

	Animation spawn;
	
	SDL_Texture * spawn_tex = nullptr;


};

#endif