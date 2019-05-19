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
#include "Item_InstantHelp.h"
#include "Obj_Portal.h"
#include "HealingShot_Area.h"
#include "Obj_FlamethrowerFlame.h"

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
	revive_life = 100;
	revive_time = 2.f;
	cycle_bar_tex = app->tex->Load(tank_node.child("spritesheets").child("cycle_bar_tex").text().as_string());
	cycle_bar_anim.frames = app->anim_bank->LoadFrames(app->anim_bank->animations_xml_node.child("cycle-progress-bar"));
	cycle_bar_anim.frames->SetSpeed((float)cycle_bar_anim.frames->GetMaxFrames() / revive_time);

	// ==========================================================

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
		dead_zone = DEFAULT_DEAD_ZONE;//TODO: Get from options menu
		break;
	case 1:
		kb_up		= SDL_SCANCODE_T;
		kb_left		= SDL_SCANCODE_F;
		kb_down		= SDL_SCANCODE_G;
		kb_right	= SDL_SCANCODE_H;
		kb_item		= SDL_SCANCODE_R;
		kb_interact = SDL_SCANCODE_Y;
		kb_ready	= SDL_SCANCODE_V;
		dead_zone = DEFAULT_DEAD_ZONE;//TODO: Get from options menu
		break;
	case 2:
		kb_up		= SDL_SCANCODE_I;
		kb_left		= SDL_SCANCODE_J;
		kb_down		= SDL_SCANCODE_K;
		kb_right	= SDL_SCANCODE_L;
		kb_item		= SDL_SCANCODE_U;
		kb_interact = SDL_SCANCODE_O;
		kb_ready	= SDL_SCANCODE_M;
		dead_zone = DEFAULT_DEAD_ZONE;//TODO: Get from options menu
		break;
	case 3:
		kb_up		= SDL_SCANCODE_KP_8;
		kb_left		= SDL_SCANCODE_KP_4;
		kb_down		= SDL_SCANCODE_KP_5;
		kb_right	= SDL_SCANCODE_KP_6;
		kb_item		= SDL_SCANCODE_KP_7;
		kb_interact	= SDL_SCANCODE_KP_9;
		kb_ready	= SDL_SCANCODE_KP_2;
		dead_zone = DEFAULT_DEAD_ZONE;//TODO: Get from options menu
		break;
	default:
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
	gamepad_shoot		= SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
	gamepad_item		= SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	gamepad_interact	= SDL_CONTROLLER_BUTTON_X;

	draw_offset.x = 46;
	draw_offset.y = 46;

	turr_draw_offset.x = 59;
	turr_draw_offset.y = 50;

	base_angle_lerp_factor = 11.25f;
	shot_angle_lerp_factor = 11.25f;

	shot_timer.Start();

	max_life = 100;
	SetLife(100);

	charged_shot_speed = 1.0f;

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

	SetItem(ItemType::HEALTH_BAG);
	time_between_portal_tp.Start();

	//Flamethrower
	coll_flame = app->collision->AddCollider(
		pos_map - fPoint(coll_w*0.5f, coll_h*0.5f),
		flame_coll_w,
		flame_coll_h,
		TAG::BULLET,
		BODY_TYPE::DYNAMIC,
		50.f,
		this);

	coll_flame->is_sensor = true;
	coll_flame->ActiveOnTrigger(false);

	flame = (Obj_FlamethrowerFlame*)app->objectmanager->CreateObject(ObjectType::FLAMETHROWER_FLAME, pos_map);
	flame->tank = this;

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
		if (coll->GetTag() == TAG::PLAYER)
		{
			coll->SetTag(TAG::GOD);
		}
		else
		{
			coll->SetTag(TAG::PLAYER);
		}
	}
	return true;
}

bool Obj_Tank::Update(float dt)
{
	Movement(dt);
	Aim(dt);//INFO: Aim always has to go before void Shoot()
	Shoot();
	Item();
	ReviveTank(dt);
	CameraMovement(dt);//Camera moves after the player and after aiming
	InputReadyKeyboard();
	

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
		if ((ReleaseShot()
			|| GetShotAutomatically())
			&& shot_timer.ReadMs() >= weapon_info.shot1.time_between_bullets)
		{
			//- Basic shot
			if (charged_shot_timer.ReadMs() < charge_time)
			{
				//set the max velocity in a basic shot
				velocity_recoil_curr_speed = velocity_recoil_speed_max;
			}

			//Item Happy hour activated
			else if (GetShotAutomatically())
			{
				velocity_recoil_curr_speed = velocity_recoil_speed_max * 0.75f;
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
	input = (fPoint)(*controller)->GetJoystick(gamepad_move, dead_zone);
}

bool Obj_Tank::Draw(float dt, Camera * camera)
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

	// Debug line
	if (show_crosshairs && camera == camera_player)
	{
		float line_length = 5.f;
		//1-- Set a position in the isometric space
		fPoint input_iso_pos(turr_pos.x + shot_dir.x * line_length, turr_pos.y + shot_dir.y * line_length);
		//2-- Transform that poin to screen coordinates
		iPoint input_screen_pos = (iPoint)app->map->MapToScreenF(input_iso_pos);
		app->render->DrawLineSplitScreen(
			pos_screen.x, pos_screen.y - cannon_height,
			input_screen_pos.x, input_screen_pos.y, 255, 0, 255, 255, camera);
	}
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

void Obj_Tank::OnTriggerEnter(Collider * c1)
{
	if (c1->GetTag() == TAG::FRIENDLY_BULLET)
	{
		Healing_Bullet* bullet = (Healing_Bullet*)c1->GetObj();
		if (bullet->player != this) // he does not heal himself
		{
			Obj_Healing_Animation* new_particle = (Obj_Healing_Animation*)app->objectmanager->CreateObject(ObjectType::HEALING_ANIMATION, pos_map);
			new_particle->tank = this;
			if (GetLife() < GetMaxLife())
			{
				SetLife(GetLife() + bullet->player->weapon_info.shot1.bullet_healing);
			}
		}
		else
		{
			bullet->to_remove = false; //if is himself, don't delete the bullet
		}
	}

	else if (c1->GetTag() == TAG::PORTAL)
	{
		if (time_between_portal_tp.ReadMs() > 2000) {
			if (c1 == portal1->coll) {
				pos_map = portal2->pos_map;
			}
			else if (c1 == portal2->coll) {
				pos_map = portal1->pos_map;
			}
			time_between_portal_tp.Start();
		}
	}

	else if (c1->GetTag() == TAG::HEALING_AREA_SHOT)
	{
		HealingShot_Area* area = (HealingShot_Area*)c1->GetObj();
		if (this->GetLife() < GetMaxLife())
		{
			Obj_Healing_Animation* new_particle = (Obj_Healing_Animation*)app->objectmanager->CreateObject(ObjectType::HEALING_ANIMATION, pos_map);
			new_particle->tank = this;
			this->SetLife(GetLife() + area->tank_parent->weapon_info.shot2.bullet_healing);
		}
	}
}

void Obj_Tank::OnTrigger(Collider * c1)
{
	if (c1->GetTag() == TAG::PICK_UP)
	{
		if (this->Alive())
		{
			tutorial_pick_up->SetStateToBranch(ELEMENT_STATE::VISIBLE);

			if (app->input->GetKey(kb_interact) == KEY_DOWN || PressInteract())
			{
				Obj_PickUp* pick_up = (Obj_PickUp*)c1->GetObj();
				SetPickUp(pick_up);
			}
		}
		else
		{
			tutorial_pick_up->SetStateToBranch(ELEMENT_STATE::HIDDEN);
		}
	}

	if (c1->GetTag() == TAG::ROAD && curr_speed < speed + road_buff)
	{
			curr_speed += road_buff;
	}
}

void Obj_Tank::OnTriggerExit(Collider * c1)
{
	if (c1->GetTag() == TAG::PICK_UP)
	{
		tutorial_pick_up->SetStateToBranch(ELEMENT_STATE::HIDDEN);
	}
	if (c1->GetTag() == TAG::ROAD && curr_speed >= speed + road_buff)
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

	else if (life <= 0)
	{
		this->life = 0;
		app->audio->PlayFx(die_sfx);
		StopTank();
	}
	else
	{
		this->life = life;
	}

	gui->SetLifeBar(this->life);
}

void Obj_Tank::ReduceLife(int damage)
{
	life -= damage;
	SetLife(life);
	damaged_timer.Start();
	damaged = true;
}

void Obj_Tank::SetItem(ItemType type)
{
	item = type;
	gui->SetItemIcon(type);
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
	if (controller != nullptr)
	{
		input_dir = (fPoint)(*controller)->GetJoystick(gamepad_aim, dead_zone);
		iso_dir.x = input_dir.x * cos_45 - input_dir.y * sin_45;
		iso_dir.y = input_dir.x * sin_45 + input_dir.y * cos_45;
		iso_dir.Normalize();
	}
}

void Obj_Tank::Shoot()
{
	switch (weapon_info.type)
	{
	case WEAPON_TYPE::CHARGED:
		ShootChargedWeapon();
		break;
	case WEAPON_TYPE::SUSTAINED:
		ShootSustainedWeapon();
		break;
	default:
		LOG("Weapon type not set correctly");
		break;
	}
}

void Obj_Tank::ShootChargedWeapon()
{
	if (PressShot())
	{
		charged_shot_timer.Start();
	}

	if (HoldShot())
	{
		if (charged_shot_timer.ReadMs() / charge_time > 0.1f)
		{
			this->curr_speed = charged_shot_speed;
			gui->SetChargedShotBar(charged_shot_timer.ReadMs() / charge_time);
		}
	}

	if ((ReleaseShot()
		|| GetShotAutomatically())
		&& shot_timer.ReadMs() >= weapon_info.shot1.time_between_bullets)
	{
		this->curr_speed = speed;
		//- Basic shot
		if (charged_shot_timer.ReadMs() < charge_time)
		{
			(this->*shot1_function[(uint)weapon_info.weapon])();
			app->audio->PlayFx(shot_sound);
			camera_player->AddTrauma(weapon_info.shot1.trauma);
			if (controller != nullptr) { (*controller)->PlayRumble(weapon_info.shot1.rumble_strength, weapon_info.shot1.rumble_duration); }
			app->objectmanager->CreateObject(weapon_info.shot1.smoke_particle, turr_pos + shot_dir * 1.2f);
		}
		//- Charged shot
		else
		{
			(this->*shot2_function[(uint)weapon_info.weapon])();
			app->audio->PlayFx(shot_sound);
			camera_player->AddTrauma(weapon_info.shot2.trauma);
			if (controller != nullptr) { (*controller)->PlayRumble(weapon_info.shot2.rumble_strength, weapon_info.shot2.rumble_duration); }
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
	}

	//- Sustained shot
	if (HoldShot()
		&& sustained_shot_timer.ReadMs() > quick_shot_time)
	{
		(this->*shot2_function[(uint)weapon_info.weapon])();
		//TODO: Play wepon sfx
		if (controller != nullptr) { (*controller)->PlayRumble(weapon_info.shot2.rumble_strength, weapon_info.shot2.rumble_duration); }
	}

	//- Quick shot
	if (ReleaseShot())
	{
		if (shot_timer.ReadMs() >= weapon_info.shot1.time_between_bullets
			&& sustained_shot_timer.ReadMs() <= quick_shot_time
			&& GetShotAutomatically())
		{
			(this->*shot1_function[(uint)weapon_info.weapon])();
			if (controller != nullptr) { (*controller)->PlayRumble(weapon_info.shot1.rumble_strength, weapon_info.shot1.rumble_duration); }
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
		if (abs((turr_target_angle + 360.f) - turr_angle) < abs(turr_target_angle - turr_angle))
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
		&& (controller != nullptr
		&& !(*controller)->GetJoystick(gamepad_move, dead_zone).IsZero()))
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
		&& (!(*controller)->GetJoystick(gamepad_aim, dead_zone).IsZero()
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
	if (this->fire_dead==false)
	{
		Obj_Fire* dead_fire = (Obj_Fire*)app->objectmanager->CreateObject(ObjectType::FIRE_DEAD, pos_map);
		dead_fire->tank = this;
		this->fire_dead = true;
	}

	this->SetWeapon(WEAPON::BASIC, 1);
	this->SetItem(ItemType::NO_TYPE);
}

bool Obj_Tank::Alive() const
{
	return life > 0;
}

void Obj_Tank::Item()
{
	if(item != ItemType::NO_TYPE
		&& (app->input->GetKey(kb_item) == KEY_DOWN
			|| (controller != nullptr
				&& (*controller)->GetButtonState(gamepad_item) == KEY_DOWN)))
	{
		Obj_Item * new_item = (Obj_Item*)app->objectmanager->CreateItem(item, pos_map);
		new_item->caster = this;
		new_item->Use();
		item = ItemType::NO_TYPE;
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

int Obj_Tank::GetTankNum() const
{
	return tank_num;
}

Controller ** Obj_Tank::GetController()
{
	return controller;
}

void Obj_Tank::InputReadyKeyboard()
{
	if (app->scene->game_state == GAME_STATE::OUT_WAVE && app->input->GetKey(kb_ready) == KEY_DOWN)
	{
		ready = !ready;
	}
	else if (app->scene->game_state != GAME_STATE::OUT_WAVE)
	{
		ready = false;
	}
}

fPoint Obj_Tank::GetShotDir() const
{
	return shot_dir;
}

void Obj_Tank::CreatePortals()
{
	portal1 = (Obj_Portal*)app->objectmanager->CreateObject(ObjectType::PORTAL, pos_map + shot_dir * 5);

	portal2 = (Obj_Portal*)app->objectmanager->CreateObject(ObjectType::PORTAL, pos_map - shot_dir * 5);
}

float Obj_Tank::GetTurrAngle() const
{
	return turr_angle;
}
