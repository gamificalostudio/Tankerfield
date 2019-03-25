#include "Obj_Tank.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

SDL_Texture * Obj_Tank::base_tex = nullptr;
SDL_Texture * Obj_Tank::turr_tex = nullptr;
SDL_Texture * Obj_Tank::base_shadow_tex = nullptr;
SDL_Texture * Obj_Tank::turr_shadow_tex = nullptr;

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

	//base_rects = new SDL_Rect[100];
	//LoadRects(tank_node.child("animations").child("rotate_base"), base_rects);

	return true;
}

bool Obj_Tank::PreUpdate()
{
	return true;
}

bool Obj_Tank::Update(float dt)
{
	return true;
}

bool Obj_Tank::PostUpdate()
{
	//SDL_Rect rect = GetRectFromAngle
	//App->render->Blit(base_tex, pos.x, pos.y, rects);
	return true;
}

bool Obj_Tank::CleanUp()
{
	return true;
}
