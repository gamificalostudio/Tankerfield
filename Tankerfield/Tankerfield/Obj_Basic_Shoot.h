#include "Object.h"
#include "j1PerfTimer.h"
#include "p2Point.h"

struct SDL_Texture;

class Obj_Basic_Shoot : public Object
{
public:
	Obj_Basic_Shoot();
	Obj_Basic_Shoot(int x, int y);
	~Obj_Basic_Shoot();

public:
	bool Awake(pugi::xml_node & tank_node);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

private:

	float speed;
	fPoint direction;
	j1PerfTimer bullet_life_timer;
	float bullet_life_ms;

};
