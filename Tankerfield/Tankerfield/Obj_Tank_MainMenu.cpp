#include <math.h>

#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

#include "app.h"
#include "Obj_Tank_MainMenu.h"

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

#include "MathUtils.h"
#include "M_AnimationBank.h"
#include "M_ObjManager.h"
#include "Camera.h"


Obj_Tank_MainMenu::Obj_Tank_MainMenu(fPoint pos) : Object(pos)
{

}

Obj_Tank_MainMenu::~Obj_Tank_MainMenu()
{
	if (controller != nullptr)
	{
		(*controller)->DetachController();
	}
}

bool Obj_Tank_MainMenu::Start()
{
	pugi::xml_node tank_node = app->config.child("object").child("tank");

	//Textures-------------------------------------------------
	//-- Base
	base_tex_blue = app->tex->Load(tank_node.child("spritesheets").child("base_blue").text().as_string());
	base_shadow_tex = app->tex->Load(tank_node.child("spritesheets").child("base_shadow").text().as_string());
	SDL_SetTextureBlendMode(base_shadow_tex, SDL_BLENDMODE_MOD);

	//-- Turr
	std::string aux = tank_node.child("spritesheets").child("turr_orange").text().as_string();
	turr_tex_blue = app->tex->Load(tank_node.child("spritesheets").child("turr_blue").text().as_string());
	turr_shadow_tex = app->tex->Load(tank_node.child("spritesheets").child("turr_shadow").text().as_string());
	SDL_SetTextureBlendMode(turr_shadow_tex, SDL_BLENDMODE_MOD);

	curr_tex = base_tex_blue;
	turr_tex = turr_tex_blue;

	rotate_base.frames = app->anim_bank->LoadFrames(tank_node.child("animations").child("rotate_base"));
	curr_anim = &rotate_base;

	rotate_turr.frames = app->anim_bank->LoadFrames(tank_node.child("animations").child("rotate_turr"));

	cos_45 = cosf(-45 * DEGTORAD);
	sin_45 = sinf(-45 * DEGTORAD);

	gamepad_rotate = Joystick::RIGHT;

	draw_offset.x = -40  ;
	draw_offset.y = 60  ;

	base_angle_lerp_factor = 11.25f;
	turr_angle_lerp_factor = 11.25f;

	dead_zone = 6000;
	tank_scale = 3.5f;
	return true;
}

bool Obj_Tank_MainMenu::Update(float dt)
{
	Rotate(dt);
	return true;
}

void Obj_Tank_MainMenu::InputController(fPoint & input)
{
	if (controller != nullptr)
	{
		input = (fPoint)(*controller)->GetJoystick(gamepad_rotate, dead_zone);
	}
	else
	{
		input = { 0.f, 0.f };
	}
}


bool Obj_Tank_MainMenu::Draw(float dt, Camera * camera)
{
	// Base =========================================
	SDL_Rect rect = curr_anim->GetFrame(angle);

	app->render->BlitScaled(
		curr_tex,
		pos_map.x - draw_offset.x - rect.w * 0.5f * tank_scale,
		pos_map.y - draw_offset.y - rect.h * 0.5f * tank_scale,
		camera,
		&rect, tank_scale, tank_scale);

	// Turret =======================================
	rect = rotate_turr.GetFrame(turr_angle);

	app->render->BlitScaled(
		turr_tex,
		pos_map.x - draw_offset.x - 1.66f* tank_scale - rect.w * 0.5f * tank_scale,
		pos_map.y - draw_offset.y + 1.66f * tank_scale - rect.h * 0.5f * tank_scale,
		camera,
		&rect, tank_scale, tank_scale);

	//app->render->DrawQuadUI({ (int)pos_map.x - 3,  (int)pos_map.y - 3, 6, 6 }, camera, { 0, 255, 0, 255 });
	return true;
}

bool Obj_Tank_MainMenu::DrawShadow(Camera * camera, float dt)
{
	// Base =========================================
	SDL_Rect rect = curr_anim->GetFrame(angle);

	app->render->BlitScaled(
		base_shadow_tex,
		pos_map.x - draw_offset.x - rect.w * 0.5f * tank_scale,
		pos_map.y - draw_offset.y - rect.h * 0.5f * tank_scale,
		camera,
		&rect, tank_scale, tank_scale);

	// Turret =======================================
	rect = rotate_turr.GetFrame(turr_angle);

	app->render->BlitScaled(
		turr_shadow_tex,
		pos_map.x - draw_offset.x - 1.66f * tank_scale - rect.w * 0.5f * tank_scale,
		pos_map.y - draw_offset.y + 1.66f * tank_scale - rect.h * 0.5f * tank_scale,
		camera,
		&rect, tank_scale, tank_scale);

	return true;
}

bool Obj_Tank_MainMenu::CleanUp()
{
	return true;
}

void Obj_Tank_MainMenu::Rotate(float dt)
{

	fPoint input_dir(0.f, 0.f);

	InputController(input_dir);


	if (input_dir.IsZero() == false)
	{
		target_angle = atan2(input_dir.y, -input_dir.x) * RADTODEG;
		float turns = floor(angle / 360.f);
		target_angle += 360.f * turns;
		if (abs((target_angle + 360.f) - angle) < abs(target_angle - angle))
		{
			target_angle += 360.f;
		}
	}
	else
	{
		target_angle += 40.f * dt;
		if (abs((target_angle + 360.f) - angle) < abs(target_angle - angle))
		{
			target_angle += 360.f;
		}
	}



	angle = lerp(angle, target_angle, base_angle_lerp_factor * dt);

	int aux = angle + 180;
	if (angle > 360)
	{
		aux = aux % 360;
	}

	turr_angle = aux;

}

void Obj_Tank_MainMenu::SetController(Controller** controller)
{
	this->controller = controller;
}
