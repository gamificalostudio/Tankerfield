#include "Obj_Weapon.h"
#include "App.h"
#include "M_UITest.h"
#include "M_Input.h"
#include "M_Render.h"



Obj_Weapon::Obj_Weapon() : Object()
{

}

Obj_Weapon::Obj_Weapon(int x, int y) : Object(x, y)
{

	CalculateDirection();

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

void Obj_Weapon::CalculateDirection()
{
	//Direction of the bullet ===========
	iPoint mouse_position = { 0,0 };
	app->input->GetMousePosition(mouse_position.x, mouse_position.y);

	//Add the position of the mouse plus the position of the camera to have the pixel that selects the mouse in the world and then pass it to the map.
	mouse_position.x += app->render->camera.x;
	mouse_position.y += app->render->camera.y;

	//Transform to map to work all variables in map(blit do MapToWorld automatically)
	fPoint map_mouse_position = app->ui_test->WorldToMapF(mouse_position, 100, 50);

	direction = map_mouse_position - pos;

	NormilizeVector(direction);


}

void Obj_Weapon::NormilizeVector(fPoint& vector)
{
	//Normilize vector
	float norm = sqrtf((vector.x*vector.x) + (vector.y*vector.y));
	vector /= norm;
}