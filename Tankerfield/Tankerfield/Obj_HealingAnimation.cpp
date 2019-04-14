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
SDL_Texture * Obj_Healing_Animation::tex = nullptr;

Obj_Healing_Animation::Obj_Healing_Animation(fPoint pos)
{

		pugi::xml_node explosion_node = app->config.child("object").child("explosion");

		if (tex == nullptr)
		{
			tex = app->tex->Load(explosion_node.child("tex").attribute("path").as_string());
		}
		if (anim == nullptr)
		{
			anim = new Animation(explosion_node.child("animations").child("explosion"));
		}
		curr_tex = tex;
		curr_anim = anim;
		draw_offset.x = 99;
		draw_offset.y = 10;

}

Obj_Healing_Animation::~Obj_Healing_Animation()
{
}

bool Obj_Healing_Animation::Update(float dt)
{
	if (curr_anim != nullptr&&curr_anim->Finished())
	{
		to_remove = true;
	}

	return true;
}
