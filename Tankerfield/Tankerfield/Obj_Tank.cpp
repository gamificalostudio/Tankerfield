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
#include "UI_Image.h"
#include "M_UI.h"
#include "M_ObjManager.h"
#include "Camera.h"
#include "Item_InstantHelp.h"
#include "Obj_Portal.h"
#include "HealingShot_Area.h"
#include "Obj_FlamethrowerFlame.h"
#include "M_Debug.h"
#include "Item_InstantHelp.h"
#include "M_PickManager.h"
#include "Obj_Enemy.h"
#include "Obj_Smoke.h"

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
		app->render->DestroyCamera(camera_player);
	}

	if (app->input->IsConnectedController(controller))
	{
		app->input->DetachController(controller);
	}
}

bool Obj_Tank::Start()
{
	pugi::xml_node tank_node = app->config.child("object").child("tank");

	pugi::xml_node tank_stats_node = app->objectmanager->balance_xml_node.child("tank");

	// Textures ================================================

	// Base -------------------------
	base_color_tex = app->tex->Load(tank_node.child("spritesheets").child("base_color").text().as_string());
	base_common_tex = app->tex->Load(tank_node.child("spritesheets").child("base_common").text().as_string());
	base_shadow_tex = app->tex->Load(tank_node.child("spritesheets").child("base_shadow").text().as_string());
	base_damaged_tex = app->tex->Load(tank_node.child("spritesheets").child("base_damaged").text().as_string());
	SDL_SetTextureBlendMode(base_shadow_tex, SDL_BLENDMODE_MOD);

	// Turret ------------------------
	turret_color_tex = app->tex->Load(tank_node.child("spritesheets").child("turret_color").text().as_string());
	turret_common_tex = app->tex->Load(tank_node.child("spritesheets").child("turret_common").text().as_string());
	turret_shadow_tex = app->tex->Load(tank_node.child("spritesheets").child("turret_shadow").text().as_string());
	tur_damaged_tex = app->tex->Load(tank_node.child("spritesheets").child("turret_damaged").text().as_string());
	SDL_SetTextureBlendMode(turret_shadow_tex, SDL_BLENDMODE_MOD);

	// Revive ------------------------ 
	revive_range = 2.5f;
	revive_range_squared = revive_range * revive_range;
	revive_life = 50;
	revive_time = 2.f;
	cycle_bar_tex = app->tex->Load(tank_node.child("spritesheets").child("cycle_bar_tex").text().as_string());
	cycle_bar_anim.frames = app->anim_bank->LoadFrames(app->anim_bank->animations_xml_node.child("cycle-progress-bar"));
	cycle_bar_anim.frames->SetSpeed((float)cycle_bar_anim.frames->GetMaxFrames() / revive_time);

	// ==========================================================

	//sfx -------------------------------------------------------------------------------------------------------
	shot_sound = app->audio->LoadFx(tank_node.child("sounds").child("basic_shot").attribute("sound").as_string(), 100);
	heal_sound = app->audio->LoadFx(tank_node.child("sounds").child("heal_shot").attribute("sound").as_string(), 100);
	laser_sound = app->audio->LoadFx(tank_node.child("sounds").child("laser_shot").attribute("sound").as_string(), 100);
	pick_item_sound = app->audio->LoadFx(tank_node.child("sounds").child("item_pick_up").attribute("sound").as_string(), 100);
	pick_weapon_sound = app->audio->LoadFx(tank_node.child("sounds").child("weapon_pick_up").attribute("sound").as_string(), 100);
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
		break;
	case 1:
		kb_up		= SDL_SCANCODE_T;
		kb_left		= SDL_SCANCODE_F;
		kb_down		= SDL_SCANCODE_G;
		kb_right	= SDL_SCANCODE_H;
		kb_item		= SDL_SCANCODE_R;
		kb_interact = SDL_SCANCODE_Y;
		break;
	case 2:
		kb_up		= SDL_SCANCODE_I;
		kb_left		= SDL_SCANCODE_J;
		kb_down		= SDL_SCANCODE_K;
		kb_right	= SDL_SCANCODE_L;
		kb_item		= SDL_SCANCODE_U;
		kb_interact = SDL_SCANCODE_O;
		break;
	case 3:
		kb_up		= SDL_SCANCODE_KP_8;
		kb_left		= SDL_SCANCODE_KP_4;
		kb_down		= SDL_SCANCODE_KP_5;
		kb_right	= SDL_SCANCODE_KP_6;
		kb_item		= SDL_SCANCODE_KP_7;
		kb_interact	= SDL_SCANCODE_KP_9;
		kb_ready	= SDL_SCANCODE_KP_2;
		break;
	default:
		LOG("Number of tanks is greater than 3. You probably restarted the game and need to set the variable to 0 again.");
		break;
	}
	dead_zone =	MAX_DEAD_ZONE*app->input->controllerInfo[tank_num].death_zone_porcenatage;//TODO: Get from options menu
	vibration_percentage = app->input->controllerInfo[tank_num].vibration_percentage;

	kb_shoot = SDL_BUTTON_LEFT;

	rotate_base.frames = app->anim_bank->LoadFrames(tank_node.child("animations").child("rotate_base"));
	curr_anim = &rotate_base;

	rotate_turr.frames = app->anim_bank->LoadFrames(tank_node.child("animations").child("rotate_turr"));

	base_max_speed = tank_stats_node.child("max_speed").attribute("value").as_float();

	charged_shot_buff.source = "charged_shot";
	charged_shot_buff.bonus_speed = tank_stats_node.child("charged_shot_buff").attribute("value").as_float();
	charged_shot_buff.has_decay = false;

	road_buff.source = "road";
	road_buff.bonus_speed = tank_stats_node.child("road_buff").attribute("value").as_float();
	road_buff.has_decay = true;
	road_buff.decay_rate = tank_stats_node.child("road_buff_decay_rate").attribute("value").as_float();

	recoil_buff.source = "recoil";
	recoil_buff.bonus_speed = tank_stats_node.child("recoil_buff").attribute("value").as_float();
	recoil_buff.has_decay = false;

	acceleration_power = tank_stats_node.child("acceleration_power").attribute("value").as_float();
	brake_power = tank_stats_node.child("brake_power").attribute("value").as_float();
	recoil_speed = tank_stats_node.child("recoil_speed").attribute("value").as_float();

	speed_colliding_with_building = 2.5f;

	cos_45 = cosf(-45 * DEGTORAD);
	sin_45 = sinf(-45 * DEGTORAD);

	run_over_damage_multiplier = 31.f;
	run_over_speed_reduction = 3.5f;

	camera_player = app->render->CreateCamera(this);
	app->ui->AddPlayerGUI(this);
	InitWeapons();

	float coll_w = 0.8f;
	float coll_h = 0.8f;
	coll = app->collision->AddCollider(pos_map, coll_w, coll_h, TAG::PLAYER, BODY_TYPE::DYNAMIC, 0.f,this);
	coll->SetObjOffset({ -coll_w * 0.5f, -coll_h * 0.5f });

	cannon_height = 16.f;

	gamepad_move		= Joystick::LEFT;
	gamepad_aim			= Joystick::RIGHT;
	gamepad_shoot		= app->input->controllerInfo[tank_num].attack_button;
	gamepad_item		= app->input->controllerInfo[tank_num].use_item_button;
	gamepad_interact	= app->input->controllerInfo[tank_num].interacton_button;

	draw_offset.x = 46;
	draw_offset.y = 46;

	turr_draw_offset.x = 59;
	turr_draw_offset.y = 50;

	base_angle_lerp_factor = 11.25f;
	shot_angle_lerp_factor = 11.25f;

	shot_timer.Start();

	max_life = life = 100;

	turr_scale = 1.2f;

	//- Tutorial
	//-- Move
	UI_InGameElementDef clue_def;
	clue_def.pointed_obj = this;

	tutorial_move = app->ui->CreateInGameHelper(pos_map, clue_def);
	tutorial_move->single_camera = camera_player;
	tutorial_move->AddButtonHelper(CONTROLLER_BUTTON::L, {0.f, 100.f});
	tutorial_move->AddTextHelper("MOVE", {0.f, 70.f});
	tutorial_move_time = 4500;
	////- Revive
	tutorial_revive = app->ui->CreateInGameHelper(pos_map, clue_def);
	tutorial_revive->single_camera = camera_player;
	tutorial_revive->AddButtonHelper(app->input->controllerInfo[tank_num].interacton_button, { 0.f, 100.f });
	tutorial_revive->AddTextHelper("REVIVE", { 0.f, 70.f });
	tutorial_revive->SetStateToBranch(ELEMENT_STATE::HIDDEN);

	////- PickUp
	tutorial_pick_up = app->ui->CreateInGameHelper(pos_map, clue_def);
	tutorial_pick_up->single_camera = camera_player;
	tutorial_pick_up->AddButtonHelper(app->input->controllerInfo[tank_num].interacton_button, { 0.f, 100.f });
	tutorial_pick_up->AddTextHelper("TAKE", { 0.f, 70.f });
	tutorial_pick_up->SetStateToBranch(ELEMENT_STATE::HIDDEN);

	SetItem(ItemType::HEALTH_BAG);
	time_between_portal_tp.Start();

	//Flamethrower
	coll_flame = app->collision->AddCollider(
		pos_map - fPoint(coll_w*0.5f, coll_h*0.5f),
		flame_coll_w,
		flame_coll_h,
		TAG::FLAMETHROWER,
		BODY_TYPE::DYNAMIC,
		50.f,
		this);

	coll_flame->is_sensor = true;
	coll_flame->SetIsTrigger(false);

	flame = (Obj_FlamethrowerFlame*)app->objectmanager->CreateObject(ObjectType::FLAMETHROWER_FLAME, pos_map);
	flame->tank = this;

	pugi::xml_node anim_node = app->anim_bank->animations_xml_node.child("charging").child("animation");

	text_charging = app->tex->Load("textures/Objects/tank/texture_charging.png");
	anim_charging.frames = app->anim_bank->LoadFrames(anim_node.child("charging"));

	text_finished_charged = app->tex->Load("textures/Objects/tank/texture_charging_finished.png");
	anim_finished_charged.frames = app->anim_bank->LoadFrames(anim_node.child("finish_charged"));

	charging_ready = app->audio->LoadFx("audio/Fx/tank/max_charged.wav");

	crosshair_tex = app->tex->Load("textures/Objects/tank/crosshair.png");

	this->time_between_portal_tp.Start();

	show_crosshairs = true;

	return true;
}

bool Obj_Tank::PreUpdate()
{
	if (!app->input->IsConnectedController(controller))
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
	UpdateMaxSpeedBuffs(dt);
	Movement(dt);
	Aim(dt);//INFO: Aim always has to go before void Shoot()
	Shoot(dt);
	Item();
	ReviveTank(dt);
	CameraMovement(dt);//Camera moves after the player and after aiming
	UpdateWeaponsWithoutBullets(dt);

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

fPoint Obj_Tank::GetTurrPos() const
{
	return turr_pos;
}

void Obj_Tank::Movement(float dt)
{
	if (!tutorial_move_pressed)
	{
		tutorial_move_timer.Start();
	}
	tutorial_move_pressed = true;

	//Don't move if tank is dead
	if (life <= 0)
	{
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

	//TODO: Instead of setting it to zero, add very high friction which makes it stop quickly
	bool no_input = false;
	if (input_dir.IsZero())
	{
		no_input = true;
	}

	fPoint brake_vector = -velocity_map;
	brake_vector.Normalize();
	brake_vector *= brake_power;

	if (no_input)
	{
		if (velocity_map.ModuleF() <= brake_power * dt)//If the velocity is less than the velocity we're going to substract, directly set it to zero
		{
			acceleration_map.SetToZero();
			velocity_map.SetToZero();
		}
		else
		{
			acceleration_map = brake_vector;
		}
	}
	else
	{
		acceleration_map = iso_dir * acceleration_power;
	}
	velocity_map += acceleration_map * dt;
	float max_speed = GetMaxSpeed();
	if (velocity_map.ModuleF() > max_speed)//If the module of the velocity is bigger than the speed
	{
		velocity_map.Normalize();
		velocity_map *= max_speed;
	}
	pos_map += velocity_map * dt;

	if (!no_input)
	{
		//CALCULATE ANGLE
		float target_angle = atan2(velocity_map.y, -velocity_map.x) * RADTODEG - ISO_COMPENSATION;
		//Calculate how many turns has the base angle and apply them to the target angle
		float turns = floor(angle / 360.f);
		target_angle += 360.f * turns;
		//Check which distance is shorter. Rotating clockwise or counter-clockwise
		if (abs(target_angle + 360.f - angle) < abs(target_angle - angle))
		{
			target_angle += 360.f;
		}
		angle = lerp(angle, target_angle, base_angle_lerp_factor * dt);
	}

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
	input = (fPoint)app->input->GetControllerJoystick(controller, gamepad_move, dead_zone);;
}

bool Obj_Tank::UpdateMaxSpeedBuffs(float dt)
{
	bool ret = true;
	for (std::list<MovementBuff>::iterator iter = movement_buffs.begin(); iter != movement_buffs.end();)
	{
		if ((*iter).decaying)
		{
			(*iter).bonus_speed -= (*iter).decay_rate * dt;
			if ((*iter).bonus_speed <= 0.f)
			{
				iter = movement_buffs.erase(iter);
			}
			else
			{
				++iter;
			}
		}
		else
		{
			++iter;
		}
	}
	return ret;
}

//Returns true if the buff has been added, returns false if there was already a buff of the same type and the new buff didn't have any effect
//Assumes that buffs from the same source will always have the same bonus
//We pass movement buff as a copy so we don't override the movement buff on Obj_Tank
bool Obj_Tank::AddMaxSpeedBuff(MovementBuff buff)
{
	bool ret = false;
	const char * source_char_ptr = buff.source.c_str();

	for (std::list<MovementBuff>::iterator iter = movement_buffs.begin(); iter != movement_buffs.end(); ++iter)
	{
		if (strcmp(source_char_ptr,(*iter).source.c_str()) == 0)
		{
			if ((*iter).decaying)
			{
				(*iter).decaying = false;
				(*iter).bonus_speed = buff.bonus_speed;
			}
			ret = true;
			break;
		}
	}
	if (ret != true)
	{
		movement_buffs.push_back(buff);
		ret = true;
	}
	return ret;
}

//Assumes all buffs have the same has_decay property
bool Obj_Tank::RemoveMaxSpeedBuff(std::string source)
{
	bool ret = false;
	const char * source_char_ptr = source.c_str();
	for (std::list<MovementBuff>::iterator iter = movement_buffs.begin(); iter != movement_buffs.end();)
	{
		if (strcmp((*iter).source.c_str(), source_char_ptr) == 0)
		{
			if ((*iter).has_decay)
			{
				(*iter).decaying = true;
				++iter;
			}
			else
			{
				iter = movement_buffs.erase(iter);
			}
			ret = true;
		}
		else
			++iter;
	}
	return ret;
}

float Obj_Tank::GetMaxSpeed()
{
	float total_bonus = 0.f;
	for (std::list<MovementBuff>::iterator iter = movement_buffs.begin(); iter != movement_buffs.end(); ++iter)
	{
		total_bonus += (*iter).bonus_speed;
	}
	return base_max_speed + total_bonus;
}

void Obj_Tank::ReduceSpeed(float reduction)
{
	float curr_speed = velocity_map.ModuleF();
	velocity_map.Normalize();
	velocity_map *= MAX(0.f, curr_speed - reduction);
}

void Obj_Tank::SetSpeed(float speed)
{
	velocity_map.Normalize();
	velocity_map *= speed;
}

bool Obj_Tank::Draw(Camera * camera)
{
	if (!damaged)
	{
		// Base common ========================================
		app->render->Blit(
			base_common_tex,
			pos_screen.x - draw_offset.x,
			pos_screen.y - draw_offset.y,
			camera,
			&curr_anim->GetFrame(angle));

		// Base color =========================================

		SDL_SetTextureColorMod(base_color_tex, tank_color.r, tank_color.g, tank_color.b);

		app->render->Blit(
			base_color_tex,
			pos_screen.x - draw_offset.x,
			pos_screen.y - draw_offset.y,
			camera,
			&curr_anim->GetFrame(angle));

		SDL_SetTextureColorMod(base_color_tex, 255, 255, 255);


		if (show_crosshairs
			&& camera == camera_player
			&& !app->IsPaused())
		{
			DrawCrosshair(camera);
		}

		// Turret common ======================================

		app->render->BlitScaled(
			turret_common_tex,
			pos_screen.x - turr_draw_offset.x,
			pos_screen.y - turr_draw_offset.y,
			camera,
			&rotate_turr.GetFrame(turr_angle),
			turr_scale,
			turr_scale);


		// Turret color =======================================

		SDL_SetTextureColorMod(turret_color_tex, tank_color.r, tank_color.g, tank_color.b);

		app->render->BlitScaled(
			turret_color_tex,
			pos_screen.x - turr_draw_offset.x,
			pos_screen.y - turr_draw_offset.y,
			camera,
			&rotate_turr.GetFrame(turr_angle),
			turr_scale,
			turr_scale);

		SDL_SetTextureColorMod(base_color_tex, 255, 255, 255);
	}
	else
	{
		// Base damaged ========================================
		app->render->Blit(
			base_damaged_tex,
			pos_screen.x - draw_offset.x,
			pos_screen.y - draw_offset.y,
			camera,
			&curr_anim->GetFrame(angle));

		if (show_crosshairs
			&& camera == camera_player
			&& !app->IsPaused())
		{
			DrawCrosshair(camera);
		}

		// Turret common ======================================
		app->render->BlitScaled(
			tur_damaged_tex,
			pos_screen.x - turr_draw_offset.x,
			pos_screen.y - turr_draw_offset.y,
			camera,
			&rotate_turr.GetFrame(turr_angle),
			turr_scale,
			turr_scale);
		if (damaged_timer.ReadSec() >= 0.4f)
		{
			damaged = false;
		}

	}

	if (HoldShot() && weapon_info.type == WEAPON_TYPE::CHARGED)
	{
		app->render->BlitAlphaAndScale(
			curr_text_charging,
			pos_screen.x-camera->rect.x - curr_anim_charging.GetFrame(0).w *0.5f * charging_scale + camera->screen_section.x,
			pos_screen.y - camera->rect.y - curr_anim_charging.GetFrame(0).h *0.5f * charging_scale + camera->screen_section.y,
			&curr_anim_charging.GetFrame(0),
			camera,
			alpha,
			charging_scale,
			charging_scale);
	}
	return true;
}

void Obj_Tank::DrawCrosshair(Camera * camera)
{
	//Pivot from which the croshair is going to be rotated
	SDL_Point crosshair_tex_pivot;
	crosshair_tex_pivot.x = 0;
	crosshair_tex_pivot.y = 2;

	//Pos1 = position of the turret in screen space
	fPoint turr_pos_screen(
		pos_screen.x,
		pos_screen.y - cannon_height);

	//Angle between the two positions
	fPoint shot_dir_max = shot_dir * 10000000;//Arbitrary number to reduce the decimals on the floating point
	iPoint shot_dir_screen = app->map->MapToScreenI(shot_dir_max.y, shot_dir_max.x);

	float crosshair_angle = atan2(
		shot_dir_screen.y,
		-shot_dir_screen.x)  * RADTODEG;

	app->render->BlitScaledAndRotated(
		crosshair_tex,
		turr_pos_screen.x - crosshair_tex_pivot.x,
		turr_pos_screen.y - crosshair_tex_pivot.y,
		camera,
		NULL,
		1.f, 1.f,
		crosshair_tex_pivot,
		crosshair_angle);

	//app->render->DrawLineSplitScreen(
	//	pos_screen.x, pos_screen.y - cannon_height,
	//	input_screen_pos.x, input_screen_pos.y, 0, 0, 255, 255, camera);

	//Natural aiming line
	//float line_length = 5.f;
	//TurrPos in screen space
	//iPoint point1 (pos_screen.x, pos_screen.y - cannon_height);
	//Input pos in screen space
	//iPoint point2 = point1 + app->input->GetControllerJoystick(controller, gamepad_aim);
	//2-- Transform that point to screen coordinates
	//app->render->DrawLineSplitScreen(
	//	point1.x, point1.y,
	//	point2.x, point2.y
	//	, 255, 255, 0, 255, camera);
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
	app->render->BlitScaled(
		turret_shadow_tex,
		pos_screen.x - turr_draw_offset.x,
		pos_screen.y - turr_draw_offset.y,
		camera,
		&rotate_turr.GetFrame(turr_angle),
		turr_scale,
		turr_scale);

	return true;
}

bool Obj_Tank::CleanUp()
{
	return true;
}

float Obj_Tank::GetCurrSpeed()
{
	return velocity_map.ModuleF();
}

void Obj_Tank::OnTriggerEnter(Collider * c1, float dt)
{
	switch (c1->GetTag())
	{
	case TAG::FRIENDLY_BULLET: {
		Healing_Bullet* bullet = (Healing_Bullet*)c1->GetObj();
		if (bullet->player != this && Alive()) // he does not heal himself
		{
			Obj_Healing_Animation* new_particle = (Obj_Healing_Animation*)app->objectmanager->CreateObject(ObjectType::HEALING_ANIMATION, pos_map);
			new_particle->tank = this;
			if (GetLife() < GetMaxLife())
			{
				IncreaseLife(bullet->player->weapon_info.shot1.bullet_healing);
				app->audio->PlayFx(heal_sound);
			}
		}
		else
		{
			bullet->to_remove = false; //if is himself, don't delete the bullet
		}
	} break;

	case TAG::PORTAL:
	{
		if (this->time_between_portal_tp.ReadMs() >= 1000)
		{
			Obj_Portal * portal = (Obj_Portal*)c1->GetObj();
			Item_InstantHelp * instant_help = portal->instant_help;
			instant_help->Teleport(this, portal);
			this->time_between_portal_tp.Start();
		}
	}break;

	case TAG::HEALING_AREA_SHOT: {
		HealingShot_Area* area = (HealingShot_Area*)c1->GetObj();
		if (this->GetLife() < GetMaxLife())
		{
			Obj_Healing_Animation* new_particle = (Obj_Healing_Animation*)app->objectmanager->CreateObject(ObjectType::HEALING_ANIMATION, pos_map);
			new_particle->tank = this;
			IncreaseLife(area->tank_parent->weapon_info.shot2.bullet_healing);
		}
	}break;

	case TAG::ROAD: {
		AddMaxSpeedBuff(road_buff);
	}break;

	case TAG::BULLET_ENEMY: {
		ReduceLife(c1->damage);
	}break;

	case TAG::ENEMY: {
		//If you collide against an enemy, you run over it, dealing damage and slowing you
		float curr_speed = GetCurrSpeed();
		if (curr_speed > 0.f)
		{
			Obj_Enemy * enemy = (Obj_Enemy*)c1->GetObj();
			enemy->ReduceLife(curr_speed * run_over_damage_multiplier, dt);
			ReduceSpeed(run_over_speed_reduction);
		}
	}break;

	//This two cases without a break are intentional. DO NOT CHANGE THIS.
	case TAG::WATER: //Fallthrough
	case TAG::WALL:
		SetSpeed(0.f);
		break;

	}
}

void Obj_Tank::OnTrigger(Collider * c1, float dt)
{
	switch (c1->GetTag()) {
	case TAG::PICK_UP: {
		if (this->Alive())
		{
			tutorial_pick_up->SetStateToBranch(ELEMENT_STATE::VISIBLE);

			if ((app->input->GetKey(kb_interact) == KEY_DOWN || PressInteract()) && !picking)
			{
				Obj_PickUp* pick_up = (Obj_PickUp*)c1->GetObj();

				SetPickUp(pick_up);
			}
		}
		else
		{
			tutorial_pick_up->SetStateToBranch(ELEMENT_STATE::HIDDEN);
		}
	}break;

	case TAG::ROAD: {
		AddMaxSpeedBuff(road_buff);
	}break;

	//This two cases without a break are intentional. DO NOT CHANGE THIS.
	case TAG::WATER: //Fallthrough
		SetSpeed(speed_colliding_with_building);
		SetSpeed(speed_colliding_with_building);
		break;

	}

}

void Obj_Tank::OnTriggerExit(Collider * c1)
{
	if (c1->GetTag() == TAG::PICK_UP)
	{
		tutorial_pick_up->SetStateToBranch(ELEMENT_STATE::HIDDEN);
	}
	if (c1->GetTag() == TAG::ROAD)
	{
		RemoveMaxSpeedBuff(road_buff.source);
	}
}

void Obj_Tank::IncreaseLife(int heal)
{
	life = MIN(max_life, life + heal);
	gui->SetLifeBar(this->life);
	gui->HealingFlash();
}

void Obj_Tank::ReduceLife(int damage)
{
	//Don't reduce life if we're in god mode or we're already dead
	if (!app->debug->god_mode
		&& life > 0)
	{
		life -= damage;
		//Tank death
		if (life <= 0)
		{
			life = 0;
			Die();
		}
		else if (life <= 33 && life > 0)
		{
			Obj_Smoke* damaged_smoke = (Obj_Smoke*)app->objectmanager->CreateObject(ObjectType::DAMAGED_SMOKE, pos_map);
			damaged_smoke->tank = this;
		}
		gui->SetLifeBar(this->life);
		damaged_timer.Start();
		damaged = true;
		gui->DamageFlash();
	}
}

void Obj_Tank::SetItem(ItemType type)
{
	item = type;
	gui->SetItem(type);
}

void Obj_Tank::SetColor(const SDL_Color new_color)
{
	tank_color = new_color;

	if (gui != nullptr)
	{
		gui->SetArrowColor(tank_color);
	}
}

WeaponInfo Obj_Tank::GetWeaponInfo() const 
{
	return weapon_info;
}

void Obj_Tank::SetTimeBetweenBullets(int time_between_bullets)
{
	weapon_info.shot1.time_between_bullets = time_between_bullets;
}

int Obj_Tank::GetLife() const
{
	return life;
}

int Obj_Tank::GetMaxLife() const
{
	return max_life;
}

int Obj_Tank::GetTimeBetweenBullets() const
{
	return weapon_info.shot1.time_between_bullets;
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
	if (app->input->IsConnectedController(controller))
	{
		input_dir = (fPoint)app->input->GetControllerJoystick(controller, gamepad_aim, dead_zone);
		iso_dir.x = input_dir.x * cos_45 - input_dir.y * sin_45;
		iso_dir.y = input_dir.x * sin_45 + input_dir.y * cos_45;
		iso_dir.Normalize();
	}
}

void Obj_Tank::Shoot(float dt)
{
	switch (weapon_info.type)
	{
	case WEAPON_TYPE::CHARGED:
		ShootChargedWeapon(dt);
		break;
	case WEAPON_TYPE::SUSTAINED:
		ShootSustainedWeapon();
		break;
	default:
		LOG("Weapon type not set correctly");
		break;
	}
}

void Obj_Tank::ShootChargedWeapon(float dt)
{
	if (PressShot())
	{
		charged_shot_timer.Start();
		charging_scale = 0.f;
		alpha = 0;
		curr_text_charging = text_charging;
		curr_anim_charging = anim_charging;
		
		charging = true;
	}

	if (HoldShot())
	{
		if (charged_shot_timer.ReadMs() / charge_time > 0.1f)
		{
			AddMaxSpeedBuff(charged_shot_buff);
			gui->SetChargedShotBar(charged_shot_timer.ReadMs() / charge_time);
			if (camera_player->GetShakeAmount() <= 0.05f)
			{
				camera_player->AddTrauma(0.02f);
			}
			if (app->input->IsConnectedController(controller))
			{ 
				app->input->ControllerPlayRumble(controller, 0.1f*vibration_percentage, 100);
			}

			if (charging)
			{
				if (alpha < 200)
				{
					alpha = charged_shot_timer.ReadMs() * 200 / charge_time;
					charging_scale = charged_shot_timer.ReadMs() * 1.50f / charge_time;
				}
				else
				{
					charging = false;
					curr_text_charging = text_finished_charged;
					curr_anim_charging = anim_finished_charged;
					charging_scale = 1;
					app->audio->PlayFx(charging_ready);
				}
			}

			curr_anim_charging.NextFrame(dt);

		}
	}

	if ((ReleaseShot()
		|| GetShotAutomatically())
		&& shot_timer.ReadMs() >= weapon_info.shot1.time_between_bullets)
	{
		RemoveMaxSpeedBuff(charged_shot_buff.source);
		anim_charging.Reset();
		//- Basic shot
		if (charged_shot_timer.ReadMs() < charge_time || GetShotAutomatically())
		{
			(this->*shot1_function[(uint)weapon_info.weapon])();
			app->audio->PlayFx(shot_sound);
			camera_player->AddTrauma(weapon_info.shot1.trauma);

			if (app->input->IsConnectedController(controller))
				app->input->ControllerPlayRumble(controller,weapon_info.shot1.rumble_strength*vibration_percentage, weapon_info.shot1.rumble_duration);

			app->objectmanager->CreateObject(weapon_info.shot1.smoke_particle, turr_pos + shot_dir * 1.2f);
			velocity_map -= shot_dir * recoil_speed;
		}
		//- Charged shot
		else
		{
			(this->*shot2_function[(uint)weapon_info.weapon])();
			app->audio->PlayFx(shot_sound);
			camera_player->AddTrauma(weapon_info.shot2.trauma);
			if (app->input->IsConnectedController(controller))
				app->input->ControllerPlayRumble(controller, weapon_info.shot2.rumble_strength*vibration_percentage, weapon_info.shot2.rumble_duration);

			app->objectmanager->CreateObject(weapon_info.shot2.smoke_particle, turr_pos + shot_dir * 1.2f);
		}
		shot_timer.Start();
		gui->SetChargedShotBar(0.f);
	}
}

void Obj_Tank::ShootSustainedWeapon()
{
	if (PressShot())
	{
		sustained_shot_timer.Start();
		shot_timer_basic_bullet.Start();
	}

	//- Sustained shot
	if (HoldShot()
		&& sustained_shot_timer.ReadMs() > weapon_info.quick_shot_time)
	{
		(this->*shot2_function[(uint)weapon_info.weapon])();
		camera_player->AddTrauma(weapon_info.shot1.trauma);
		//TODO: Play wepon sfx
		if (app->input->IsConnectedController(controller)) { app->input->ControllerPlayRumble(controller, weapon_info.shot2.rumble_strength*vibration_percentage, weapon_info.shot2.rumble_duration); }
	}

	//- Quick shot
	if (ReleaseShot())
	{
		if (shot_timer.ReadMs() >= weapon_info.shot1.time_between_bullets
			&& sustained_shot_timer.ReadMs() <= weapon_info.quick_shot_time
			&& GetShotAutomatically())
		{
			camera_player->AddTrauma(weapon_info.shot2.trauma);
			(this->*shot1_function[(uint)weapon_info.weapon])();
			if (app->input->IsConnectedController(controller)) { app->input->ControllerPlayRumble(controller, weapon_info.shot1.rumble_strength*vibration_percentage, weapon_info.shot1.rumble_duration); }
			app->objectmanager->CreateObject(weapon_info.shot1.smoke_particle, turr_pos + shot_dir * 1.2f);
			shot_timer.Start();
		}

		(this->*release_shot[(uint)weapon_info.weapon])();
	}
}

void Obj_Tank::Aim(float dt)
{
	//fPoint Obj_Tank::pos is on the center of the base
	//fPoint shot_pos is on the center of the turret (considers the cannon_height)
	turr_pos = pos_map - app->map->ScreenToMapF(0, cannon_height);
	fPoint iso_dir(0.f, 0.f);
	fPoint input_dir(0.f, 0.f);
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
		if (abs(turr_target_angle + 360.f - turr_angle) < abs(turr_target_angle - turr_angle))
		{
			turr_target_angle += 360.f;
		}

		//Direction
		shot_dir = iso_dir;//Keep the last direction to shoot bullets if the joystick is not being aimed
	}
	turr_angle = lerp(turr_angle, turr_target_angle, shot_angle_lerp_factor * dt);
}

bool Obj_Tank::PressShot()
{
	if (shot_input == INPUT_METHOD::KEYBOARD_MOUSE)
	{
		return app->input->GetMouseButton(kb_shoot) == KEY_DOWN;
	}
	else if (shot_input == INPUT_METHOD::CONTROLLER)
	{
		return app->input->GetControllerButtonOrTriggerState(controller, gamepad_shoot) == KEY_DOWN;
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
		return app->input->GetControllerButtonOrTriggerState(controller, gamepad_shoot) == KEY_REPEAT;
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
		return app->input->GetControllerButtonOrTriggerState(controller, gamepad_shoot) == KEY_UP;
	}
}

void Obj_Tank::ShotAutormaticallyActivate()
{
	shot_automatically = true;
}

void Obj_Tank::ShotAutormaticallyDisactivate()
{
	shot_automatically = false;
}

bool Obj_Tank::GetShotAutomatically() const
{
	return shot_automatically;
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
		&& (app->input->IsConnectedController(controller)
		&& !app->input->GetControllerJoystick(controller, gamepad_move, dead_zone).IsZero()))
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
		&& (app->input->IsConnectedController(controller)
			&& (!app->input->GetControllerJoystick(controller, gamepad_aim, dead_zone).IsZero()
				|| app->input->GetControllerButtonOrTriggerState(controller, gamepad_shoot) > 0)))
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
		if (this != (*iter))
		{
			if (pos_map.DistanceNoSqrt((*iter)->pos_map) <= revive_range_squared
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
					(*iter)->IncreaseLife(revive_life);
					reviving_tank[(*iter)->tank_num] = false;
					app->audio->PlayFx(revive_sfx);
					(*iter)->draw_revive_cycle_bar = false;
					(*iter)->cycle_bar_anim.Reset();
				}
			}
			else
			{
				if (reviving_tank[(*iter)->tank_num])
				{
					reviving_tank[(*iter)->tank_num] = false;
					(*iter)->draw_revive_cycle_bar = false;
				}
			}
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
	return (app->input->IsConnectedController(controller) && (app->input->GetControllerButtonOrTriggerState(controller, gamepad_interact) == KEY_DOWN) || app->input->GetKey(kb_interact) == KeyState::KEY_DOWN);
}

bool Obj_Tank::ReleaseInteract()
{
	return (app->input->IsConnectedController(controller) && (app->input->GetControllerButtonOrTriggerState(controller, gamepad_interact) == KEY_UP) || (app->input->GetKey(kb_interact) == KeyState::KEY_UP));
}

void Obj_Tank::Die()
{
	app->audio->PlayFx(die_sfx);
	Obj_Fire* dead_fire = (Obj_Fire*)app->objectmanager->CreateObject(ObjectType::FIRE_DEAD, pos_map);
	dead_fire->tank = this;
	SetWeapon(WEAPON::BASIC, app->scene->round);
	SetItem(ItemType::NO_TYPE);
}

bool Obj_Tank::Alive() const
{
	return life > 0;
}

void Obj_Tank::Item()
{
	if(item != ItemType::NO_TYPE
		&& (app->input->GetKey(kb_item) == KEY_DOWN
			|| (app->input->IsConnectedController(controller)
				&& app->input->GetControllerButtonOrTriggerState(controller, gamepad_item) == KEY_DOWN)))
	{
		Obj_Item * new_item = (Obj_Item*)app->objectmanager->CreateItem(item, pos_map);
		new_item->caster = this;
		new_item->Use();
		item = ItemType::NO_TYPE;
		gui->SetItem(item);
	}

	picking = false;
}


void Obj_Tank::SetPickUp(Obj_PickUp* pick_up)
{
	if (pick_up->type_of_pick_up == PICKUP_TYPE::ITEM)
	{
		if (item != ItemType::NO_TYPE)
		{
			app->pick_manager->CreatePickUp(pick_up->pos_map, PICKUP_TYPE::ITEM, item);
		}
		app->audio->PlayFx(pick_item_sound);
		SetItem(pick_up->type_of_item);
		gui->CreateParticleToItemFrame();
		
	}
	else
	{
		if (weapon_info.weapon != WEAPON::BASIC)
		{
			app->pick_manager->CreatePickUp(pick_up->pos_map,PICKUP_TYPE::WEAPON, ItemType::MAX_ITEMS, weapon_info.weapon, weapon_info.level_weapon);
		}
		app->audio->PlayFx(pick_weapon_sound);
		SetWeapon(pick_up->type_of_weapon, pick_up->level_of_weapon);
		gui->CreateParticleToWeaponFrame();
	}
	tutorial_pick_up->SetStateToBranch(ELEMENT_STATE::HIDDEN);
	pick_up->DeletePickUp();
	picking = true;
}

void Obj_Tank::SetGui(Player_GUI * gui)
{
	this->gui = gui;
}

int Obj_Tank::GetTankNum() const
{
	return tank_num;
}

int Obj_Tank::GetController()
{
	return controller;
}

fPoint Obj_Tank::GetShotDir() const
{
	return shot_dir;
}

float Obj_Tank::GetTurrAngle() const
{
	return turr_angle;
}