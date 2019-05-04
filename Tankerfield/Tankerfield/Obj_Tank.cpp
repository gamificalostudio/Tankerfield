#include <math.h>

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
#include "Obj_Fire.h"
#include "Obj_PickUp.h"
#include "M_AnimationBank.h"
#include "Player_GUI.h"
#include "UI_InGameElement.h"
#include "M_UI.h"
#include "M_ObjManager.h"
#include "Camera.h"

int Obj_Tank::number_of_tanks = 0;

Obj_Tank::Obj_Tank(fPoint pos) : Object(pos)
{
	tank_num = number_of_tanks++;
}

Obj_Tank::~Obj_Tank()
{
	number_of_tanks--;
	if (app->on_clean_up == false)
	{
		if (tutorial_move != nullptr)
		{
			tutorial_move->Destroy();
		}
		if (tutorial_pick_up != nullptr)
		{
			tutorial_pick_up->Destroy();
		}
		if (tutorial_revive != nullptr)
		{
			tutorial_revive->Destroy();
		}
	}

	if (camera_player != nullptr)
	{
		camera_player->assigned = false;
		camera_player = nullptr;
	}

	if (controller != nullptr)
	{
		(*controller)->DetachController();
	}
}

bool Obj_Tank::Start()
{
	pugi::xml_node tank_node = app->config.child("object").child("tank");

	pugi::xml_node tank_node_recoil = app->config.child("object").child("tank").child("recoil");

	velocity_recoil_decay = tank_node_recoil.child("velocity_recoil_decay").attribute("value").as_float();
	velocity_recoil_speed_max = tank_node_recoil.child("velocity_recoil_speed_max").attribute("value").as_float();
	velocity_recoil_speed_max_charged = tank_node_recoil.child("velocity_recoil_speed_max_charged").attribute("value").as_float();
	lerp_factor_recoil = tank_node_recoil.child("lerp_factor_recoil").attribute("value").as_float();

	//Textures-------------------------------------------------
	//-- Base
	base_tex_orange = app->tex->Load(tank_node.child("spritesheets").child("base_orange").text().as_string());
	base_tex_green = app->tex->Load(tank_node.child("spritesheets").child("base_green").text().as_string());
	base_tex_pink = app->tex->Load(tank_node.child("spritesheets").child("base_pink").text().as_string());
	base_tex_blue = app->tex->Load(tank_node.child("spritesheets").child("base_blue").text().as_string());

	base_shadow_tex = app->tex->Load(tank_node.child("spritesheets").child("base_shadow").text().as_string());
	SDL_SetTextureBlendMode(base_shadow_tex, SDL_BLENDMODE_MOD);

	//-- Turr
	std::string aux = tank_node.child("spritesheets").child("turr_orange").text().as_string();
	turr_tex_orange = app->tex->Load(tank_node.child("spritesheets").child("turr_orange").text().as_string());
	turr_tex_green = app->tex->Load(tank_node.child("spritesheets").child("turr_green").text().as_string());
	turr_tex_pink = app->tex->Load(tank_node.child("spritesheets").child("turr_pink").text().as_string());
	turr_tex_blue = app->tex->Load(tank_node.child("spritesheets").child("turr_blue").text().as_string());
	turr_shadow_tex = app->tex->Load(tank_node.child("spritesheets").child("turr_shadow").text().as_string());
	SDL_SetTextureBlendMode(turr_shadow_tex, SDL_BLENDMODE_MOD);
	//-- Revive 
	revive_range = 2.5f;
	revive_range_squared = revive_range * revive_range;
	revive_life = 100;
	revive_time = 2.f;
	cycle_bar_tex = app->tex->Load(tank_node.child("spritesheets").child("cycle_bar_tex").text().as_string());
	cycle_bar_anim.frames = app->anim_bank->LoadFrames(app->anim_bank->animations_xml_node.child("cycle-progress-bar"));
	cycle_bar_anim.frames->SetSpeed((float)cycle_bar_anim.frames->GetMaxFrames() / revive_time);


	//sfx -------------------------------------------------------------------------------------------------------
	shot_sound = app->audio->LoadFx(tank_node.child("sounds").child("basic_shot").attribute("sound").as_string());
	revive_sfx = app->audio->LoadFx("audio/Fx/tank/revivir.wav");
	die_sfx = app->audio->LoadFx("audio/Fx/tank/death-sfx.wav");

	switch (tank_num) {
	case 0:
		kb_up		= SDL_SCANCODE_W;
		kb_left		= SDL_SCANCODE_A;
		kb_down		= SDL_SCANCODE_S;
		kb_right	= SDL_SCANCODE_D;
		kb_item		= SDL_SCANCODE_Q;
		kb_interact	= SDL_SCANCODE_E;
		kb_ready	= SDL_SCANCODE_Z;
		curr_tex = base_tex_green;
		turr_tex = turr_tex_green;
		break;
	case 1:
		kb_up		= SDL_SCANCODE_T;
		kb_left		= SDL_SCANCODE_F;
		kb_down		= SDL_SCANCODE_G;
		kb_right	= SDL_SCANCODE_H;
		kb_item		= SDL_SCANCODE_R;
		kb_interact = SDL_SCANCODE_Y;
		kb_ready	= SDL_SCANCODE_V;
		curr_tex = base_tex_blue;
		turr_tex = turr_tex_blue;

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
		turr_tex = turr_tex_pink;

		break;
	case 3:
		kb_up		= SDL_SCANCODE_KP_8;
		kb_left		= SDL_SCANCODE_KP_4;
		kb_down		= SDL_SCANCODE_KP_5;
		kb_right	= SDL_SCANCODE_KP_6;
		kb_item		= SDL_SCANCODE_KP_7;
		kb_interact	= SDL_SCANCODE_KP_9;
		kb_ready	= SDL_SCANCODE_KP_2;
		curr_tex = base_tex_orange;
		turr_tex = turr_tex_orange;

		break;
	default:
		curr_tex = base_tex_orange;
		LOG("Number of tanks is greater than 3. You probably restarted the game and need to set the variable to 0 again.");
		break;
	}
	kb_shoot = SDL_BUTTON_LEFT;

	rotate_base.frames = app->anim_bank->LoadFrames(tank_node.child("animations").child("rotate_base"));
	curr_anim = &rotate_base;

	rotate_turr.frames = app->anim_bank->LoadFrames(tank_node.child("animations").child("rotate_turr"));

	curr_speed = speed = 5.f;//TODO: Load from xml
	road_buff = 3.f;
	cos_45 = cosf(-45 * DEGTORAD);
	sin_45 = sinf(-45 * DEGTORAD);

	InitWeapons();

	coll = app->collision->AddCollider(pos_map, 0.8f, 0.8f, Collider::TAG::PLAYER,0.f,this);
	coll->AddRigidBody(Collider::BODY_TYPE::DYNAMIC);
	coll->SetObjOffset({ -0.4f, -0.4f });

	cannon_height = 11.f;

	gamepad_move		= Joystick::LEFT;
	gamepad_aim			= Joystick::RIGHT;
	gamepad_shoot		= SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
	gamepad_item		= SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	gamepad_interact	= SDL_CONTROLLER_BUTTON_X;

	draw_offset.x = 46;
	draw_offset.y = 46;

	base_angle_lerp_factor = 11.25f;
	shot_angle_lerp_factor = 11.25f;

	shot_timer.Start();

	life = 90;
	max_life = 100;

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
	tutorial_move->AddButtonHelper(CONTROLLER_BUTTON::L, {0.f, 100.f});
	tutorial_move->AddTextHelper("MOVE", {0.f, 70.f});
	tutorial_move_time = 4500;
	movement_timer.Start();
	////- Revive
	tutorial_revive = app->ui->CreateInGameHelper(pos_map, clue_def);
	tutorial_revive->single_camera = camera_player;
	tutorial_revive->AddButtonHelper(CONTROLLER_BUTTON::X, { 0.f, 100.f });
	tutorial_revive->AddTextHelper("REVIVE", { 0.f, 70.f });
	tutorial_revive->SetStateToBranch(ELEMENT_STATE::HIDDEN);

	////- PickUp
	tutorial_pick_up = app->ui->CreateInGameHelper(pos_map, clue_def);
	tutorial_pick_up->single_camera = camera_player;
	tutorial_pick_up->AddButtonHelper(CONTROLLER_BUTTON::X, { 0.f, 100.f });
	tutorial_pick_up->AddTextHelper("TAKE", { 0.f, 70.f });
	tutorial_pick_up->SetStateToBranch(ELEMENT_STATE::HIDDEN);

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
	if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
	{
		if (coll->GetTag() == Collider::TAG::PLAYER)
		{
			coll->SetTag(Collider::TAG::GOD);
		}
		else
		{
			coll->SetTag(Collider::TAG::PLAYER);
		}
	}
	return true;
}

bool Obj_Tank::Update(float dt)
{
	Movement(dt);
	Shoot(dt);
	Item();
	StopTank();
	ReviveTank(dt);
	CameraMovement(dt);//Camera moves after the player
	InputReadyKeyboard();

	return true;
}

void Obj_Tank::CameraMovement(float dt)
{
	if (camera_player == nullptr)
		return;

	camera_player->ResetCamera();
	camera_player->FollowPlayer(dt, this);
	camera_player->ShakeCamera(dt);
}

fPoint Obj_Tank::GetTurrPos()
{
	return turr_pos;
}

void Obj_Tank::Movement(float dt)
{
	if (!tutorial_move_pressed)
		tutorial_move_timer.Start();

	tutorial_move_pressed = true;

	//Don't move if tank is dead
	if (life <= 0) {
		return;
	}

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
		if (movement_timer.ReadSec() >= 0.7)
		{
			movement_timer.Start();
		}

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

	ShotRecoilMovement(dt);

	pos_map += velocity;

	


	if (tutorial_move != nullptr && tutorial_move_pressed && tutorial_move_timer.Read() > tutorial_move_time)
	{
		tutorial_move->Destroy();
		tutorial_move = nullptr;
	}
}

void Obj_Tank::ShotRecoilMovement(float &dt)
{
	if (this->life != 0) {
		//if the player shot
		if (ReleaseShot() && shot_timer.ReadMs() >= weapon_info.time_between_bullets)
		{
			//- Basic shot
			if (charged_timer.ReadMs() < charge_time)
			{
				//set the max velocity in a basic shot
				velocity_recoil_curr_speed = velocity_recoil_speed_max;
			}
			//- Charged shot
			else
			{
				//set the max velocity in a charged shot
				velocity_recoil_curr_speed = velocity_recoil_speed_max_charged;
			}
			// set the direction when shot
			recoil_dir = -GetShotDir();
		}
		else
		{
			//reduce the velocity to 0 with decay
			if (velocity_recoil_curr_speed > 0)
			{
				velocity_recoil_curr_speed -= velocity_recoil_decay * dt;
				if (velocity_recoil_curr_speed < 0)
				{
					velocity_recoil_curr_speed = 0;
				}
			}
		}
		//calculate the max position of the lerp
		velocity_recoil_final_lerp = recoil_dir * velocity_recoil_curr_speed * dt;

		//calculate the velocity in lerp
		//velocity_recoil_lerp = lerp({ 0,0 }, velocity_recoil_final_lerp, 0.5f*dt);
		velocity += velocity_recoil_final_lerp;
	}
}

void Obj_Tank::InputMovementKeyboard(fPoint & input)
{
	if (app->input->GetKey(kb_up) == KEY_DOWN || app->input->GetKey(kb_up) == KEY_REPEAT)
	{
		input.y -= 1.f;
	}
	if (app->input->GetKey(kb_left) == KEY_DOWN || app->input->GetKey(kb_left) == KEY_REPEAT)
	{
		input.x -= 1.f;
	}
	if (app->input->GetKey(kb_down) == KEY_DOWN || app->input->GetKey(kb_down) == KEY_REPEAT)
	{
		input.y += 1.f;
	}
	if (app->input->GetKey(kb_right) == KEY_DOWN || app->input->GetKey(kb_right) == KEY_REPEAT)
	{
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

	if (show_crosshairs && camera == camera_player)
	{
		float line_length = 5.f;
		//1-- Set a position in the isometric space
		fPoint input_iso_pos(turr_pos.x + shot_dir.x * line_length, turr_pos.y + shot_dir.y * line_length);
		//2-- Transform that poin to screen coordinates
		iPoint input_screen_pos = (iPoint)app->map->MapToScreenF(input_iso_pos);
		app->render->DrawLineSplitScreen(
			pos_screen.x, pos_screen.y - cannon_height,
			input_screen_pos.x, input_screen_pos.y, 0, 0, 255, 123, camera);
	}

	// Turret =======================================
	app->render->Blit(
		turr_tex,
		pos_screen.x - draw_offset.x,
		pos_screen.y - draw_offset.y,
		camera,
		&rotate_turr.GetFrame(turr_angle));

	return true;
}

bool Obj_Tank::DrawShadow(Camera * camera, float dt)
{
	// Revive cycle =================================
	if (draw_revive_cycle_bar)
	{
		SDL_Rect rect = cycle_bar_anim.GetFrame(0.f);
		float scale_w = 0.63f;
		float scale_h = scale_w * 0.5f;//In isometric drawing, h is half of w
		app->render->BlitScaled(
			cycle_bar_tex,
			pos_screen.x - rect.w * scale_w * 0.5f - 2,//-2 and + 2 = offset because blit and draw iso circle don't draw completely on the same position
			pos_screen.y - rect.h * scale_h * 0.5f + 2,
			camera,
			&rect,
			scale_w,
			scale_h);
		//TO DO: (low importance) relate scale of the circle with the revive_range so it doesn't have to be adjusted manually
	}
	else if (life <= 0)
	{
		app->render->DrawIsoCircle(
			pos_screen.x,
			pos_screen.y,
			revive_range * app->map->data.tile_height,
			camera,
			200,
			200,
			200,
			123);// 30 = tile mesure
	}
	

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
		Healing_Bullet* bullet = (Healing_Bullet*)c1->GetObj();
		if (bullet->tank_parent != this) // he does not heal himself
		{
			Obj_Healing_Animation* new_particle = (Obj_Healing_Animation*)app->objectmanager->CreateObject(ObjectType::HEALING_ANIMATION, pos_map);
			new_particle->tank = this;
			if (GetLife() < GetMaxLife())
			{
				SetLife(GetLife() + weapon_info.bullet_healing);
			}
		}
		else
		{
			bullet->to_remove = false; //if is himself, don't delete the bullet
		}
	}

	if (c1->GetTag() == Collider::TAG::PICK_UP)
	{
		tutorial_pick_up->SetStateToBranch(ELEMENT_STATE::VISIBLE);
		
		if (app->input->GetKey(kb_interact) == KEY_DOWN || PressInteract())
		{
			Obj_PickUp* pick_up = (Obj_PickUp*)c1->GetObj();
			SetPickUp(pick_up);
		}
	}

	if (c1->GetTag() == Collider::TAG::ROAD && curr_speed < speed + road_buff)
	{
			curr_speed += road_buff;
	}
}

void Obj_Tank::OnTriggerExit(Collider * c1)
{
	if (c1->GetTag() == Collider::TAG::PICK_UP)
	{
		tutorial_pick_up->SetStateToBranch(ELEMENT_STATE::HIDDEN);
	}
	if (c1->GetTag() == Collider::TAG::ROAD && curr_speed >= speed + road_buff)
		{
			curr_speed = (curr_speed - road_buff) < speed ? speed : curr_speed - road_buff;
		}
}

void Obj_Tank::SetLife(int life)
{

	if (life > GetMaxLife())
	{
		this->life = GetMaxLife();
	}

	else if (life <= 0 && this->life != 0)
	{
		this->life = 0;
		app->audio->PlayFx(die_sfx);
	}
	else
	{
		this->life = life;
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
		weapon_info.bullet_damage = 25 + level * 2;
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
		weapon_info.bullet_damage = 0;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		break;
	case WEAPON::HEALING_SHOT:
		weapon_info.bullet_damage = 25+level;
		weapon_info.bullet_healing = 5 + level;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		break;
	case WEAPON::LASER_SHOT:
		weapon_info.bullet_damage = 10 + level * 2;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 20;
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
			if (weapon_info.type != WEAPON::LASER_SHOT)
			{
				camera_player->AddTrauma(0.54f);
			}
			else
			{
				camera_player->AddTrauma(0.54f * 0.75f);
			}
			if (controller != nullptr)
			{
				(*controller)->PlayRumble(0.92f, 250);
			}
		}
		//- Charged shot
		else
		{
			(this->*charged_shot_function[(uint)weapon_info.type])();
			app->audio->PlayFx(shot_sound);
			if(weapon_info.type != WEAPON::LASER_SHOT)
			{
				camera_player->AddTrauma(0.76f);
			}
			else
			{
				camera_player->AddTrauma(0.76f * 0.75f);
			}

			if (controller != nullptr)
			{
				(*controller)->PlayRumble(1.0f, 400);
			}

		}
		app->objectmanager->CreateObject(ObjectType::CANNON_FIRE, turr_pos + shot_dir * 1.2f);
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

void Obj_Tank::ReviveTank(float dt)
{
	//fPoint circlePos = { 3.f,3.f };
	//circlePos = app->map->MapToScreenF(circlePos);
	//app->render->DrawCircle(circlePos.x, circlePos.y, revive_range, 0, 255, 0, 100);

	if (draw_revive_cycle_bar)
	{
		cycle_bar_anim.NextFrame(dt);
	}

	bool can_revive = false;

	for (std::vector<Obj_Tank*>::iterator iter = app->objectmanager->obj_tanks.begin();
		iter != app->objectmanager->obj_tanks.end();
		++iter)
	{
		if (this != (*iter)
			&& pos_map.DistanceNoSqrt((*iter)->pos_map) <= revive_range_squared
			&& !(*iter)->Alive()
			&& Alive())
		{
			if (!can_revive)
			{
				can_revive = true;
			}

			//Presses the button
			if (!reviving_tank[(*iter)->tank_num] && PressInteract())
			{
				reviving_tank[(*iter)->tank_num] = true;
				revive_timer[(*iter)->tank_num].Start();
				(*iter)->cycle_bar_anim.Reset();
				(*iter)->draw_revive_cycle_bar = true;
			}
			//Releases the button
			else if (ReleaseInteract())
			{
				reviving_tank[(*iter)->tank_num] = false;
				(*iter)->draw_revive_cycle_bar = false;
				(*iter)->cycle_bar_anim.Reset();
			}

			//Finishes reviving the tank
			if (reviving_tank[(*iter)->tank_num] && (*iter)->cycle_bar_anim.Finished())
			{
				(*iter)->SetLife(revive_life);
				reviving_tank[(*iter)->tank_num] = false;
				(*iter)->fire_dead = false;
				app->audio->PlayFx(revive_sfx);
				(*iter)->draw_revive_cycle_bar = false;
				(*iter)->cycle_bar_anim.Reset();
			}
		}
		else
		{
			reviving_tank[(*iter)->tank_num] = false;
		}
	}

	if (can_revive /*&& tutorial_revive->GetState() != ELEMENT_STATE::VISIBLE*/)
	{
		tutorial_revive->SetStateToBranch(ELEMENT_STATE::VISIBLE);
	}
	else /*if (&tutorial_revive->GetState() != ELEMENT_STATE::HIDDEN)*/ {
		tutorial_revive->SetStateToBranch(ELEMENT_STATE::HIDDEN);
	}
}

bool Obj_Tank::PressInteract()
{
	return (controller != nullptr && ((*controller)->GetButtonState(gamepad_interact) == KEY_DOWN) || app->input->GetKey(kb_interact) == KeyState::KEY_DOWN);
}

bool Obj_Tank::ReleaseInteract()
{
	return (controller != nullptr && ((*controller)->GetButtonState(gamepad_interact) == KEY_UP) || (app->input->GetKey(kb_interact) == KeyState::KEY_UP));
}

void Obj_Tank::StopTank()
{
	if (life <= 0)
	{
		if (!fire_dead)
		{
			fire_dead = true;
			Obj_Fire* dead_fire = (Obj_Fire*)app->objectmanager->CreateObject(ObjectType::FIRE_DEAD, pos_map);
			dead_fire->tank = this;
		}
		this->SetWeapon(WEAPON::BASIC, 0);
		this->SetItem(ObjectType::NO_TYPE);
	}
}

bool Obj_Tank::Alive()
{
	return life > 0;
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
		gui->SetItemIcon(item);
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
	tutorial_pick_up->SetStateToBranch(ELEMENT_STATE::HIDDEN);
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

fPoint Obj_Tank::GetShotDir() const
{
	return shot_dir;
}

