#ifndef __OBJ_SUICIDAL_H__
#define __OBJ_SUICIDAL_H__

#include "Circle.h"
#include "Point.h"
#include "Animation.h"
#include "Obj_Enemy.h"
#include "Timer.h"
#include "PerfTimer.h"

struct SDL_Texture;
class Timer;
class Obj_Tank;

class Obj_Suicidal : public Obj_Enemy
{
public:
	Obj_Suicidal(fPoint pos);
	~Obj_Suicidal();

	bool Update(float dt) override;

	void ChangeTexture();

	void Movement(float &dt);

	void Spawn();


private:



	// ----------
	iPoint spawn_draw_offset = { 0,0 };
	iPoint normal_draw_offset = { 0, 0 };

	Animation spawn;

	SDL_Texture * spawn_tex = nullptr;


};

#endif /* __OBJ_SUICIDAL_H__ */