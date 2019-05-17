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
	void ChangeTexture();

	void Spawn(const float& dt) override;

	inline void Burn(const float & dt);

	bool Draw(float dt, Camera* cam) override;


private:



	// ----------
	iPoint spawn_draw_offset	= { 0,0 };
	
	

	Animation spawn;
	
	SDL_Texture * spawn_tex = nullptr;


};

#endif