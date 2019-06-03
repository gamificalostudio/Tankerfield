#include "Obj_OilPool.h"
#include "M_Collision.h"
#include "App.h"
#include "M_Textures.h"
#include "Animation.h"
#include "M_AnimationBank.h"

Obj_OilPool::Obj_OilPool(fPoint pos) : Obj_Bullet(pos)
{
}

Obj_OilPool::~Obj_OilPool()
{
}

bool Obj_OilPool::Update(float dt)
{
	if (time.ReadMs() >= 5000)
	{
		to_remove = true;
	}

	return true;
}

bool Obj_OilPool::Start()
{
	pugi::xml_node bullet_node = app->config.child("object").child("oil");

	anim.frames = app->anim_bank->LoadFrames(bullet_node.child("animations").child("rotate"));
	curr_anim = &anim;

	tex = app->tex->Load(bullet_node.child("tex").attribute("path").as_string());
	curr_tex = tex;

	draw_offset = { 150,0 };

	time.Start();
	if (coll)
	{
		coll->SetIsTrigger(true);
	}
	else
	{
		coll = app->collision->AddCollider(pos_map, 5, 5, TAG::OIL_POOL, BODY_TYPE::DYNAMIC);
		coll->is_sensor = true;
	}
	bullet_life_ms_timer.Start();
	return true;
}