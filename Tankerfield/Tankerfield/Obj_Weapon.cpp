#include "Obj_Weapon.h"
#include "j1App.h"
#include "UI_Test.h"
#include "j1Input.h"
#include "j1Render.h"



Obj_Weapon::Obj_Weapon() : Object()
{

}

Obj_Weapon::Obj_Weapon(int x, int y) : Object(x, y)
{

	//Direction of the bullet ===========
	iPoint mouse_position = { 0,0 };
	App->input->GetMousePosition(mouse_position.x, mouse_position.y);

	//Add the position of the mouse plus the position of the camera to have the pixel that selects the mouse in the world and then pass it to the map.
	mouse_position.x += -App->render->camera.x;
	mouse_position.y += -App->render->camera.y;

	//Transform to map to work all variables in map(blit do MapToWorld automatically)
	fPoint map_mouse_position = App->ui_test->WorldToMapF(mouse_position, 100, 50);

	direction = map_mouse_position - pos;

	//Normilize vector
	float norm = sqrtf((direction.x*direction.x) + (direction.y*direction.y));
	direction /= norm;

	//Start life timer ====
	bullet_life_timer.Start();
}

Obj_Weapon::~Obj_Weapon()
{
}

bool Obj_Weapon::Awake(pugi::xml_node &)
{
	return true;
}

bool Obj_Weapon::Start()
{
	return true;
}

bool Obj_Weapon::PreUpdate()
{
	return true;
}

bool Obj_Weapon::Update(float dt)
{

	return true;
}

bool Obj_Weapon::PostUpdate()
{
	if (bullet_life_timer.ReadMs() >= bullet_life_ms)
	{
		to_remove = true;
	}
	return true;
}

bool Obj_Weapon::CleanUp()
{
	return true;
}

