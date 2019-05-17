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

	void Spawn(const float& dt) override;

private:
	iPoint spawn_draw_offset = { 0,0 };
	iPoint normal_draw_offset = { 0, 0 };
};

#endif /* __OBJ_SUICIDAL_H__ */