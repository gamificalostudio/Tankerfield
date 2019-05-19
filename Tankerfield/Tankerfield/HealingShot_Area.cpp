#include "HealingShot_Area.h"
#include "WeaponInfo.h"
#include "App.h"
#include "Object.h"
#include "M_Collision.h"
#include "Obj_Bullet.h"
#include "M_Textures.h"
#include "Animation.h"
#include "M_AnimationBank.h"
#include "Object.h"

HealingShot_Area::HealingShot_Area(fPoint pos) : Obj_Bullet(pos)
{
}

HealingShot_Area::~HealingShot_Area()
{
}

bool HealingShot_Area::Start()
{
	pugi::xml_node area_node = app->config.child("object").child("charged_healing");

	anim.frames = app->anim_bank->LoadFrames(area_node.child("animations").child("anim"));
	curr_anim = &anim;

	tex = app->tex->Load(area_node.child("tex").attribute("path").as_string());
	curr_tex = tex;

	draw_offset = { 75, 110 };

	coll = app->collision->AddCollider(pos_map, 2.5f, 2.5f, TAG::HEALING_AREA_SHOT, BODY_TYPE::DYNAMIC, 0.f, this);
	coll->SetObjOffset({-1.25f,-1.25f});
	coll->is_sensor = true;

	return true;
}


bool HealingShot_Area::Update(float dt)
{
	if (anim.Finished())
	{
		to_remove = true;
	}

	return true;
}
