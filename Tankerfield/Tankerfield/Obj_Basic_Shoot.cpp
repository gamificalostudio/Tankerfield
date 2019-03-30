#include "Obj_Basic_Shoot.h"
#include "App.h"
#include "M_UITest.h"
#include "M_Input.h"
#include "M_Render.h"



Obj_Basic_Shoot::Obj_Basic_Shoot() : Object()
{

}

Obj_Basic_Shoot::Obj_Basic_Shoot(int x, int y) : Object(x, y)
{

	//Load XML var ============
	pugi::xml_node basic_bullet_node = app->config.child("object").child("basic_bullet");

	speed = basic_bullet_node.child("speed").attribute("value").as_float();
	bullet_life_ms = basic_bullet_node.child("life").attribute("value").as_float();
	

	//Direction of the bullet ===========
	iPoint mouse_position = { 0,0 };
	app->input->GetMousePosition(mouse_position.x, mouse_position.y);

	//Add the position of the mouse plus the position of the camera to have the pixel that selects the mouse in the world and then pass it to the map.
	mouse_position.x += -app->render->camera.x;
	mouse_position.y += -app->render->camera.y;

	//Transform to map to work all variables in map(blit do MapToWorld automatically)
	fPoint map_mouse_position = app->ui_test->WorldToMapF(mouse_position, 100, 50);

	direction = map_mouse_position - pos;

	//Normilize vector
	float norm = sqrtf((direction.x*direction.x) + (direction.y*direction.y));
	direction /= norm;

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
	app->ui_test->DrawIsometricQuad(pos.x, pos.y, 0.75f, 0.5f);
	
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

