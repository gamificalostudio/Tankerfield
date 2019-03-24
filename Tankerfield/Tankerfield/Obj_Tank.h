#include "Object.h"

class Obj_Tank : public Object
{
public:
	Obj_Tank();
	~Obj_Tank();

public:
	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

private:

public:

private:

};