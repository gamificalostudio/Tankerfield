#include "Obj_Weapon.h"
#include "j1PerfTimer.h"
#include "p2Point.h"

class Obj_Basic_Shoot : public Obj_Weapon
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

	

};
