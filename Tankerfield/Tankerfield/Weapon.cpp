#include "Weapon.h"
#include "App.h"
#include "M_UITest.h"
#include "M_Input.h"
#include "M_Render.h"
#include "M_ObjManager.h"
#include "Obj_Bullet.h"
#include "App.h"

Weapon::Weapon()
{
}

Weapon::Weapon(int damage, float speed, float life, float time_betwen_bulltes, ObjectType type) :
	damage(damage),
	speed(speed),
	bullet_life_ms(life),
	bullet_type(type),
	time_between_bullets(time_betwen_bulltes)
{
}

Weapon::~Weapon()
{
}

void Weapon::Shoot(float x, float y)
{
	bullet = (Obj_Bullet*)app->objectmanager->CreateObject(bullet_type, x, y);
	bullet->direction = CalculateDirection();
	bullet->bullet_life_ms = bullet_life_ms;
	bullet->damage = damage;
	bullet->speed = speed;
}

fPoint Weapon::CalculateDirection()
{
	//Direction of the bullet ===========
	iPoint mouse_position = { 0,0 };
	app->input->GetMousePosition(mouse_position.x, mouse_position.y);

	//Add the position of the mouse plus the position of the camera to have the pixel that selects the mouse in the world and then pass it to the map.
	mouse_position.x += app->render->camera.x;
	mouse_position.y += app->render->camera.y;

	//Transform to map to work all variables in map(blit do MapToWorld automatically)
	fPoint map_mouse_position = app->ui_test->WorldToMapF(mouse_position, 100, 50);

	fPoint direction = map_mouse_position - bullet->pos;
	direction.Normalize();

	return direction;
}