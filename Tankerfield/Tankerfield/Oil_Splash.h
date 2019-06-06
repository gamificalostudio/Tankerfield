#ifndef __OIL_SPLASH_H__
#define __OIL_SPLASH_H__

#include "Circle.h"
#include "Point.h"
#include "Animation.h"
#include "Object.h"
#include "PerfTimer.h"
#include "WeaponInfo.h"

struct SDL_Texture;
class Timer;

class Oil_Splash : public Object
{
public:
	Oil_Splash(fPoint pos);

	bool Update(float dt) override;

	void SetSplashDamage(float damage);

private:
	int frame_num = 0;
	int frame_damage = 0;
	int splash_damage = 0;
	Animation anim;
	SDL_Texture * tex = nullptr;
};

#endif
