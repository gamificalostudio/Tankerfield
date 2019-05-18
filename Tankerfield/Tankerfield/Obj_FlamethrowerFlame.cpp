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
#include "ElectroShotAnimation.h"
#include "Animation.h"
#include "M_AnimationBank.h"
#include "M_Audio.h"
#include "Obj_Tank.h"
#include "WeaponInfo.h"

Obj_FlamethrowerFlame::Obj_FlamethrowerFlame(fPoint pos) :Object(pos)
{
	pugi::xml_node flamethrower_node = app->config.child("object").child("tank").child("flamethrower").child("flamethrower_animation");;

	tex = app->tex->Load(flamethrower_node.child("tex_flamethrower").attribute("path").as_string());
	curr_tex = tex;

	anim.frames = app->anim_bank->LoadFrames(flamethrower_node.child("animations").child("anim_flamethrower1"));
	curr_anim = &anim;

	draw_offset = { 0,0 };
	draw_offset.x = curr_anim->GetFrame(0).w * 0.5f;
	//	electro_offset.y = 30;

}

Obj_FlamethrowerFlame::~Obj_FlamethrowerFlame()
{
}

bool Obj_FlamethrowerFlame::Update(float dt)
{
	/*if (curr_anim != nullptr
		&& curr_anim->Finished())
	{
		to_remove = true;
	}*/
	/*if (frame_explosion == 2)
	{
		coll->to_destroy = true;
		coll = nullptr;
	}*/

	pos_map = tank->pos_map;
	pos_screen = tank->pos_screen;
	if (!hit_no_enemie)
	{
		player_enemy_distance_point = app->map->MapToScreenF(enemy_pos_map - pos_map);
		distance = pos_screen.DistanceTo(enemy_pos_screen);
	}
	else
	{
		distance = 25.f;
		player_enemy_distance_point = app->map->MapToScreenF(tank->GetShotDir());
	}
	if (!curr_anim->Finished())
	{
		curr_anim->NextFrame(dt);
	}
	else
	{
		curr_anim->Reset();
		to_remove = true;
	}

	//frame_explosion++;
	return true;
}

bool Obj_FlamethrowerFlame::Draw(float dt, Camera* camera) {

	app->render->BlitScaledAndRotated(
		tex,
		pos_screen.x - draw_offset.x,
		pos_screen.y - draw_offset.y,
		camera,
		&curr_anim->GetFrame(0),
		1,
		distance / curr_anim->GetFrame(0).h,
		SDL_Point{ (int)(curr_anim->GetFrame(0).w * 0.5f) , 0 },
		atan2(player_enemy_distance_point.y, player_enemy_distance_point.x) * RADTODEG - 2 * ISO_COMPENSATION
	);

	return true;
}

