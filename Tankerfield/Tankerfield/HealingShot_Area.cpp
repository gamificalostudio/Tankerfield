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
	pugi::xml_node bullet_node = app->config.child("object").child("basic_bullet");

	anim.frames = app->anim_bank->LoadFrames(bullet_node.child("animations").child("rotate"));
	curr_anim = &anim;

	tex = app->tex->Load(bullet_node.child("tex").attribute("path").as_string());
	curr_tex = tex;

	draw_offset = { 35, 14 };

	coll = app->collision->AddCollider(pos_map, 3, 3, TAG::HEALING_AREA_SHOT, BODY_TYPE::DYNAMIC, 0.f, this);
	coll->is_sensor = true;

	time.Start();

	return true;
}


bool HealingShot_Area::Update(float dt)
{
	if (time.ReadMs() >= 3000)
	{
		to_remove = true;
	}

	return true;
}
