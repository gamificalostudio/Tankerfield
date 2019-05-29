#include "Item_HealthBag.h"
#include "Object.h"
#include "Obj_Tank.h"
#include "Obj_Item.h"
#include "M_Audio.h"
#include "App.h"
#include "M_Textures.h"
#include "M_AnimationBank.h"

Item_HealthBag::Item_HealthBag(fPoint pos) : Obj_Item(pos)
{
	health_bag = app->audio->LoadFx("audio/Fx/tank/health_bag.wav",50);
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
		caster->IncreaseLife(caster->GetMaxLife());
	}

	anim.frames = app->anim_bank->LoadFrames(health_bag_node.child("animations").child("rotate"));
	curr_anim = &anim;

	tex = app->tex->Load(health_bag_node.child("texture").attribute("path").as_string());
	curr_tex = tex;

	draw_offset = { 17, 65 };

	app->audio->PlayFx(health_bag);

	return true;
}