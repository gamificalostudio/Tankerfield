#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

#include "app.h"
#include "Obj_Tank.h"
#include "M_Textures.h"
#include "M_Render.h"

#include "M_Input.h"
#include "Log.h"
#include "M_UITest.h"
#include "M_ObjManager.h"
#include "PerfTimer.h"
#include "Weapon_Flamethrower.h"


SDL_Texture * Obj_Tank::base_tex = nullptr;
SDL_Texture * Obj_Tank::turr_tex = nullptr;
SDL_Texture * Obj_Tank::base_shadow_tex = nullptr;
SDL_Texture * Obj_Tank::turr_shadow_tex = nullptr;
int Obj_Tank::base_rects_num = 128;
SDL_Rect * Obj_Tank::base_rects = new SDL_Rect[base_rects_num];

Obj_Tank::Obj_Tank() : Object()
{

}

Obj_Tank::Obj_Tank(int x, int y) : Object(x, y)
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
	pugi::xml_node tank_node = app->config.child("object").child("tank");

	Obj_Tank::base_tex = app->tex->Load(tank_node.child("spritesheets").child("base").text().as_string());
	Obj_Tank::base_shadow_tex = app->tex->Load(tank_node.child("spritesheets").child("base_shadow").text().as_string());
	Obj_Tank::turr_tex = app->tex->Load(tank_node.child("spritesheets").child("turr").text().as_string());
	Obj_Tank::turr_shadow_tex = app->tex->Load(tank_node.child("spritesheets").child("turr_shadow").text().as_string());

	LoadRects(tank_node.child("animations").child("rotate_base"), base_rects);

	cos_45 = cosf(-45 * DEGTORAD);
	sin_45 = sinf(-45 * DEGTORAD);


	weapons[WEAPON_TYPE::FLAMETHROWER] = new Weapon_Flamethrower();
	//weapons[WEAPON_TYPE::BASIC] = new Weapon(tank_node.child("basic").attribute("damage").as_float(), );
	weapons[WEAPON_TYPE::BASIC] = new Weapon(10, 50, 300, 100, BASIC_BULLET);

	return true;
}

bool Obj_Tank::PreUpdate()
{
	if (controller == nullptr)
	{
		controller = app->input->GetAbleController();
	}
	return true;
}

bool Obj_Tank::Update(float dt)
{
	Movement(dt);
	Shoot();
	return true;
}

void Obj_Tank::Movement(float dt)
{
	fPoint input(0.f, 0.f);
	InputMovementKeyboard(input);
	InputMovementController(input);

	fPoint dir(0.f, 0.f);
	//The tank has to go up in isometric space, so we need to rotate the input vector by 45 degrees
	dir.x = input.x * cos_45 - input.y * sin_45;
	dir.y = input.x * sin_45 + input.y * cos_45;
	dir.Normalize();

	if (!dir.IsZero())
	{
		angle = (atan2(input.y, -input.x) * RADTODEG);
	}

	pos += dir * speed * dt;
}

void Obj_Tank::InputMovementKeyboard(fPoint & input)
{
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		input.y -= 1.f;
	}
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		input.x -= 1.f;
	}
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		input.y += 1.f;
	}
	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		input.x += 1.f;
	}
}

void Obj_Tank::InputMovementController(fPoint & input)
{
	if (controller != nullptr)
	{
		input += (fPoint)(*controller)->GetJoystick(Joystick::LEFT);
	}
}

bool Obj_Tank::PostUpdate()
{
	int tile_width = 100, tile_height = 50;
	uint ind = GetRotatedIndex(base_rects_num, angle, ROTATION_DIR::COUNTER_CLOCKWISE, 315);
	fPoint iso_pos = MapToWorldF(pos.x, pos.y, tile_width, tile_height);
	app->render->Blit(base_tex, iso_pos.x, iso_pos.y, &base_rects[ind]);
	return true;
}

bool Obj_Tank::CleanUp()
{
	return true;
}

void Obj_Tank::InputShotMouse(fPoint & input)
{
	iPoint mouse_pos = { 0,0 };
	app->input->GetMousePosition(mouse_pos.x, mouse_pos.y);

	//Add the position of the mouse plus the position of the camera to have the pixel that selects the mouse in the world and then pass it to the map.
	mouse_pos.x += app->render->camera.x;
	mouse_pos.y += app->render->camera.y;

	//Transform to map to work all variables in map(blit do MapToWorld automatically)
	fPoint map_mouse_pos = app->ui_test->WorldToMapF(mouse_pos, 100, 50);

	input = map_mouse_pos - pos;
	input.Normalize();
}

void Obj_Tank::InputShotController(fPoint & dir)
{
	if (controller != nullptr)
	{
		fPoint input = (fPoint)(*controller)->GetJoystick(Joystick::RIGHT);
		//LOG("input x: %f, y: %f", input.x, input.y);
		dir = input;
		//dir.x = input.x * cos_45 - input.y * sin_45;
		//dir.y = input.x * sin_45 + input.y * cos_45;
		//dir.Normalize();
	}
}

void Obj_Tank::Shoot()
{
	//1. Get the direction
	fPoint input(0.f, 0.f);
	//If the direction of the controller is null, we get the keyboard direction
	//InputShotMouse(input);
	InputShotController(input);

	//TODO: Rotate turret sprite

	if (IsShooting() && time_between_bullets_timer.ReadMs() >= weapons[weapon_type]->time_between_bullets)
	{
		weapons[weapon_type]->Shoot(pos.x, pos.y, input);
		time_between_bullets_timer.Start();
	}
}

bool Obj_Tank::IsShooting() {
	return ((app->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN
		|| app->input->GetKey(SDL_SCANCODE_B) == KEY_REPEAT)
		|| (controller != nullptr
			&& (*controller)->GetAxis(SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 0));
}