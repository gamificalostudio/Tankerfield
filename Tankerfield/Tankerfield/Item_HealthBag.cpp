#include "Item_HealthBag.h"
#include "Object.h"
#include "Obj_Tank.h"
#include "Obj_Item.h"
#include "App.h"
#include "M_Textures.h"

SDL_Texture * Item_HealthBag::tex = nullptr;
Animation * Item_HealthBag::anim = nullptr;

Item_HealthBag::Item_HealthBag(fPoint pos) : Obj_Item(pos)
{
}

bool Item_HealthBag::Update(float dt)
{
	if (caster != nullptr)
	{
		pos_map = caster->pos_map;
	}

	if (curr_anim != nullptr && curr_anim->Finished())
	{
		to_remove = true;
	}

	return true;
}

bool Item_HealthBag::Use()
{
	pugi::xml_node health_bag_node = app->config.child("items").child("health_bag");

	if (caster != nullptr)
	{
		caster->SetLife(caster->GetMaxLife());
	}

	if(anim == nullptr)
	{
		anim = new Animation(health_bag_node.child("animations").child("rotate"));
	}
	curr_anim = anim;

	if (tex == nullptr)
	{
		tex = app->tex->Load(health_bag_node.child("texture").attribute("path").as_string());
	}
	curr_tex = tex;

	return true;
}