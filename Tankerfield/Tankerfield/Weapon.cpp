#include "Weapon.h"
#include "App.h"
#include "M_UITest.h"
#include "M_Input.h"
#include "M_Render.h"

Weapon::Weapon(int damage, float speed, float life, ObjectType type)
{
}

Weapon::~Weapon()
{
}

void Weapon::Shoot()
{
	//Calculate direction
	//Bullet * obj = CreateObject(obj_type)
	//bullet->damage = ...;
	//bullet->direction = ...;
}

void Weapon::CalculateDirection()
{
	////Direction of the bullet ===========
	//iPoint mouse_position = { 0,0 };
	//app->input->GetMousePosition(mouse_position.x, mouse_position.y);

	////Add the position of the mouse plus the position of the camera to have the pixel that selects the mouse in the world and then pass it to the map.
	//mouse_position.x += app->render->camera.x;
	//mouse_position.y += app->render->camera.y;

	////Transform to map to work all variables in map(blit do MapToWorld automatically)
	//fPoint map_mouse_position = app->ui_test->WorldToMapF(mouse_position, 100, 50);

	//direction = map_mouse_position - pos;
	//direction.Normalize();
}