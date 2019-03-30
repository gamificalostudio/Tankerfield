#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

#include "App.h"
#include "Obj_Tank.h"
#include "M_Textures.h"
#include "M_Render.h"

#include "M_Input.h"
#include "p2Log.h"
#include "M_UITest.h"


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
	Movement(dt);

	return true;
}

void Obj_Tank::Movement(float dt)
{
	fPoint input(0.f, 0.f);
	GetKeyboardInput(input);
	GetControllerInput(input);

	fPoint dir(0.f, 0.f);
	//The tank has to go up in isometric space, so we need to rotate the input vector by 45 degrees
	dir.x = input.x * cos_45 - input.y * sin_45;
	dir.y = input.x * sin_45 + input.y * cos_45;
	dir.Normalize();

	if (!dir.IsZero())
	{
		angle = (atan2(input.y, -input.x) * RADTODEG);
	}

	pos += dir * speed * dt;
}

void Obj_Tank::GetKeyboardInput(fPoint & input)
{
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		input.y -= 1.f;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		input.x -= 1.f;
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		input.y += 1.f;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		input.x += 1.f;
	}
}

void Obj_Tank::GetControllerInput(fPoint & input)
{
	if (controller != nullptr)
	{
		input += (fPoint)(*controller)->GetJoystick(Joystick::LEFT);
	}
}

bool Obj_Tank::PostUpdate()
{
	int tile_width = 100, tile_height = 50;
	uint ind = GetRotatedIndex(base_rects_num, angle, ROTATION_DIR::COUNTER_CLOCKWISE, 315);
	fPoint iso_pos = MapToWorldF(pos.x, pos.y, tile_width, tile_height);
	App->render->Blit(base_tex, iso_pos.x, iso_pos.y, &base_rects[ind]);
	return true;
}

bool Obj_Tank::CleanUp()
{
	return true;
}
