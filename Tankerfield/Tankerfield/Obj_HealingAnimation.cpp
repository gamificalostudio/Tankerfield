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
#include "Obj_HealingAnimation.h"
#include "Animation.h"

Animation*  Obj_Healing_Animation::anim = nullptr;

Obj_Healing_Animation::Obj_Healing_Animation(fPoint pos)
{
	pugi::xml_node health_bag_node = app->config.child("items").child("health_bag");

	if (anim == nullptr)
	{
		anim = new Animation(health_bag_node.child("animations").child("rotate"));
	}
	curr_anim = anim;

	if (tex == nullptr)
	{
		tex = app->tex->Load(health_bag_node.child("texture").attribute("path").as_string());
	}
	curr_tex = tex;

	draw_offset = { 17, 65 };

}

Obj_Healing_Animation::~Obj_Healing_Animation()
{
}

bool Obj_Healing_Animation::Update(float dt)
{
	/*if (tank != nullptr)
	{
		pos_map = tank->pos_map;
	}*/
	
	if (curr_anim != nullptr&&curr_anim->Finished())
	{
		to_remove = true;
	}

	return true;
}
