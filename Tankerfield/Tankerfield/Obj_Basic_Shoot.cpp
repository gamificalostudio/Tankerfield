#include "Obj_Basic_Shoot.h"
#include "j1App.h"
#include "UI_Test.h"



Obj_Basic_Shoot::Obj_Basic_Shoot() : Object()
{

}

Obj_Basic_Shoot::Obj_Basic_Shoot(int x, int y) : Object(x, y)
{
	pos.x = x;
	pos.y = y;
}

Obj_Basic_Shoot::~Obj_Basic_Shoot()
{
}

bool Obj_Basic_Shoot::Awake(pugi::xml_node &)
{
	return true;
}

bool Obj_Basic_Shoot::Start()
{
	return true;
}

bool Obj_Basic_Shoot::PreUpdate()
{
	return true;
}

bool Obj_Basic_Shoot::Update(float dt)
{



	//Draw ========================
	App->ui_test->DrawIsometricQuad(pos.x, pos.y, 0.75f, 0.5f);
	
	return true;
}

bool Obj_Basic_Shoot::PostUpdate()
{
	//uint ind = GetRotatedIndex(base_rects_num, angle, ROTATION_DIR::COUNTER_CLOCKWISE, 135);
	//App->render->Blit(base_tex, pos.x, pos.y, &base_rects[ind]);
	return true;
}

bool Obj_Basic_Shoot::CleanUp()
{
	return true;
}
