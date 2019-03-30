#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

#include "j1App.h"
#include "Obj_Tank.h"
#include "j1Textures.h"
#include "j1Render.h"

#include "j1Input.h"
#include "p2Log.h"
#include "UI_Test.h"

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
	pugi::xml_node tank_node = App->config.child("object").child("tank");

	Obj_Tank::base_tex = App->tex->Load(tank_node.child("spritesheets").child("base").text().as_string());
	Obj_Tank::base_shadow_tex = App->tex->Load(tank_node.child("spritesheets").child("base_shadow").text().as_string());
	Obj_Tank::turr_tex = App->tex->Load(tank_node.child("spritesheets").child("turr").text().as_string());
	Obj_Tank::turr_shadow_tex = App->tex->Load(tank_node.child("spritesheets").child("turr_shadow").text().as_string());

	LoadRects(tank_node.child("animations").child("rotate_base"), base_rects);

	cos_45 = cosf(-45 * DEGTORAD);
	sin_45 = sinf(-45 * DEGTORAD);

	return true;
}

bool Obj_Tank::PreUpdate()
{
	if (controller == nullptr)
	{
		controller = App->input->GetAbleController();
	}
	return true;
}

bool Obj_Tank::Update(float dt)
{
	if (controller != nullptr)
	{
		fPoint joystick = (fPoint)(*controller)->GetJoystick(Joystick::LEFT);
		//The tank has to go up in isometric space, so we need to rotate the input vector by 45 degrees
		fPoint iso_dir
			(joystick.x * cos_45 - joystick.y * sin_45,
			 joystick.x * sin_45 + joystick.y * cos_45);
		iso_dir.Normalize();
		pos += iso_dir * speed * dt;

		if(!joystick.IsZero())
		{
			angle = (atan2(joystick.y, -joystick.x) * RADTODEG) + 180;
		}
	}

	fPoint input (0,0);
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		input.x += sin_45;
		input.y += sin_45;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		input.x += sin_45;
		input.y -= sin_45;
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		input.x -= sin_45;
		input.y -= sin_45;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		input.x -= sin_45;
		input.y += sin_45;
	}
	input.Normalize();
	pos += input * speed * dt;

	return true;
}

bool Obj_Tank::PostUpdate()
{
	int tile_width = 100, tile_height = 50;
	uint ind = GetRotatedIndex(base_rects_num, angle, ROTATION_DIR::COUNTER_CLOCKWISE, 135);
	fPoint iso_pos = MapToWorldF(pos.x, pos.y, tile_width, tile_height);
	App->render->Blit(base_tex, iso_pos.x, iso_pos.y, &base_rects[ind]);
	return true;
}

bool Obj_Tank::CleanUp()
{
	return true;
}
