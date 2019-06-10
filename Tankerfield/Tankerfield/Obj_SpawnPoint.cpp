#include "Obj_SpawnPoint.h"
#include "App.h"
#include "M_Textures.h"
#include "M_AnimationBank.h"
Obj_SpawnPoint::Obj_SpawnPoint(fPoint pos):pos(pos), Object(pos)
{
	tex_portal = app->tex->Load("textures/Objects/enemies/portal.png");
	portal.frames = app->anim_bank->LoadFrames(app->anim_bank->animations_xml_node.child("portal").child("animations").child("idle"));
	curr_anim = &portal;
	curr_tex = tex_portal;
}

bool Obj_SpawnPoint::Start()
{
	show_portal_timer.Start();
	return true;
}

bool Obj_SpawnPoint::Update(float dt)
{
	if (show_portal_timer.ReadSec() >= 20)
	{
		active = false;
	}
	return true;
}
