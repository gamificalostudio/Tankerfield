#include <list>

//#include "Brofiler/Brofiler.h"

#include "App.h"
#include "Object.h"
#include "M_ObjManager.h"
#include "M_Render.h"
#include "M_Scene.h"
#include "M_Pathfinding.h"
#include "Log.h"
#include "M_Collision.h"
#include "M_Map.h"
#include "Obj_FlamethrowerFlame.h"
#include "Animation.h"
#include "M_AnimationBank.h"
#include "M_Audio.h"
#include "Obj_Tank.h"
#include "WeaponInfo.h"

Obj_FlamethrowerFlame::Obj_FlamethrowerFlame(fPoint pos) :Object(pos)
{
	pugi::xml_node explosion_node = app->config.child("object").child("explosion");

	anim.frames = app->anim_bank->LoadFrames(explosion_node.child("animations").child("explosion"));
	curr_anim = &anim;

	tex = app->tex->Load(explosion_node.child("tex").attribute("path").as_string());
	curr_tex = tex;

	draw_offset.x = 0;
	draw_offset.y = 0;

}

Obj_FlamethrowerFlame::~Obj_FlamethrowerFlame()
{
}

bool Obj_FlamethrowerFlame::Update(float dt)
{
	if (curr_anim != nullptr
		&& curr_anim->Finished())
	{
		to_remove = true;
	}
	if (frame_explosion == 2)
	{
		coll->to_destroy = true;
		coll = nullptr;
	}

	frame_explosion++;
	return true;
}

bool Obj_FlamethrowerFlame::Draw(float dt, Camera* camera) {
	app->render->Blit(tex,
		pos_screen.x - draw_offset.x,
		pos_screen.y - draw_offset.y,
		camera,
		NULL);
	return true;
}
