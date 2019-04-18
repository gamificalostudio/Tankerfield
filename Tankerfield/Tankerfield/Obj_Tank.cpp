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
#include "M_Scene.h"
#include "M_ObjManager.h"
#include "M_Window.h"
#include "M_Audio.h"
#include "PerfTimer.h"
#include "MathUtils.h"
#include "Obj_Bullet.h"
#include "Bullet_Missile.h"
#include "Healing_Bullet.h"
#include "Obj_HealingAnimation.h"
#include "Obj_PickUp.h"
#include "M_AnimationBank.h"

int Obj_Tank::number_of_tanks = 0;

Obj_Tank::Obj_Tank(fPoint pos) : Object(pos)
{
	tank_num = number_of_tanks++;
}

Obj_Tank::~Obj_Tank()
{
	//number_of_tanks--;
}

bool Obj_Tank::Start()
{
	pugi::xml_node tank_node = app->config.child("object").child("tank");

	base_tex_yellow = app->tex->Load(tank_node.child("spritesheets").child("base_yellow").text().as_string());
	//base_tex_orange = app->tex->Load(tank_node.child("spritesheets").child("base_orange").text().as_string());
	base_tex_red = app->tex->Load(tank_node.child("spritesheets").child("base_red").text().as_string());
	//base_tex_light_green = app->tex->Load(tank_node.child("spritesheets").child("base_light_green").text().as_string());
	base_tex_pink = app->tex->Load(tank_node.child("spritesheets").child("base_pink").text().as_string());
	base_tex_light_blue = app->tex->Load(tank_node.child("spritesheets").child("base_light_blue").text().as_string());
	//base_tex_dark_blue = app->tex->Load(tank_node.child("spritesheets").child("base_dark_blue").text().as_string());
	//base_tex_purple = app->tex->Load(tank_node.child("spritesheets").child("babase_purplese").text().as_string());
	base_shadow_tex = app->tex->Load(tank_node.child("spritesheets").child("base_shadow").text().as_string());
	SDL_SetTextureBlendMode(base_shadow_tex, SDL_BLENDMODE_MOD);

	turr_tex = app->tex->Load(tank_node.child("spritesheets").child("turr").text().as_string());
	turr_shadow_tex = app->tex->Load(tank_node.child("spritesheets").child("turr_shadow").text().as_string());
	SDL_SetTextureBlendMode(turr_shadow_tex, SDL_BLENDMODE_MOD);

	shot_sound = app->audio->LoadFx(tank_node.child("sounds").child("basic_shot").attribute("sound").as_string());

	switch (tank_num) {
	case 0:
		kb_up		= SDL_SCANCODE_W;
		kb_left		= SDL_SCANCODE_A;
		kb_down		= SDL_SCANCODE_S;
		kb_right	= SDL_SCANCODE_D;
		kb_item		= SDL_SCANCODE_Q;
		kb_interact	= SDL_SCANCODE_E;
		curr_tex = base_tex_red;
		break;
	case 1:
		kb_up		= SDL_SCANCODE_T;
		kb_left		= SDL_SCANCODE_F;
		kb_down		= SDL_SCANCODE_G;
		kb_right	= SDL_SCANCODE_H;
		kb_item		= SDL_SCANCODE_R;
		kb_interact = SDL_SCANCODE_Y;
		curr_tex = base_tex_light_blue;
		break;
	case 2:
		kb_up		= SDL_SCANCODE_I;
		kb_left		= SDL_SCANCODE_J;
		kb_down		= SDL_SCANCODE_K;
		kb_right	= SDL_SCANCODE_L;
		kb_item		= SDL_SCANCODE_U;
		kb_interact = SDL_SCANCODE_O;
		curr_tex = base_tex_pink;
		break;
	case 3:
		kb_up		= SDL_SCANCODE_KP_8;
		kb_left		= SDL_SCANCODE_KP_4;
		kb_down		= SDL_SCANCODE_KP_5;
		kb_right	= SDL_SCANCODE_KP_6;
		kb_item		= SDL_SCANCODE_KP_7;
		kb_interact	= SDL_SCANCODE_KP_9;
		curr_tex = base_tex_yellow;
		break;
	default:
		curr_tex = base_tex_yellow;
		LOG("Number of tanks is greater than 3. You probably restarted the game and need to set the variable to 0 again.");
		break;
	}
	kb_shoot = SDL_BUTTON_LEFT;

	rotate_base.frames = app->anim_bank->LoadFrames(tank_node.child("animations").child("rotate_base"));
	curr_anim = &rotate_base;

	rotate_turr.frames = app->anim_bank->LoadFrames(tank_node.child("animations").child("rotate_turr"));

	speed = 4.f;//TODO: Load from xml

	cos_45 = cosf(-45 * DEGTORAD);
	sin_45 = sinf(-45 * DEGTORAD);

	weapon_info.LoadProperties(app->config.child("weapons").child("basic"));

	basic_shot_function[(uint)WEAPON::BASIC]			= &Obj_Tank::ShootBasic;
	basic_shot_function[(uint)WEAPON::DOUBLE_MISSILE]	= &Obj_Tank::ShootDoubleMissile;
	basic_shot_function[(uint)WEAPON::HEALING_SHOT]		= &Obj_Tank::ShootHealingShot;

	charge_time = 3000.f; // Same for all bullets (player gets used to it)
	charged_shot_function[(uint)WEAPON::BASIC]			= &Obj_Tank::ShootBasic;
	charged_shot_function[(uint)WEAPON::DOUBLE_MISSILE] = &Obj_Tank::ShootDoubleMissile;
	charged_shot_function[(uint)WEAPON::HEALING_SHOT]	= &Obj_Tank::ShootHealingShot;

	coll = app->collision->AddCollider(pos_map, 0.8f, 0.8f, Collider::TAG::PLAYER,0.f,this);
	coll->AddRigidBody(Collider::BODY_TYPE::DYNAMIC);
	coll->SetObjOffset({ -0.4f, -0.4f });

	cannon_height = 11.f;
	cannon_length = 0.75f;

	gamepad_move		= Joystick::LEFT;
	gamepad_aim			= Joystick::RIGHT;
	gamepad_shoot		= SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
	gamepad_item		= SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	gamepad_interact	= SDL_CONTROLLER_BUTTON_A;

	draw_offset.x = 46;
	draw_offset.y = 46;

	base_angle_lerp_factor = 11.25f;

	shot_timer.Start();

	life = 90;
	max_life = 100;

	//Life inicialistation
	//item = ObjectType::HEALTH_BAG;

	std::vector<Camera*>::iterator item_cam;
	for (item_cam = app->render->camera.begin(); item_cam != app->render->camera.end(); ++item_cam)
	{
		if (!(*item_cam)->assigned)
		{
			(*item_cam)->assigned = true;
			camera_player = (*item_cam);
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
	Shoot(dt);
	Item();
	Movement(dt);
	StopTank();
	ReviveTank();
	CameraMovement(dt);

	return true;
}

void Obj_Tank::CameraMovement(float dt)
{
	fPoint screen_pos = app->map->MapToScreenF(pos_map);
	fPoint target_pos =
	{
		(float)camera_player->rect.x,
		(float)camera_player->rect.y
	};

	camera_player->rect.x = lerp(screen_pos.x - camera_player->rect.w * 0.5f, target_pos.x, 0.6f/*37.5f * dt*/);
	camera_player->rect.y = lerp(screen_pos.y - camera_player->rect.h * 0.5f, target_pos.y, 0.6f/*37.5f * dt*/);
}

void Obj_Tank::Movement(float dt)
{
	fPoint input_dir(0.f, 0.f);
	if (move_input == INPUT_METHOD::KEYBOARD_MOUSE)
	{
		InputMovementKeyboard(input_dir);
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

		velocity = iso_dir * speed * dt;
		pos_map += velocity;
	}
}

void Obj_Tank::InputMovementKeyboard(fPoint & input)
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

bool Obj_Tank::Draw(float dt, Camera * camera)
{
	// Base =========================================
	app->render->Blit(
		curr_tex,
		pos_screen.x - draw_offset.x,
		pos_screen.y - draw_offset.y,
		camera,
		&curr_anim->GetFrame(angle));

	// Turret =======================================
	app->render->Blit(
		turr_tex,
		pos_screen.x - draw_offset.x,
		pos_screen.y - draw_offset.y,
		camera,
		&rotate_turr.GetFrame(turr_angle));

	if (camera == camera_player)
	{
		//DEBUG
		float line_length = 5.f;
		//1-- Set a position in the isometric space
		fPoint input_iso_pos(turr_pos.x + shot_dir.x * line_length, turr_pos.y + shot_dir.y * line_length);
		//2-- Transform that poin to screen coordinates
		iPoint input_screen_pos = (iPoint)app->map->MapToScreenF(input_iso_pos);
		app->render->DrawLineSplitScreen(
			pos_screen.x, pos_screen.y - cannon_height,
			input_screen_pos.x, input_screen_pos.y, 255, 255, 255, 123, camera);
	}

	return true;
}

bool Obj_Tank::DrawShadow(Camera * camera)
{
	fPoint screen_pos = app->map->MapToScreenF(pos_map);

	// Base =========================================
	app->render->Blit(
		base_shadow_tex,
		pos_screen.x - draw_offset.x,
		pos_screen.y - draw_offset.y,
		camera,
		&curr_anim->GetFrame(angle));

	// Turret =======================================
	app->render->Blit(
		turr_shadow_tex,
		pos_screen.x - draw_offset.x,
		pos_screen.y - draw_offset.y,
		camera,
		&rotate_turr.GetFrame(turr_angle));

	return true;
}

bool Obj_Tank::CleanUp()
{
	return true;
}


void Obj_Tank::OnTrigger(Collider * c1)
{
	if (c1->GetTag() == Collider::TAG::FRIENDLY_BULLET)
	{
		Obj_Healing_Animation* new_particle = (Obj_Healing_Animation*)app->objectmanager->CreateObject(ObjectType::HEALING_ANIMATION, pos_map);
		new_particle->tank = this;
		if (GetLife()<GetMaxLife())
		{
			SetLife(GetLife() + weapon_info.bullet_healing);
		}
	}

	if (c1->GetTag() == Collider::TAG::PICK_UP)
	{
		Obj_PickUp* pick_up = (Obj_PickUp*)c1->GetObj();
		if (app->input->GetKey(kb_interact) == KEY_DOWN || app->input->GetKey(gamepad_interact) == KEY_DOWN)
		{
			SetPickUp(pick_up);
		}
	}
}

void Obj_Tank::SetLife(int life)
{
	//TODO: Update UI bars
	if (this->life > GetMaxLife())
	{
		this->life = GetMaxLife();
	}
	this->life = life;
}

void Obj_Tank::SetItem(ObjectType type) 
{
	item = type;
}

void Obj_Tank::SetWeapon(WEAPON type)
{
	weapon_info.type = type;
}

int Obj_Tank::GetLife()
{
	return life;
}

int Obj_Tank::GetMaxLife()
{
	return max_life;
}

void Obj_Tank::InputShotMouse(const fPoint & turr_map_pos, fPoint & input_dir, fPoint & iso_dir)
{
	iPoint mouse_screen_pos = { 0, 0 };
	app->input->GetMousePosition(mouse_screen_pos.x, mouse_screen_pos.y);

	//Add the position of the mouse plus the position of the camera to have the pixel that selects the mouse in the world and then pass it to the map.
	mouse_screen_pos.x += camera_player->rect.x;
	mouse_screen_pos.y += camera_player->rect.y;

	input_dir = (fPoint)mouse_screen_pos - app->map->MapToScreenF(turr_map_pos);

	//Transform to map to work all variables in map(blit do MapToWorld automatically)
	iso_dir = app->map->ScreenToMapF(mouse_screen_pos.x, mouse_screen_pos.y) - turr_map_pos;
	iso_dir.Normalize();
}

void Obj_Tank::InputShotController(const fPoint & shot_pos, fPoint & input_dir, fPoint & iso_dir)
{
	if (controller != nullptr)
	{
		input_dir = (fPoint)(*controller)->GetJoystick(gamepad_aim);
		iso_dir.x = input_dir.x * cos_45 - input_dir.y * sin_45;
		iso_dir.y = input_dir.x * sin_45 + input_dir.y * cos_45;
		iso_dir.Normalize();
	}
}

void Obj_Tank::Shoot(float dt)
{
	//fPoint Obj_Tank::pos is on the center of the base
	//fPoint shot_pos is on the center of the turret (considers the cannon_height)
	turr_pos = pos_map - app->map->ScreenToMapF(  0, cannon_height );
	fPoint iso_dir (0.f, 0.f);
	fPoint input_dir (0.f, 0.f);
	if (shot_input == INPUT_METHOD::KEYBOARD_MOUSE)
	{
		InputShotMouse(turr_pos, input_dir, iso_dir);
	}
	else if (shot_input == INPUT_METHOD::CONTROLLER)
	{
		InputShotController(turr_pos, input_dir, iso_dir);
	}

	if (!input_dir.IsZero())
	{
		//Angle
		fPoint shot_screen_pos = app->map->MapToScreenF(iso_dir);
		float target_angle = atan2(-shot_screen_pos.y, shot_screen_pos.x) * RADTODEG;
		//- Calculate how many turns has the base angle and apply them to the target angle
		float turns = floor(turr_angle / 360.f);
		target_angle += 360.f * turns;
		//- Check which distance is shorter. Rotating clockwise or counter-clockwise
		if (abs((target_angle + 360.f) - turr_angle) < abs(target_angle - turr_angle))
		{
			target_angle += 360.f;
		}
		turr_angle = lerp(turr_angle, target_angle, base_angle_lerp_factor * dt);

		//Direction
		shot_dir = iso_dir;//Keep the last direction to shoot bullets if the joystick is not being aimed
	}

	if (PressShot())
	{
		charged_timer.Start();
	}

	if (ReleaseShot() && shot_timer.ReadMs() >= weapon_info.time_between_bullets)
	{
		//- Basic shot
		if (charged_timer.ReadMs() < charge_time) 
		{
			(this->*basic_shot_function[(uint)weapon_info.type])();
			app->audio->PlayFx(shot_sound);
		}
		//- Charged shot
		else
		{
			(this->*charged_shot_function[(uint)weapon_info.type])();
			app->audio->PlayFx(shot_sound);
		}
		shot_timer.Start();
	}
}

bool Obj_Tank::PressShot()
{
	if (shot_input == INPUT_METHOD::KEYBOARD_MOUSE)
	{
		return app->input->GetMouseButton(kb_shoot) == KEY_DOWN;
	}
	else if (shot_input == INPUT_METHOD::CONTROLLER)
	{
		return (*controller)->GetTriggerState(gamepad_shoot) == KEY_DOWN;
	}
}

bool Obj_Tank::ReleaseShot()
{
	if (shot_input == INPUT_METHOD::KEYBOARD_MOUSE)
	{
		return app->input->GetMouseButton(kb_shoot) == KEY_UP;
	}
	else if (shot_input == INPUT_METHOD::CONTROLLER)
	{
		return (*controller)->GetTriggerState(gamepad_shoot) == KEY_UP;
	}
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

void Obj_Tank::ShootBasic()
{
	Obj_Bullet * bullet = (Obj_Bullet*)app->objectmanager->CreateObject(ObjectType::BASIC_BULLET, turr_pos + shot_dir * cannon_length);
	bullet->SetBulletProperties(
		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45);
}

void Obj_Tank::ShootFlameThrower()
{
}


void Obj_Tank::ReviveTank()
{
	switch (tank_num) {
	case 0:
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_interact) == KEY_DOWN && app->scene->tank_2->life == 0)		//TODO condicion collider
		{
			{
				app->scene->tank_2->speed = 4.f;
				app->scene->tank_2->life = 100;
			}
		}
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_interact) == KEY_DOWN && app->scene->tank_3->life == 0)
		{
			{
				app->scene->tank_3->speed = 4.f;
				app->scene->tank_3->life = 100;
			}
		}
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_interact) == KEY_DOWN && app->scene->tank_4->life == 0)
		{
			{
				app->scene->tank_4->speed = 4.f;
				app->scene->tank_4->life = 100;
			}
		}

		break;
	case 1:
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_interact) == KEY_DOWN && app->scene->tank_1->life == 0)
		{
			{
				app->scene->tank_1->speed = 4.f;
				app->scene->tank_1->life = 100;
			}
		}
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_interact) == KEY_DOWN && app->scene->tank_3->life == 0)
		{
			{
				app->scene->tank_3->speed = 4.f;
				app->scene->tank_3->life = 100;
			}
		}
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_interact) == KEY_DOWN && app->scene->tank_4->life == 0)
		{
			{
				app->scene->tank_4->speed = 4.f;
				app->scene->tank_4->life = 100;
			}
		}
		break;
	case 2:	
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_interact) == KEY_DOWN && app->scene->tank_1->life == 0)
		{
			{
				app->scene->tank_1->speed = 4.f;
				app->scene->tank_1->life = 100;
			}
		}
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_interact) == KEY_DOWN && app->scene->tank_2->life == 0)
		{
			{
				app->scene->tank_2->speed = 4.f;
				app->scene->tank_2->life = 100;
			}
		}
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_interact) == KEY_DOWN && app->scene->tank_4->life == 0)
		{
			{
				app->scene->tank_4->speed = 4.f;
				app->scene->tank_4->life = 100;
			}
		}
		break;
	case 3:
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_interact) == KEY_DOWN && app->scene->tank_1->life == 0)
		{
			{
				app->scene->tank_1->speed = 4.f;
				app->scene->tank_1->life = 100;
			}
		}
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_interact) == KEY_DOWN && app->scene->tank_2->life == 0)
		{
			{
				app->scene->tank_2->speed = 4.f;
				app->scene->tank_2->life = 100;
			}
		}
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_interact) == KEY_DOWN && app->scene->tank_3->life == 0)
		{
			{
				app->scene->tank_3->speed = 4.f;
				app->scene->tank_3->life = 100;
			}
		}
		break;
	}
}

void Obj_Tank::StopTank()
{

	//if (app->input->GetKey(SDL_SCANCODE_J) == KeyState::KEY_DOWN || app->input->GetKey(SDL_SCANCODE_J) == KeyState::KEY_REPEAT)  //testing life=0
	//	app->scene->tank_1->life = 0;
	//
	//if (app->input->GetKey(SDL_SCANCODE_K) == KeyState::KEY_DOWN || app->input->GetKey(SDL_SCANCODE_K) == KeyState::KEY_REPEAT)
	//	app->scene->tank_2->life = 0;

	//if (app->input->GetKey(SDL_SCANCODE_L) == KeyState::KEY_DOWN || app->input->GetKey(SDL_SCANCODE_L) == KeyState::KEY_REPEAT)
	//	app->scene->tank_3->life = 0;
	
	switch (tank_num) {
	case 0:
		if (life == 0)
		{
			app->scene->tank_1->speed = 0;
			app->scene->tank_1->angle = 0;
		}
		break;
	case 1:
		if (life == 0)
		{
			app->scene->tank_2->speed = 0;
			app->scene->tank_2->angle = 0;
		}
		break;
	case 2:
		if (life == 0)
		{
			app->scene->tank_3->speed = 0;
			app->scene->tank_3->angle = 0;
		}
		break;
	case 3:
		if (life == 0)
		{
			app->scene->tank_4->speed = 0;
			app->scene->tank_4->angle = 0;
		}
		break;
	}
}


void Obj_Tank::ShootDoubleMissile()
{
	fPoint double_missiles_offset = shot_dir;
	double_missiles_offset.RotateDegree(90);
	float missiles_offset = 0.2f;

	Bullet_Missile * left_missile = (Bullet_Missile*)app->objectmanager->CreateObject(ObjectType::BULLET_MISSILE, turr_pos + shot_dir * cannon_length + double_missiles_offset * missiles_offset);
	Bullet_Missile * right_missile = (Bullet_Missile*)app->objectmanager->CreateObject(ObjectType::BULLET_MISSILE, turr_pos + shot_dir * cannon_length - double_missiles_offset * missiles_offset);

	float bullet_angle = atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45;

	left_missile->SetBulletProperties(
		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		bullet_angle);

	right_missile->SetBulletProperties(
		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		bullet_angle);
}

void Obj_Tank::ShootHealingShot()
{
	Healing_Bullet * heal_bullet = (Healing_Bullet*)app->objectmanager->CreateObject(ObjectType::HEALING_BULLET, turr_pos + shot_dir * cannon_length);

	heal_bullet->SetBulletProperties(
		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45);
}

void Obj_Tank::Item()
{
	if(item != ObjectType::NO_TYPE
		&& (app->input->GetKey(kb_item) == KEY_DOWN
			|| (controller != nullptr
				&& (*controller)->GetButtonState(gamepad_item) == KEY_DOWN)))
	{
		Obj_Item * new_item = (Obj_Item*)app->objectmanager->CreateObject(item, pos_map);
		new_item->caster = this;
		new_item->Use();
		item = ObjectType::NO_TYPE;
	}
}


void Obj_Tank::SetPickUp(Obj_PickUp* pick_up)
{
	if (pick_up->type_of_pick_up == PICKUP_TYPE::ITEM)
	{
		SetItem(pick_up->type_of_item);
	}
	else
	{
		SetWeapon(pick_up->type_of_weapon);
	}

	pick_up->DeletePickUp();
}

