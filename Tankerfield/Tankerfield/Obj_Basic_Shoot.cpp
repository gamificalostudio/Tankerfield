#include "Obj_Basic_Shoot.h"
#include "j1App.h"
#include "UI_Test.h"
#include "j1Input.h"



Obj_Basic_Shoot::Obj_Basic_Shoot() : Object()
{

}

Obj_Basic_Shoot::Obj_Basic_Shoot(int x, int y) : Object(x, y)
{
	pos.x = x;
	pos.y = y;

	speed = 0.25f;
	iPoint mouse_position = { 0,0 };
	App->input->GetMousePosition(mouse_position.x, mouse_position.y);

	direction.x = mouse_position.x - pos.x;
	direction.y = mouse_position.y - pos.y;

	float modulo = sqrtf((direction.x*direction.x) + (direction.y*direction.y));

	direction.x /= modulo;
	direction.y /= modulo;

	bullet_life_timer.Start();
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

	pos.x += speed * direction.x;
	pos.y += speed * direction.y;

	//Draw ========================
	App->ui_test->DrawIsometricQuad(pos.x, pos.y, 0.75f, 0.5f);
	
	return true;
}

bool Obj_Basic_Shoot::PostUpdate()
{
	if (bullet_life_timer.ReadMs() >= bullet_life_ms)
	{
		to_remove = true;
	}
	return true;
}

bool Obj_Basic_Shoot::CleanUp()
{
	return true;
}
