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

	tex = app->tex->Load(flamethrower_node.child("tex_flamethrower").text().as_string());
	curr_tex = tex;

	fire_start.frames = app->anim_bank->LoadFrames(flamethrower_node.child("animations").child("fire1"));
	fire.frames = app->anim_bank->LoadFrames(flamethrower_node.child("animations").child("fire2"));
	fire_end.frames = app->anim_bank->LoadFrames(flamethrower_node.child("animations").child("fire3"));
	
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
		if (is_holding == true)
		{	
			fire_start.Reset();
		}
		else {
			fire.Reset();
			curr_anim = &fire_end;
		}
	}
	if (fire_end.Finished())
	{
		fire_end.Reset();
		curr_anim = nullptr;
	}
	pos_map = tank->pos_map;

	return true;
}

bool Obj_FlamethrowerFlame::Draw(float dt, Camera* camera)
{
	is_holding = false;

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

