#include "Obj_CannonFire.h"
#include "M_AnimationBank.h"
#include "App.h"
#include "M_Textures.h"

Obj_CannonFire::Obj_CannonFire(fPoint pos_map) : Object(pos_map)
{
	pugi::xml_node cannon_fire_node = app->config.child("object").child("cannon_fire");

	anim.frames = app->anim_bank->LoadFrames(cannon_fire_node.child("animations").child("cannon_fire"));
	curr_anim = &anim;

	tex = app->tex->Load("textures/objects/cannon_fire.png");
	curr_tex = tex;

	draw_offset.x = 74;
	draw_offset.y = 85;
}

bool Obj_CannonFire::Update(float dt)
{
	if (curr_anim != nullptr
		&& curr_anim->Finished())
	{
		to_remove = true;
	}

	return true;
}
