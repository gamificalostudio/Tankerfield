#include "Obj_BasicShoot.h"
#include "App.h"
#include "M_UITest.h"
#include "M_Input.h"
#include "M_Render.h"




Obj_BasicShoot::Obj_BasicShoot() : Obj_Weapon()
{

}

Obj_BasicShoot::Obj_BasicShoot(int x, int y) : Obj_Weapon(x, y)
{

	//Load XML var ============
	pugi::xml_node basic_bullet_node = app->config.child("object").child("basic_bullet");

	speed = basic_bullet_node.child("speed").attribute("value").as_float();
	bullet_life_ms = basic_bullet_node.child("life").attribute("value").as_float();
}

Obj_BasicShoot::~Obj_BasicShoot()
{
}

bool Obj_BasicShoot::Awake(pugi::xml_node &)
{
	return true;
}

bool Obj_BasicShoot::Start()
{
	return true;
}

bool Obj_BasicShoot::PreUpdate()
{
	return true;
}

bool Obj_BasicShoot::Update(float dt)
{
	//Calculate new pos of the bullet =====

	pos.x += speed * direction.x * dt;
	pos.y += speed * direction.y * dt;

	//Draw ========================
	app->ui_test->DrawIsometricQuad(pos.x, pos.y, 0.75f, 0.5f);
	
	return true;
}

bool Obj_BasicShoot::PostUpdate()
{
	Obj_Weapon::PostUpdate();
	return true;
}

bool Obj_BasicShoot::CleanUp()
{
	return true;
}

