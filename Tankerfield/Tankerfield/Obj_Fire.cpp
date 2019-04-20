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
#include "Obj_Fire.h"
#include "Animation.h"
#include "M_AnimationBank.h"
#include "M_Audio.h"
#include "Obj_Tank.h"

Obj_Fire::Obj_Fire(fPoint pos) :Object(pos)
{
	pugi::xml_node fire_node = app->config.child("object").child("fire_dead_tank");

	anim.frames = app->anim_bank->LoadFrames(fire_node.child("animations").child("fire"));
	curr_anim = &anim;

	tex = app->tex->Load(fire_node.child("tex").attribute("path").as_string());
	curr_tex = tex;

	draw_offset = { 33, 70 };
}

Obj_Fire::~Obj_Fire()
{
}

bool Obj_Fire::Update(float dt)
{
	if (tank != nullptr)
	{
		pos_map = tank->pos_map;
	}

	if (tank->GetLife() > 0)
	{
		to_remove = true;
	}

	return true;
}