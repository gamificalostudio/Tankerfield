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
#include "Obj_ElectroShotAnimation.h"
#include "Animation.h"
#include "M_AnimationBank.h"
#include "M_Audio.h"
#include "Obj_Tank.h"
#include "WeaponInfo.h"

Obj_FlamethrowerFlame::Obj_FlamethrowerFlame(fPoint pos) : Object(pos)
{
	pugi::xml_node flamethrower_node = app->config.child("object").child("tank").child("flamethrower");
	pugi::xml_node anim_flame_node = app->anim_bank->animations_xml_node.child("flamethrower");

	tex = app->tex->Load(flamethrower_node.child("tex_flamethrower").text().as_string());
	curr_tex = tex;

	fire_start.frames = app->anim_bank->LoadFrames(anim_flame_node.child("fire_start"));
	fire.frames = app->anim_bank->LoadFrames(anim_flame_node.child("fire"));
	fire_reverse.frames = app->anim_bank->LoadFrames(anim_flame_node.child("fire_reverse"));
	fire_end.frames = app->anim_bank->LoadFrames(anim_flame_node.child("fire_end"));
	
	curr_anim = nullptr;

	
	draw_offset = { 77, 0 };
	
	pivot.x = draw_offset.x;
	pivot.y = draw_offset.y;

	scale = 1.f;
}

Obj_FlamethrowerFlame::~Obj_FlamethrowerFlame()
{
}

bool Obj_FlamethrowerFlame::Update(float dt)
{
	if (is_holding == true && curr_anim == nullptr)
	{
		curr_anim = &fire_start;
	}
	

	if (fire_start.Finished())
	{
		fire_start.Reset();
		curr_anim = &fire;
	}


	if (fire.Finished())
	{
		if (is_holding)
		{
			fire.Reset();
			curr_anim = &fire_reverse;
		}
		else
		{
			fire.Reset();
			curr_anim = &fire_end;
		}
	}

	if (fire_reverse.Finished())
	{
		fire_reverse.Reset();
		curr_anim = &fire;
	}

	if (fire_end.Finished())
	{
		fire_end.Reset();
		curr_anim = nullptr;
	}

	pos_map = tank->GetTurrPos();

	return true;
}

bool Obj_FlamethrowerFlame::Draw(float dt, Camera* camera)
{
	if (curr_anim != nullptr) {
		app->render->BlitScaledAndRotated(
			tex,
			pos_screen.x - draw_offset.x,
			pos_screen.y - draw_offset.y,
			camera,
			&curr_anim->GetFrame(0),
			scale,
			scale,
			pivot,
			-tank->GetTurrAngle() - 90.f);
	}

	return true;
}

