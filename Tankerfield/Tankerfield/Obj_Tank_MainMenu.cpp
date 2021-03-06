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
	if (controller != -1)
	{
		app->input->DetachController(controller);
	}
}

bool Obj_Tank_MainMenu::Start()
{
	pugi::xml_node tank_node = app->config.child("object").child("tank");

	// Textures ================================================

	// Base -------------------------
	base_color_tex = app->tex->Load(tank_node.child("spritesheets").child("base_color").text().as_string());
	base_common_tex = app->tex->Load(tank_node.child("spritesheets").child("base_common").text().as_string());
	base_shadow_tex = app->tex->Load(tank_node.child("spritesheets").child("base_shadow").text().as_string());

	SDL_SetTextureBlendMode(base_shadow_tex, SDL_BLENDMODE_MOD);

	// Turret ------------------------
	turret_color_tex = app->tex->Load(tank_node.child("spritesheets").child("turret_color").text().as_string());
	turret_common_tex = app->tex->Load(tank_node.child("spritesheets").child("turret_common").text().as_string());
	turret_shadow_tex = app->tex->Load(tank_node.child("spritesheets").child("turret_shadow").text().as_string());

	SDL_SetTextureBlendMode(turret_shadow_tex, SDL_BLENDMODE_MOD);

	// ==========================================================

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
	pos_screen = pos_map;
	return true;
}

void Obj_Tank_MainMenu::InputController(fPoint & input)
{
	if (controller != -1)
	{
		input = (fPoint)app->input->GetControllerJoystick(controller, gamepad_rotate, dead_zone);
	}
	else
	{
		input = { 0.f, 0.f };
	}
}


bool Obj_Tank_MainMenu::Draw(Camera * camera)
{

	// Base common =========================================

	SDL_Rect rect = curr_anim->GetFrame(angle);

	app->render->BlitScaled(
		base_common_tex,
		pos_map.x - draw_offset.x - rect.w * 0.5f * tank_scale,
		pos_map.y - draw_offset.y - rect.h * 0.5f * tank_scale,
		camera,
		&rect, tank_scale, tank_scale);

	// Base color =========================================

	SDL_SetTextureColorMod(base_color_tex, tank_color.r, tank_color.g, tank_color.b);

	app->render->BlitScaled(
		base_color_tex,
		pos_map.x - draw_offset.x - rect.w * 0.5f * tank_scale,
		pos_map.y - draw_offset.y - rect.h * 0.5f * tank_scale,
		camera,
		&rect, tank_scale, tank_scale);

	SDL_SetTextureColorMod(base_color_tex, 255, 255, 255);

	// Turret common ======================================

	rect = rotate_turr.GetFrame(turr_angle);

	app->render->BlitScaled(
		turret_common_tex,
		pos_map.x - draw_offset.x - 3.5f * tank_scale - rect.w * 0.5f * tank_scale,
		pos_map.y - draw_offset.y + 3.5f * tank_scale - rect.h * 0.5f * tank_scale,
		camera,
		&rect, tank_scale, tank_scale);

	// Turret color =======================================

	SDL_SetTextureColorMod(turret_color_tex, tank_color.r, tank_color.g, tank_color.b);

	app->render->BlitScaled(
		turret_color_tex,
		pos_map.x - draw_offset.x - 3.5f * tank_scale - rect.w * 0.5f * tank_scale,
		pos_map.y - draw_offset.y + 3.5f * tank_scale - rect.h * 0.5f * tank_scale,
		camera,
		&rect, tank_scale, tank_scale);

	SDL_SetTextureColorMod(turret_color_tex, 255, 255, 255);


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
		turret_shadow_tex,
		pos_map.x - draw_offset.x - 3.5f * tank_scale - rect.w * 0.5f * tank_scale,
		pos_map.y - draw_offset.y + 3.5f * tank_scale - rect.h * 0.5f * tank_scale,
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
		target_angle += 55.f * dt;
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

void Obj_Tank_MainMenu::SetColor(const SDL_Color new_color)
{
	tank_color = new_color;
}

void Obj_Tank_MainMenu::SetController(int controller)
{
	this->controller = controller;
}
