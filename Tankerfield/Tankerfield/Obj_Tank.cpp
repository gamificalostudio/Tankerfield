#include "Obj_Tank.h"
#include "j1App.h"
#include "j1Textures.h"

Obj_Tank::Obj_Tank()
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
	pugi::xml_node tank_node = App->config.child("object");
	base_tex = App->tex->Load(App->config.child("object").child("spritesheets").child("tank_base").text().as_string());
	LoadRects(App->config->child("object"));
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
	App->render->Blit();
	return true;
}

bool Obj_Tank::CleanUp()
{
	return true;
}
