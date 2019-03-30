#include "Object.h"
#include "PerfTimer.h"
#include "Point.h"

struct SDL_Texture;

class Obj_BasicShoot : public Object
{
public:
	Obj_BasicShoot();
	Obj_BasicShoot(int x, int y);
	~Obj_BasicShoot();

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
	PerfTimer bullet_life_timer;
	float bullet_life_ms;

};
