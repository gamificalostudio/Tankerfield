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
#include "Bullet_Laser.h"
#include "Healing_Bullet.h"
#include "Obj_HealingAnimation.h"
#include "Obj_PickUp.h"
#include "M_AnimationBank.h"
#include "Player_GUI.h"
#include "UI_InGameElement.h"
#include "M_UI.h"

int Obj_Tank::number_of_tanks = 0;

Obj_Tank::Obj_Tank(fPoint pos) : Object(pos)
{
	tank_num = number_of_tanks++;
}

Obj_Tank::~Obj_Tank()
{
	number_of_tanks--;
	if (camera_player != nullptr)
	{
		camera_player->assigned = false;
		camera_player->number_player = 0;
	}
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
		kb_ready	= SDL_SCANCODE_Z;
		curr_tex = base_tex_red;
		break;
	case 1:
		kb_up		= SDL_SCANCODE_T;
		kb_left		= SDL_SCANCODE_F;
		kb_down		= SDL_SCANCODE_G;
		kb_right	= SDL_SCANCODE_H;
		kb_item		= SDL_SCANCODE_R;
		kb_interact = SDL_SCANCODE_Y;
		kb_ready	= SDL_SCANCODE_V;
		curr_tex = base_tex_light_blue;
		break;
	case 2:
		kb_up		= SDL_SCANCODE_I;
		kb_left		= SDL_SCANCODE_J;
		kb_down		= SDL_SCANCODE_K;
		kb_right	= SDL_SCANCODE_L;
		kb_item		= SDL_SCANCODE_U;
		kb_interact = SDL_SCANCODE_O;
		kb_ready	= SDL_SCANCODE_M;
		curr_tex = base_tex_pink;
		break;
	case 3:
		kb_up		= SDL_SCANCODE_KP_8;
		kb_left		= SDL_SCANCODE_KP_4;
		kb_down		= SDL_SCANCODE_KP_5;
		kb_right	= SDL_SCANCODE_KP_6;
		kb_item		= SDL_SCANCODE_KP_7;
		kb_interact	= SDL_SCANCODE_KP_9;
		kb_ready	= SDL_SCANCODE_KP_2;
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

	curr_speed = speed = 4.f;//TODO: Load from xml

	cos_45 = cosf(-45 * DEGTORAD);
	sin_45 = sinf(-45 * DEGTORAD);

	//Basic weapon starting properties
	weapon_info.bullet_damage = 50;
	weapon_info.bullet_healing = 0;
	weapon_info.bullet_life_ms = 2000;
	weapon_info.bullet_speed = 10;
	weapon_info.time_between_bullets = 500;

	basic_shot_function[(uint)WEAPON::BASIC]			= &Obj_Tank::ShootBasic;
	basic_shot_function[(uint)WEAPON::DOUBLE_MISSILE]	= &Obj_Tank::ShootDoubleMissile;
	basic_shot_function[(uint)WEAPON::HEALING_SHOT]		= &Obj_Tank::ShootHealingShot;
	basic_shot_function[(uint)WEAPON::LASER_SHOT]		= &Obj_Tank::ShootLaserShot;


	charge_time = 3000.f; // Same for all bullets (player gets used to it)
	charged_shot_function[(uint)WEAPON::BASIC]			= &Obj_Tank::ShootBasic;
	charged_shot_function[(uint)WEAPON::DOUBLE_MISSILE] = &Obj_Tank::ShootDoubleMissile;
	charged_shot_function[(uint)WEAPON::HEALING_SHOT]	= &Obj_Tank::ShootHealingShot;

	coll = app->collision->AddCollider(pos_map, 0.8f, 0.8f, Collider::TAG::PLAYER,0.f,this);
	coll->AddRigidBody(Collider::BODY_TYPE::DYNAMIC);
	coll->SetObjOffset({ -0.4f, -0.4f });

	cannon_height = 11.f;
	cannon_length = 0.f;

	gamepad_move		= Joystick::LEFT;
	gamepad_aim			= Joystick::RIGHT;
	gamepad_shoot		= SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
	gamepad_item		= SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	gamepad_interact	= SDL_CONTROLLER_BUTTON_A;

	draw_offset.x = 46;
	draw_offset.y = 46;

	base_angle_lerp_factor = 11.25f;
	shot_angle_lerp_factor = 11.25f;

	shot_timer.Start();


	life = 90;
	max_life = 100;


	revive_range = 2.5f;
	revive_range_squared = revive_range * revive_range;
	revive_life = 100;




	//Life inicialistation
	//item = ObjectType::HEALTH_BAG;

	std::vector<Camera*>::iterator item_cam;
	for (item_cam = app->render->cameras.begin(); item_cam != app->render->cameras.end(); ++item_cam)
	{
		if (!(*item_cam)->assigned)
		{
			(*item_cam)->assigned = true;
			camera_player = (*item_cam);
			break;
		}
	}


	//- Tutorial
	//-- Move
	UI_InGameElementDef clue_def;
	clue_def.pointed_obj = this;

	tutorial_move = app->ui->CreateInGameHelper(pos_map, clue_def);
	tutorial_move->single_camera = camera_player;
	tutorial_move->AddButtonHelper(M_UI::GAMEPAD_BUTTON::L, {0.f, 100.f});
	tutorial_move->AddTextHelper("MOVE", {0.f, 70.f});
	tutorial_move_time = 4000;
	////- Revive
	tutorial_revive = app->ui->CreateInGameHelper(pos_map, clue_def);
	tutorial_revive->single_camera = camera_player;
	tutorial_revive->AddButtonHelper(M_UI::GAMEPAD_BUTTON::X, { 0.f, 100.f });
	tutorial_revive->AddTextHelper("REVIVE", { 0.f, 70.f });
	tutorial_revive->SetStateToBranch(ELEMENT_STATE::HIDDEN);

	return true;
}

bool Obj_Tank::PreUpdate()
{
	if (controller == nullptr)
	{
		controller = app->input->GetAbleController();
	}
	SelectInputMethod();

	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		show_crosshairs = !show_crosshairs;
	}

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
	InputReadyKeyboard();

	return true;
}

void Obj_Tank::CameraMovement(float dt)
{
	if (camera_player == nullptr)
		return;

	fPoint screen_pos = app->map->MapToScreenF(pos_map);
	fPoint target_pos =
	{
		(float)camera_player->rect.x,
		(float)camera_player->rect.y
	};

	//camera_player->rect.x = lerp(screen_pos.x - camera_player->rect.w * 0.5f, target_pos.x, 0.1f /*37.5f*/ * dt);
	//camera_player->rect.y = lerp(screen_pos.y - camera_player->rect.h * 0.5f, target_pos.y, 0.1f /*37.5f*/ * dt);

	camera_player->rect.x = screen_pos.x - (float) camera_player->rect.w * 0.5f;
	camera_player->rect.y = screen_pos.y - (float) camera_player->rect.h * 0.5f;
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
		tutorial_move_timer.Start();
		tutorial_move_pressed = true;

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

	velocity = iso_dir * curr_speed * dt;                                                               
	pos_map += velocity;

	if (tutorial_move != nullptr && tutorial_move_pressed && tutorial_move_timer.Read() > tutorial_move_time)
	{
		tutorial_move->Destroy();
		tutorial_move = nullptr;
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

	if (life == 0)
	{
		fPoint circlePos = pos_map;

		circlePos = app->map->MapToScreenF(circlePos);
		app->render->DrawIsoCircle(circlePos.x, circlePos.y, revive_range * 30, camera, 255, 0, 0, 100);// 30 = tile mesure
	}

	// Base =========================================
	app->render->Blit(
		curr_tex,
		pos_screen.x - draw_offset.x,
		pos_screen.y - draw_offset.y,
		camera,
		&curr_anim->GetFrame(angle));

	if (show_crosshairs && camera == camera_player)
	{
		float line_length = 5.f;
		//1-- Set a position in the isometric space
		fPoint input_iso_pos(turr_pos.x + shot_dir.x * line_length, turr_pos.y + shot_dir.y * line_length);
		//2-- Transform that poin to screen coordinates
		iPoint input_screen_pos = (iPoint)app->map->MapToScreenF(input_iso_pos);
		app->render->DrawLineSplitScreen(
			pos_screen.x, pos_screen.y - cannon_height,
			input_screen_pos.x, input_screen_pos.y, 255, 255, 255, 123, camera);
	}

	// Turret =======================================
	app->render->Blit(
		turr_tex,
		pos_screen.x - draw_offset.x,
		pos_screen.y - draw_offset.y,
		camera,
		&rotate_turr.GetFrame(turr_angle));

	

																							//only appears when hes dead and disappear when he has been revived
	//DEBUG
	//	iPoint debug_mouse_pos = { 0, 0 };
//	app->input->GetMousePosition(debug_mouse_pos.x, debug_mouse_pos.y);

//	debug_mouse_pos.x += camera_player->rect.x;
//	debug_mouse_pos.y += camera_player->rect.y;

//	fPoint shot_pos(pos_map - app->map->ScreenToMapF( 0.f, cannon_height ));
//	fPoint debug_screen_pos = app->map->MapToScreenF(shot_pos);

	//  std::vector<Camera*>::iterator item_cam;
//	for (item_cam = app->render->camera.begin(); item_cam != app->render->camera.end(); ++item_cam)
//	{
	//	app->render->DrawLineSplitScreen((*item_cam), debug_mouse_pos.x, debug_mouse_pos.y, debug_screen_pos.x, debug_screen_pos.y,  0, 255, 0);
//	}


	return true;
}

bool Obj_Tank::DrawShadow(Camera * camera)
{
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
	this->life = life;

	if (this->life > GetMaxLife())
	{
		this->life = GetMaxLife();
	}

	else if (this->life < 0)
	{
		this->life = 0;
	}

	gui->SetLifeBar(this->life);
}

void Obj_Tank::SetItem(ObjectType type) 
{
	item = type;
	gui->SetItemIcon(type);
}

void Obj_Tank::SetWeapon(WEAPON type, uint level)
{
	weapon_info.level_weapon = level;
	weapon_info.type = type;

	gui->SetWeaponIcon(type);

	switch (type)
	{
	case WEAPON::BASIC:
		weapon_info.bullet_damage = 50 + level * 2;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		break;
	case WEAPON::FLAMETHROWER:
		weapon_info.bullet_damage = 50 + level * 2;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		break;
	case WEAPON::DOUBLE_MISSILE:
		weapon_info.bullet_damage = 50 + level * 2;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		break;
	case WEAPON::HEALING_SHOT:
		weapon_info.bullet_damage = 0;
		weapon_info.bullet_healing = 25 + 1 * level;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		break;
	case WEAPON::LASER_SHOT:
		weapon_info.bullet_damage = 50 + level * 2;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		break;
	}

}

WeaponInfo Obj_Tank::GetWeaponInfo() const 
{
	return weapon_info;
}

void Obj_Tank::SetTimeBetweenBullets(int time_between_bullets)
{
	weapon_info.time_between_bullets = time_between_bullets;
}

int Obj_Tank::GetLife()
{
	return life;
}

int Obj_Tank::GetMaxLife()
{
	return max_life;
}

int Obj_Tank::GetTimeBetweenBullets()
{
	return weapon_info.time_between_bullets;
}

void Obj_Tank::InputShotMouse(const fPoint & turr_map_pos, fPoint & input_dir, fPoint & iso_dir)
{
	iPoint mouse_screen_pos = { 0, 0 };
	app->input->GetMousePosition(mouse_screen_pos.x, mouse_screen_pos.y);

	
	//Add the position of the mouse plus the position of the camera to have the pixel that selects the mouse in the world and then pass it to the map.

	if (camera_player != nullptr)
		mouse_screen_pos += {camera_player->rect.x, camera_player->rect.y};


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
		turr_target_angle = atan2(-input_dir.y, input_dir.x) * RADTODEG;
		//- Calculate how many turns has the base angle and apply them to the target angle
		float turns = floor(turr_angle / 360.f);
		turr_target_angle += 360.f * turns;
		//- Check which distance is shorter. Rotating clockwise or counter-clockwise
		if (abs((turr_target_angle + 360.f) - turr_angle) < abs(turr_target_angle - turr_angle))
		{
			turr_target_angle += 360.f;
		}

		//Direction
		shot_dir = iso_dir;//Keep the last direction to shoot bullets if the joystick is not being aimed
	}
	turr_angle = lerp(turr_angle, turr_target_angle, shot_angle_lerp_factor * dt);

	if (PressShot())
	{
		charged_timer.Start();
	}

	if (HoldShot())
	{
		if (charged_timer.ReadMs() / charge_time > 0.1f)
		{
			gui->SetChargedShotBar(charged_timer.ReadMs() / charge_time);
		}
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
		gui->SetChargedShotBar(0.f);
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

bool Obj_Tank::HoldShot()
{
	if (shot_input == INPUT_METHOD::KEYBOARD_MOUSE)
	{
		return app->input->GetMouseButton(kb_shoot) == KEY_REPEAT;
	}
	else if (shot_input == INPUT_METHOD::CONTROLLER)
	{
		return (*controller)->GetTriggerState(gamepad_shoot) == KEY_REPEAT;
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


	Obj_Tank* tank_arr[4];

	tank_arr[0] = app->scene->tank_1;
	tank_arr[1] = app->scene->tank_2;
	tank_arr[2] = app->scene->tank_3;
	tank_arr[3] = app->scene->tank_4;

	//fPoint circlePos = { 3.f,3.f };
	//circlePos = app->map->MapToScreenF(circlePos);
	//app->render->DrawCircle(circlePos.x, circlePos.y, revive_range, 0, 255, 0, 100);

	bool show_tutorial = false;

	for (int i = 0; i < 4; i++)
	{
		if (this != tank_arr[i]
			&& pos_map.DistanceNoSqrt(tank_arr[i]->pos_map) <= revive_range_squared
			&& tank_arr[i]->life == 0
			&& this->life != 0)
		{
			if (!show_tutorial)
			{
				show_tutorial = true;
			}

			if ((controller != nullptr && ((*controller)->GetButtonState(gamepad_interact) == KEY_DOWN)
				|| app->input->GetKey(kb_interact) == KeyState::KEY_DOWN || app->input->GetKey(kb_interact) == KeyState::KEY_REPEAT))
			{
				tank_arr[i]->curr_speed = speed;
				tank_arr[i]->SetLife(revive_life);
			}
		}
	}

	if (show_tutorial /*&& tutorial_revive.GetState() != ELEMENT_STATE::VISIBLE*/)
	{
		tutorial_revive->SetStateToBranch(ELEMENT_STATE::VISIBLE);
	}
	else /*if (&tutorial_revive.GetState() != ELEMENT_STATE::HIDDEN)*/ {
		tutorial_revive->SetStateToBranch(ELEMENT_STATE::HIDDEN);
	}
}

void Obj_Tank::StopTank()
{
	if (life == 0)
	{
		curr_speed = 0;
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

void Obj_Tank::ShootLaserShot()
{
	Laser_Bullet *	 laser_bullet= (Laser_Bullet*)app->objectmanager->CreateObject(ObjectType::BULLET_LASER, turr_pos + shot_dir * cannon_length);

	laser_bullet->SetBulletProperties(
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
		SetWeapon(pick_up->type_of_weapon, pick_up->level_of_weapon);
	}

	pick_up->DeletePickUp();
}

void Obj_Tank::SetGui(Player_GUI * gui)
{
	this->gui = gui;
}

bool Obj_Tank::IsReady() const
{
	return ready;
}

void Obj_Tank::InputReadyKeyboard()
{
	if (app->scene->stat_of_wave == WaveStat::OUT_WAVE && app->input->GetKey(kb_ready) == KEY_DOWN)
	{
		ready = !ready;
	}
	else if (app->scene->stat_of_wave != WaveStat::OUT_WAVE)
	{
		ready = false;
	}
}

