#include "Obj_Basic_Shoot.h"
#include "j1App.h"
#include "UI_Test.h"
#include "j1Input.h"
#include "j1Render.h"



Obj_Basic_Shoot::Obj_Basic_Shoot() : Obj_Weapon()
{

}

Obj_Basic_Shoot::Obj_Basic_Shoot(int x, int y) : Obj_Weapon(x, y)
{

	//Load XML var ============
	pugi::xml_node basic_bullet_node = App->config.child("object").child("basic_bullet");

	speed = basic_bullet_node.child("speed").attribute("value").as_float();
	bullet_life_ms = basic_bullet_node.child("life").attribute("value").as_float();

	//Start life timer ====
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
	//Calculate new pos of the bullet =====

	pos.x += speed * direction.x * dt;
	pos.y += speed * direction.y * dt;

	//Draw ========================
	App->ui_test->DrawIsometricQuad(pos.x, pos.y, 0.75f, 0.5f);
	
	return true;
}

bool Obj_Basic_Shoot::PostUpdate()
{
	Obj_Weapon::PostUpdate();
	return true;
}

bool Obj_Basic_Shoot::CleanUp()
{
	return true;
}

