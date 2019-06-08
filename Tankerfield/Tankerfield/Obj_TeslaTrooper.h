#ifndef __OBJ_TESLATROOPER_H__
#define __OBJ_TESLATROOPER_H__

#include "Circle.h"
#include "Point.h"
#include "Animation.h"
#include "Object.h"
#include "PerfTimer.h"

#include "Obj_Enemy.h"

struct SDL_Texture;
class Timer;
class Obj_Tank;



class Obj_TeslaTrooper : public Obj_Enemy 
{
public:
	Obj_TeslaTrooper(fPoint pos);
	~Obj_TeslaTrooper();

	inline void TeleportOut(float & dt) override;

	inline void TeleportIn(float & dt) override;

	inline void Spawn(const float & dt) override;

	inline void Idle() override;

	inline void Move(const float & dt) override;

	bool Draw(Camera * camera) override;

	void SetStats(int level) override;

private:

	Animation spawn_anim;
	
	SDL_Texture * explosion_apper_tex	= nullptr;


};
#endif // !__TESLATROOPER_H__

