#include "Obj_Tank.h"

Obj_Tank::Obj_Tank()
{
}

Obj_Tank::~Obj_Tank()
{
}

bool Obj_Tank::Awake(pugi::xml_node &)
{
	return true;
}

bool Obj_Tank::Start()
{
	return true;
}

bool Obj_Tank::PreUpdate()
{
	return true;
}

bool Obj_Tank::Update(float dt)
{
	return true;
}

bool Obj_Tank::PostUpdate()
{
	return true;
}

bool Obj_Tank::CleanUp()
{
	return true;
}
