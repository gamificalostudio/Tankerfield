#include "Object.h"
#include "PerfTimer.h"
#include "Point.h"

struct SDL_Texture;

class Weapon
{
public:
	Weapon();
	~Weapon();

public:
	void Shoot();

protected:
	void CalculateDirection();

protected:
	float speed = 0.f;
	fPoint direction;
	PerfTimer bullet_life_timer;
	float bullet_life_ms = 0.f;

};

