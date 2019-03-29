#include "Obj_Tank.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"
#include "j1Input.h"
#include "p2Log.h"

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
		//1. Get controller vector
		iPoint joystick = (*controller)->GetJoystick(Joystick::LEFT);
		//2. Rotate it (45 degrees clockwise) -> we don't want it to move in the "isometric space"
		float angle_cos = cosf(45);//TODO: Create a macro (value doesn't change, so it's useless to recalculate it every frame)
		float angle_sin = sinf(45);
		iPoint iso_dir;
		iso_dir.x = joystick.x * angle_cos - joystick.y * angle_sin;
		iso_dir.y = joystick.x * angle_sin + joystick.y * angle_cos;
		//3. Normalize it
		LOG("X: %i, Y: %i", joystick.x, joystick.y);
	}

	//4. Apply it to the position

	//if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	//{
	//	position
	//}
	return true;
}

bool Obj_Tank::PostUpdate()
{
	uint ind = GetRotatedIndex(base_rects_num, angle, ROTATION_DIR::COUNTER_CLOCKWISE, 135);
	App->render->Blit(base_tex, pos.x, pos.y, &base_rects[ind]);
	return true;
}

bool Obj_Tank::CleanUp()
{
	return true;
}
