#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

#include "app.h"
#include "Obj_Tank.h"
#include "M_Textures.h"
#include "M_Render.h"
#include "M_Collision.h"
#include "M_Input.h"
#include "Log.h"
#include "M_Map.h"
#include "M_ObjManager.h"
#include "M_Window.h"
#include "PerfTimer.h"
#include "Weapon_Flamethrower.h"
#include "MathUtils.h"

SDL_Texture * Obj_Tank::base_tex = nullptr;
SDL_Texture * Obj_Tank::turr_tex = nullptr;
SDL_Texture * Obj_Tank::base_shadow_tex = nullptr;
SDL_Texture * Obj_Tank::turr_shadow_tex = nullptr;
Animation * Obj_Tank::rotate_base = nullptr;
Animation * Obj_Tank::rotate_turr = nullptr;

Obj_Tank::Obj_Tank() : Object()
{

}

Obj_Tank::Obj_Tank(fPoint pos) : Object(pos)
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

	if (base_tex == nullptr)
	{
		Obj_Tank::base_tex = app->tex->Load(tank_node.child("spritesheets").child("base").text().as_string());
	}
	if (base_shadow_tex == nullptr)
	{
		Obj_Tank::base_shadow_tex = app->tex->Load(tank_node.child("spritesheets").child("base_shadow").text().as_string());
	}
	if (turr_tex == nullptr)
	{
		Obj_Tank::turr_tex = app->tex->Load(tank_node.child("spritesheets").child("turr").text().as_string());
	}
	if(turr_shadow_tex == nullptr)
	{
		Obj_Tank::turr_shadow_tex = app->tex->Load(tank_node.child("spritesheets").child("turr_shadow").text().as_string());
	}

	if (rotate_base == nullptr)
	{
		rotate_base = new Animation;
		rotate_base->LoadAnimation(tank_node.child("animations").child("rotate_base"));
		rotate_base->rotation = COUNTER_CLOCKWISE;
		rotate_base->first_dir_angle = 315;
	}
	curr_anim = rotate_base;
	if (rotate_turr == nullptr)
	{
		rotate_turr = new Animation;
		rotate_turr->LoadAnimation(tank_node.child("animations").child("rotate_turr"));
		rotate_turr->rotation = COUNTER_CLOCKWISE;
		rotate_turr->first_dir_angle = 315;
	}

	speed = 5.f;//TODO: Load from xml
	
	cos_45 = cosf(-45 * DEGTORAD);
	sin_45 = sinf(-45 * DEGTORAD);


	weapons[WEAPON_TYPE::FLAMETHROWER] = new Weapon_Flamethrower();
	//weapons[WEAPON_TYPE::BASIC] = new Weapon(tank_node.child("basic").attribute("damage").as_float(), );
	weapons[WEAPON_TYPE::BASIC] = new Weapon(10, 10.f, 2000.f, 50.f, BASIC_BULLET);

	coll = app->collision->AddCollider(pos_map, 0.8f, 0.8f, Collider::TAG::PLAYER, this);
	coll->AddRigidBody(Collider::BODY_TYPE::DYNAMIC);
	coll->SetObjOffset({ .0f,- .0f });

	//TODO: Load them from the XML
	kb_shoot		= SDL_BUTTON_LEFT;
	kb_up			= SDL_SCANCODE_W;
	kb_left			= SDL_SCANCODE_A;
	kb_down			= SDL_SCANCODE_S;
	kb_right		= SDL_SCANCODE_D;
	gamepad_move	= Joystick::LEFT;
	gamepad_aim		= Joystick::RIGHT;
	gamepad_shoot	= SDL_CONTROLLER_AXIS_TRIGGERRIGHT;

	draw_offset.x = 46;
	draw_offset.y = 36;

	base_angle_lerp_factor = 11.25f;
	for (std::vector<Camera*>::iterator item_cam = app->render->camera.begin(); item_cam != app->render->camera.end(); item_cam++)
	{
		if (!(*item_cam)->assigned)
		{
			camera_player = (*item_cam);
			camera_player->assigned = true;
			break;

		}
	}
	return true;
}

bool Obj_Tank::PreUpdate()
{
	if (controller == nullptr)
	{
		controller = app->input->GetAbleController();
	}
	SelectInputMethod();
	return true;
}

bool Obj_Tank::Update(float dt)
{
	Shoot();
	Movement(dt);
	return true;
}

void Obj_Tank::Movement(float dt)
{
	fPoint input_dir(0.f, 0.f);
	if (move_input == INPUT_METHOD::KEYBOARD_MOUSE)
	{
		InputMovementKeyboard(input_dir,dt);
	}
	else if (move_input == INPUT_METHOD::CONTROLLER)
	{
		InputMovementController(input_dir);
	}
	//The tank has to go up in isometric space, so we need to rotate the input vector by 45 degrees
	fPoint iso_dir(0.f, 0.f);
	iso_dir.x = input_dir.x * cos_45 - input_dir.y * sin_45;
	iso_dir.y = input_dir.x * sin_45 + input_dir.y * cos_45;
	iso_dir.Normalize();

	if (!iso_dir.IsZero())
	{
		float target_angle = atan2(input_dir.y, -input_dir.x) * RADTODEG;
		//Calculate how many turns has the base angle and apply them to the target angle
		float turns = floor(angle / 360.f);
		target_angle += 360.f * turns;
		//Check which distance is shorter. Rotating clockwise or counter-clockwise
		if (abs((target_angle + 360.f) - angle) < abs(target_angle - angle))
		{
			target_angle += 360.f;
		}
		angle = lerp(angle, target_angle, base_angle_lerp_factor * dt);
	}

	velocity = iso_dir * speed * dt;                                                               
	pos_map += velocity;
}

void Obj_Tank::InputMovementKeyboard(fPoint & input,float dt)
{
	if (app->input->GetKey(kb_up) == KEY_DOWN || app->input->GetKey(kb_up) == KEY_REPEAT)
	{
		//app->render->camera.y -= floor(100.0f * dt);
		input.y -= 1.f;
	}
	if (app->input->GetKey(kb_left) == KEY_DOWN || app->input->GetKey(kb_left) == KEY_REPEAT)
	{
		//app->render->camera.x -= floor(100.0f * dt);
		input.x -= 1.f;
	}
	if (app->input->GetKey(kb_down) == KEY_DOWN || app->input->GetKey(kb_down) == KEY_REPEAT)
	{
		//app->render->camera.y += floor(100.0f * dt);
		input.y += 1.f;
	}
	if (app->input->GetKey(kb_right) == KEY_DOWN || app->input->GetKey(kb_right) == KEY_REPEAT)
	{
		//app->render->camera.x += floor(100.0f * dt);
		input.x += 1.f;
	}
}

void Obj_Tank::InputMovementController(fPoint & input)
{
	input = (fPoint)(*controller)->GetJoystick(gamepad_move);
}

bool Obj_Tank::PostUpdate(float dt, Camera * camera)
{
	fPoint screen_pos = app->map->MapToScreenF(pos_map);

	app->render->Blit(
		base_tex,
		screen_pos.x - draw_offset.x,
		screen_pos.y - draw_offset.y,
		camera,
		&curr_anim->GetFrame(angle, dt));

	// Turret =======================================

	app->render->Blit(
		turr_tex,
		screen_pos.x - draw_offset.x,
		screen_pos.y - draw_offset.y,
		camera,
		&rotate_turr->GetFrame(turr_angle, dt));

	//Camera centration
	fPoint target_pos;

	target_pos.x = camera_player->rect.x;
	target_pos.y = camera_player->rect.y;

	camera_player->rect.x = lerp(screen_pos.x - camera_player->rect.w * 0.5f, target_pos.x, 0.6f);
	camera_player->rect.y = lerp(screen_pos.y - camera_player->rect.h * 0.5f, target_pos.y, 0.6f);

	//DEBUG
	iPoint debug_mouse_pos = { 0, 0 };
	app->input->GetMousePosition(debug_mouse_pos.x, debug_mouse_pos.y);
	debug_mouse_pos.x += camera_player->rect.x;
	debug_mouse_pos.y += camera_player->rect.y;
	fPoint debug_screen_pos = app->map->MapToScreenF(pos_map);
	app->render->DrawLine(debug_mouse_pos.x, debug_mouse_pos.y, debug_screen_pos.x, debug_screen_pos.y, 99, 38, 127);

	return true;
}

bool Obj_Tank::CleanUp()
{
	return true;
}

void Obj_Tank::OnTrigger(Collider * c1)
{
	if (c1->GetTag() == Collider::TAG::WALL)
	{
		LOG("WALL");
	}
}


void Obj_Tank::InputShotMouse(fPoint & input_dir, fPoint & iso_dir)
{
	iPoint mouse_pos = { 0, 0 };
	app->input->GetMousePosition(mouse_pos.x, mouse_pos.y);

	//Add the position of the mouse plus the position of the camera to have the pixel that selects the mouse in the world and then pass it to the map.
	mouse_pos.x += camera_player->rect.x;
	mouse_pos.y += camera_player->rect.y;

	int tile_width = 100, tile_height = 50;
	fPoint screen_pos = app->map->MapToScreenF(pos_map);
	input_dir = (fPoint)mouse_pos - screen_pos;

	//Transform to map to work all variables in map(blit do MapToWorld automatically)
	fPoint map_mouse_pos = app->map->ScreenToMapF(mouse_pos.x,mouse_pos.y);

	iso_dir = map_mouse_pos - pos_map;
	iso_dir.Normalize();
}

void Obj_Tank::InputShotController(fPoint & input_dir, fPoint & iso_dir)
{
	if (controller != nullptr)
	{
		input_dir = (fPoint)(*controller)->GetJoystick(gamepad_aim);
		iso_dir.x = input_dir.x * cos_45 - input_dir.y * sin_45;
		iso_dir.y = input_dir.x * sin_45 + input_dir.y * cos_45;
		iso_dir.Normalize();
	}
}

void Obj_Tank::Shoot()
{
	//1. Get the direction
	fPoint input_dir(0.f, 0.f);
	fPoint iso_dir;
	if (shot_input == INPUT_METHOD::KEYBOARD_MOUSE)
	{
		InputShotMouse(input_dir, iso_dir);
	}
	else if (shot_input == INPUT_METHOD::CONTROLLER)
	{
		InputShotController(input_dir, iso_dir);
	}

	if (!input_dir.IsZero())
	{
		turr_angle = (atan2(-input_dir.y, input_dir.x) * RADTODEG);
		shot_dir = iso_dir;//Keep the last direction to shoot bullets if the joystick is not being aimed
	}

	if (IsShooting() && time_between_bullets_timer.ReadMs() >= weapons[weapon_type]->time_between_bullets)
	{
		weapons[weapon_type]->Shoot(pos_map, shot_dir, turr_angle);
		time_between_bullets_timer.Start();
	}
}

bool Obj_Tank::IsShooting() {
	return ((app->input->GetMouseButton(kb_shoot) == KEY_DOWN
		|| app->input->GetMouseButton(kb_shoot) == KEY_REPEAT)
		|| (controller != nullptr
			&& (*controller)->GetAxis(gamepad_shoot) > 0));
}

//Select the input method depending on the last input pressed
//Prioritize controller if both inputs are being pressed at the same time
void Obj_Tank::SelectInputMethod()
{
	//Move input
	if (move_input != INPUT_METHOD::KEYBOARD_MOUSE
		&& (app->input->GetKey(kb_up) != KEY_IDLE
		|| app->input->GetKey(kb_left) != KEY_IDLE
		|| app->input->GetKey(kb_down) != KEY_IDLE
		|| app->input->GetKey(kb_right) != KEY_IDLE))
	{
		move_input = INPUT_METHOD::KEYBOARD_MOUSE;
	}
	if (move_input != INPUT_METHOD::CONTROLLER
		&& (controller != nullptr
		&& !(*controller)->GetJoystick(gamepad_move).IsZero()))
	{
		move_input = INPUT_METHOD::CONTROLLER;
	}

	//Shot input
	if (shot_input != INPUT_METHOD::KEYBOARD_MOUSE
		&& app->input->GetMouseButton(kb_shoot) != KEY_IDLE)
	{
		shot_input = INPUT_METHOD::KEYBOARD_MOUSE;
		SDL_ShowCursor(SDL_ENABLE);
	}
	if (shot_input != INPUT_METHOD::CONTROLLER
		&& (controller != nullptr
		&& (!(*controller)->GetJoystick(gamepad_aim).IsZero()
		|| (*controller)->GetAxis(gamepad_shoot) > 0)))
	{
		shot_input = INPUT_METHOD::CONTROLLER;
		SDL_ShowCursor(SDL_DISABLE);
	}
}
