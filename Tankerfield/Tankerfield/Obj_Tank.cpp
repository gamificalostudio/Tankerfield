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
#include "Obj_PickUp.h"

SDL_Texture * Obj_Tank::base_tex_yellow		= nullptr;
//SDL_Texture * Obj_Tank::base_tex_orange		= nullptr;
SDL_Texture * Obj_Tank::base_tex_red		= nullptr;
//SDL_Texture * Obj_Tank::base_tex_light_green= nullptr;
SDL_Texture * Obj_Tank::base_tex_pink		= nullptr;
SDL_Texture * Obj_Tank::base_tex_light_blue	= nullptr;
//SDL_Texture * Obj_Tank::base_tex_dark_blue	= nullptr;
//SDL_Texture * Obj_Tank::base_tex_purple		= nullptr;
SDL_Texture * Obj_Tank::turr_tex			= nullptr;
SDL_Texture * Obj_Tank::base_shadow_tex		= nullptr;
SDL_Texture * Obj_Tank::turr_shadow_tex		= nullptr;
Animation   * Obj_Tank::rotate_base			= nullptr;
Animation   * Obj_Tank::rotate_turr			= nullptr;
WeaponInfo  * Obj_Tank::weapons_info		= nullptr;
int			  Obj_Tank::number_of_tanks		= 0;
//void       (* Obj_Tank::shot_function)()	= nullptr;//TODO: Test if function pointers can be static or they are executing the function on other tanks

Obj_Tank::Obj_Tank(fPoint pos) : Object(pos)
{}

bool Obj_Tank::Start()
{
	pugi::xml_node tank_node = app->config.child("object").child("tank");

	if (base_tex_yellow == nullptr)
	{
		Obj_Tank::base_tex_yellow = app->tex->Load(tank_node.child("spritesheets").child("base_yellow").text().as_string());
	}
	//if (base_tex_orange == nullptr)
	//{
	//	Obj_Tank::base_tex_orange = app->tex->Load(tank_node.child("spritesheets").child("base_orange").text().as_string());
	//}
	if (base_tex_red == nullptr)
	{
		Obj_Tank::base_tex_red = app->tex->Load(tank_node.child("spritesheets").child("base_red").text().as_string());
	}
	//if (base_tex_light_green == nullptr)
	//{
	//	Obj_Tank::base_tex_light_green = app->tex->Load(tank_node.child("spritesheets").child("base_light_green").text().as_string());
	//}
	if (base_tex_pink == nullptr)
	{
		Obj_Tank::base_tex_pink = app->tex->Load(tank_node.child("spritesheets").child("base_pink").text().as_string());
	}
	if (base_tex_light_blue == nullptr)
	{
		Obj_Tank::base_tex_light_blue = app->tex->Load(tank_node.child("spritesheets").child("base_light_blue").text().as_string());
	}
	//if (base_tex_dark_blue == nullptr)
	//{
	//	Obj_Tank::base_tex_dark_blue = app->tex->Load(tank_node.child("spritesheets").child("base_dark_blue").text().as_string());
	//}
	//if (base_tex_purple == nullptr)
	//{
	//	Obj_Tank::base_tex_purple = app->tex->Load(tank_node.child("spritesheets").child("babase_purplese").text().as_string());
	//}

	tank_num = number_of_tanks++;

	basic_shot_sound = app->audio->LoadFx(tank_node.child("sounds").child("basic_shot").attribute("sound").as_string());

	switch (tank_num) {
	case 0:
		curr_tex = base_tex_red;
		break;
	case 1:
		curr_tex = base_tex_light_blue;
		break;
	case 2:
		curr_tex = base_tex_pink;
		break;
	case 3:
		curr_tex = base_tex_yellow;
		break;
	default:
		curr_tex = base_tex_yellow;
		LOG("Number of tanks is greater than 3. You probably restarted the game and need to set the variable to 0 again.");
		break;
	}

	if (base_shadow_tex == nullptr)
	{
		Obj_Tank::base_shadow_tex = app->tex->Load(tank_node.child("spritesheets").child("base_shadow").text().as_string());
		SDL_SetTextureBlendMode(base_shadow_tex, SDL_BLENDMODE_MOD);
	}
	if (turr_tex == nullptr)
	{
		Obj_Tank::turr_tex = app->tex->Load(tank_node.child("spritesheets").child("turr").text().as_string());
	}
	if (turr_shadow_tex == nullptr)
	{
		Obj_Tank::turr_shadow_tex = app->tex->Load(tank_node.child("spritesheets").child("turr_shadow").text().as_string());
		SDL_SetTextureBlendMode(turr_shadow_tex, SDL_BLENDMODE_MOD);
	}

	if (rotate_base == nullptr)
	{
		rotate_base = new Animation (tank_node.child("animations").child("rotate_base"));
	}
	curr_anim = rotate_base;
	if (rotate_turr == nullptr)
	{
		rotate_turr = new Animation (tank_node.child("animations").child("rotate_turr"));
	}

	speed = 10.f;
		//2.5f;//TODO: Load from xml

	
	cos_45 = cosf(-45 * DEGTORAD);
	sin_45 = sinf(-45 * DEGTORAD);

	// Tanks life hardcoded 

	

	if (weapons_info == nullptr)
	{
		pugi::xml_node weapons_node = app->config.child("weapons");
		weapons_info = new WeaponInfo[(uint)WEAPON::MAX];
		weapons_info[(uint)WEAPON::BASIC].LoadProperties(weapons_node.child("basic"));
		weapons_info[(uint)WEAPON::FLAMETHROWER].LoadProperties(weapons_node.child("flamethrower"));
		weapons_info[(uint)WEAPON::DOUBLE_MISSILE].LoadProperties(weapons_node.child("double_missile"));
	}

	shot_function[(uint)WEAPON::BASIC] = &Obj_Tank::ShootBasic;
	shot_function[(uint)WEAPON::DOUBLE_MISSILE] = &Obj_Tank::ShootDoubleMissile;

	coll = app->collision->AddCollider(pos_map, 0.8f, 0.8f, Collider::TAG::PLAYER,0.f,this);
	coll->AddRigidBody(Collider::BODY_TYPE::DYNAMIC);
	coll->SetObjOffset({ -.4f, -.4f });

	cannon_height = 11.f;
	cannon_length = 1.f;

	//TODO: Load them from the XML
	kb_up			  = SDL_SCANCODE_W;
	kb_left			= SDL_SCANCODE_A;
	kb_down			= SDL_SCANCODE_S;
	kb_right		= SDL_SCANCODE_D;
	kb_shoot_basic		= SDL_BUTTON_RIGHT;
	kb_shoot_special	= SDL_BUTTON_LEFT;
	kb_item			= SDL_SCANCODE_F;
	kb_interact		= SDL_SCANCODE_SPACE;
  
	gamepad_move	= Joystick::LEFT;
	gamepad_aim		= Joystick::RIGHT;
	gamepad_shoot_basic		= SDL_CONTROLLER_AXIS_TRIGGERLEFT;
	gamepad_shoot_special	= SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
	gamepad_item		= SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	gamepad_revive_tank = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
	gamepad_interact	= SDL_CONTROLLER_BUTTON_A;

	draw_offset.x = 46;
	draw_offset.y = 46;

	base_angle_lerp_factor = 11.25f;

	basic_shot_timer.Start();

	life =  100;
	max_life = 200;

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
	Shoot();
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
		&rotate_turr->GetFrame(turr_angle));

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
		&rotate_turr->GetFrame(turr_angle));

	return true;
}

bool Obj_Tank::CleanUp()
{
	return true;
}


void Obj_Tank::OnTrigger(Collider * c1)
{

	if (c1->GetTag() == Collider::TAG::PICK_UP)
	{
		Obj_PickUp* pick_up = (Obj_PickUp*)c1->GetObj();
		if (app->input->GetKey(kb_interact) == KEY_DOWN || app->input->GetKey(gamepad_interact) == KEY_DOWN)
		{
			SetPickUp(pick_up);
		}
  }
	else if (c1->GetTag() == Collider::TAG::WALL)
	{
		app->scene->tank_1->life - 1;

	}
}

void Obj_Tank::SetLife(int life)
{
	//TODO: Update UI bars
	this->life = life;
}

void Obj_Tank::SetItem(ObjectType type) 
{
	item = type;
}

void Obj_Tank::SetWeapon(WEAPON type)
{
	special_shoot = (uint)type;
}

int Obj_Tank::GetLife()
{
	return life;
}

int Obj_Tank::GetMaxLife()
{
	return max_life;
}

void Obj_Tank::InputShotMouse(const fPoint & turr_pos, fPoint & input_dir, fPoint & iso_dir)
{
	iPoint mouse_pos = { 0, 0 };
	app->input->GetMousePosition(mouse_pos.x, mouse_pos.y);

	//Add the position of the mouse plus the position of the camera to have the pixel that selects the mouse in the world and then pass it to the map.
	mouse_pos.x += camera_player->rect.x;
	mouse_pos.y += camera_player->rect.y;

	int tile_width = 100, tile_height = 50;
  
	fPoint screen_pos = app->map->MapToScreenF(turr_pos);
	input_dir = (fPoint)mouse_pos - screen_pos;

	//Transform to map to work all variables in map(blit do MapToWorld automatically)
	fPoint map_mouse_pos = app->map->ScreenToMapF(mouse_pos.x,mouse_pos.y);

	iso_dir = map_mouse_pos - turr_pos;
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

void Obj_Tank::Shoot()
{
	//fPoint Obj_Tank::pos is on the center of the base
	//fPoint shot_pos is on the center of the turret (considers the cannon_height)
	turr_pos = pos_map - app->map->ScreenToMapF(  0, cannon_height );

	fPoint input_dir(0.f, 0.f);
	fPoint iso_dir;
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
		turr_angle = atan2(-input_dir.y, input_dir.x) * RADTODEG;
		shot_dir = iso_dir;//Keep the last direction to shoot bullets if the joystick is not being aimed
	}

	//- Special shoot (prioritize first the special shot)
	if (IsShootingSpecial() && special_shot_timer.ReadMs() >= weapons_info[(uint)special_shoot].time_between_bullets)
	{
		(this->*shot_function[(uint)special_shoot])();
		app->audio->PlayFx(basic_shot_sound);
		special_shot_timer.Start();
	}
	//- Basic shoot
	else if (!IsShootingSpecial() && IsShootingBasic() && basic_shot_timer.ReadMs() >= weapons_info[(uint)basic_shot].time_between_bullets)
	{
		(this->*shot_function[(uint)basic_shot])();
		app->audio->PlayFx(basic_shot_sound);
		basic_shot_timer.Start();
	}
}

bool Obj_Tank::IsShootingSpecial()
{
	if (shot_input == INPUT_METHOD::KEYBOARD_MOUSE)
	{
		return app->input->GetMouseButton(kb_shoot_special) == KEY_DOWN || app->input->GetMouseButton(kb_shoot_special) == KEY_REPEAT;
	}
	else if (shot_input == INPUT_METHOD::CONTROLLER)
	{
		return (*controller)->GetAxis(gamepad_shoot_special) > 0;
	}
}

bool Obj_Tank::IsShootingBasic()
{
	if (shot_input == INPUT_METHOD::KEYBOARD_MOUSE)
	{
		return app->input->GetMouseButton(kb_shoot_basic) == KEY_DOWN || app->input->GetMouseButton(kb_shoot_basic) == KEY_REPEAT;
	}
	else if (shot_input == INPUT_METHOD::CONTROLLER)
	{
		return (*controller)->GetAxis(gamepad_shoot_basic) > 0;
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
		&& app->input->GetMouseButton(kb_shoot_basic) != KEY_IDLE)
	{
		shot_input = INPUT_METHOD::KEYBOARD_MOUSE;
		SDL_ShowCursor(SDL_ENABLE);
	}
	if (shot_input != INPUT_METHOD::CONTROLLER
		&& (controller != nullptr
		&& (!(*controller)->GetJoystick(gamepad_aim).IsZero()
		|| (*controller)->GetAxis(gamepad_shoot_basic) > 0)))
	{
		shot_input = INPUT_METHOD::CONTROLLER;
		SDL_ShowCursor(SDL_DISABLE);
	}
}

void Obj_Tank::ShootBasic()
{
	Obj_Bullet * bullet = (Obj_Bullet*)app->objectmanager->CreateObject(ObjectType::BASIC_BULLET, turr_pos + shot_dir * cannon_length);
	bullet->SetBulletProperties(
		weapons_info[(uint)basic_shot].bullet_speed,
		weapons_info[(uint)basic_shot].bullet_life_ms,
		weapons_info[(uint)basic_shot].bullet_damage,
		shot_dir,
		turr_angle);
}

void Obj_Tank::ShootFlameThrower()
{
}


void Obj_Tank::ReviveTank()
{
	switch (tank_num) {
	case 0:
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_revive_tank) == KEY_DOWN && app->scene->tank_2->life == 0)		//TODO condicion collider
		{
			{
				app->scene->tank_2->speed = 4.f;
				app->scene->tank_2->life = 100;
			}
		}
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_revive_tank) == KEY_DOWN && app->scene->tank_3->life == 0)
		{
			{
				app->scene->tank_3->speed = 4.f;
				app->scene->tank_3->life = 100;
			}
		}
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_revive_tank) == KEY_DOWN && app->scene->tank_4->life == 0)
		{
			{
				app->scene->tank_4->speed = 4.f;
				app->scene->tank_4->life = 100;
			}
		}

		break;
	case 1:
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_revive_tank) == KEY_DOWN && app->scene->tank_1->life == 0)
		{
			{
				app->scene->tank_1->speed = 4.f;
				app->scene->tank_1->life = 100;
			}
		}
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_revive_tank) == KEY_DOWN && app->scene->tank_3->life == 0)
		{
			{
				app->scene->tank_3->speed = 4.f;
				app->scene->tank_3->life = 100;
			}
		}
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_revive_tank) == KEY_DOWN && app->scene->tank_4->life == 0)
		{
			{
				app->scene->tank_4->speed = 4.f;
				app->scene->tank_4->life = 100;
			}
		}
		break;
	case 2:	
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_revive_tank) == KEY_DOWN && app->scene->tank_1->life == 0)				
		{
			{
				app->scene->tank_1->speed = 4.f;
				app->scene->tank_1->life = 100;
			}
		}
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_revive_tank) == KEY_DOWN && app->scene->tank_2->life == 0)
		{
			{
				app->scene->tank_2->speed = 4.f;
				app->scene->tank_2->life = 100;
			}
		}
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_revive_tank) == KEY_DOWN && app->scene->tank_4->life == 0)
		{
			{
				app->scene->tank_4->speed = 4.f;
				app->scene->tank_4->life = 100;
			}
		}
		break;
	case 3:
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_revive_tank) == KEY_DOWN && app->scene->tank_1->life == 0)
		{
			{
				app->scene->tank_1->speed = 4.f;
				app->scene->tank_1->life = 100;
			}
		}
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_revive_tank) == KEY_DOWN && app->scene->tank_2->life == 0)
		{
			{
				app->scene->tank_2->speed = 4.f;
				app->scene->tank_2->life = 100;
			}
		}
		if (controller != nullptr && (*controller)->GetButtonState(gamepad_revive_tank) == KEY_DOWN && app->scene->tank_3->life == 0)
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

	if (app->input->GetKey(SDL_SCANCODE_J) == KeyState::KEY_DOWN || app->input->GetKey(SDL_SCANCODE_J) == KeyState::KEY_REPEAT)  //testing life=0
		app->scene->tank_1->life = 0;
	
	if (app->input->GetKey(SDL_SCANCODE_K) == KeyState::KEY_DOWN || app->input->GetKey(SDL_SCANCODE_K) == KeyState::KEY_REPEAT)
		app->scene->tank_2->life = 0;

	if (app->input->GetKey(SDL_SCANCODE_L) == KeyState::KEY_DOWN || app->input->GetKey(SDL_SCANCODE_L) == KeyState::KEY_REPEAT)
		app->scene->tank_3->life = 0;
	
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

	left_missile->SetBulletProperties(
		weapons_info[(uint)basic_shot].bullet_speed,
		weapons_info[(uint)basic_shot].bullet_life_ms,
		weapons_info[(uint)basic_shot].bullet_damage,
		shot_dir,
		turr_angle);

	right_missile->SetBulletProperties(
		weapons_info[(uint)basic_shot].bullet_speed,
		weapons_info[(uint)basic_shot].bullet_life_ms,
		weapons_info[(uint)basic_shot].bullet_damage,
		shot_dir,
		turr_angle);
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

