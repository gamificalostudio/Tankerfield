#include "Object.h"
#include "j1PerfTimer.h"
#include "p2Point.h"

struct SDL_Texture;

class Obj_Weapon : public Object
{
public:
	Obj_Weapon();
	Obj_Weapon(int x, int y);
	~Obj_Weapon();

public:
	bool Awake(pugi::xml_node & tank_node);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

protected:

	float speed;
	fPoint direction;
	j1PerfTimer bullet_life_timer;
	float bullet_life_ms;

};

