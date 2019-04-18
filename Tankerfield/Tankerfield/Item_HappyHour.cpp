#include "Item_HappyHour.h"
#include "Object.h"
#include "Obj_Tank.h"
#include "Obj_Item.h"
#include "App.h"
#include "M_Textures.h"
#include "M_AnimationBank.h"

Item_HappyHour::Item_HappyHour(fPoint pos) : Obj_Item(pos)
{
}

bool Item_HappyHour::Update(float dt)
{
	if (caster != nullptr)
	{
		pos_map = caster->pos_map;
	}

	if (time.ReadMs()>=5000)
	{
		caster->SetTimeBetweenBullets(original_time_between_bullets);
		to_remove = true;
	}


	return true;
}

bool Item_HappyHour::Use()
{
	pugi::xml_node happy_hour_node = app->config.child("items").child("happy_hour");

	time.Start();
	if (caster != nullptr)
	{
		original_time_between_bullets=caster->GetTimeBetweenBullets();
		happy_time_between_bullets=caster->GetTimeBetweenBullets() / 4;
		caster->SetTimeBetweenBullets(happy_time_between_bullets);
	}

	anim.frames = app->anim_bank->LoadFrames(happy_hour_node.child("animations").child("rotate"));
	curr_anim = &anim;

	tex = app->tex->Load(happy_hour_node.child("texture").attribute("path").as_string());
	curr_tex = tex;

	draw_offset = { 17, 65 };

	return true;
}
