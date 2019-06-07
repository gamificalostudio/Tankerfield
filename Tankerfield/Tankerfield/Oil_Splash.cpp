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
#include "Oil_Splash.h"
#include "Animation.h"
#include "M_AnimationBank.h"
#include "M_Audio.h"
#include "Obj_Tank.h"
#include "WeaponInfo.h"

Oil_Splash::Oil_Splash(fPoint pos) :Object(pos)
{
	pugi::xml_node explosion_node = app->config.child("object").child("oil_splash");

	anim.frames = app->anim_bank->LoadFrames(explosion_node.child("animations").child("oil_splash"));
	curr_anim = &anim;

	tex = app->tex->Load(explosion_node.child("tex").attribute("path").as_string());
	curr_tex = tex;

	draw_offset.x = 45;
	draw_offset.y = 48;

	frame_damage = 0;
}

bool Oil_Splash::Update(float dt)
{
	//Create a collider during one frame which deals damage equivalent to int explosion_damage
	if (frame_num == frame_damage)
	{
		float coll_w = 1.5f;
		float coll_h = 1.5f;

		coll = app->collision->AddCollider(
			pos_map - fPoint(coll_w * 0.5f, coll_h * 0.5f),
			coll_w,
			coll_h,
			TAG::BULLET_OIL,
			BODY_TYPE::DYNAMIC,
			splash_damage,
			nullptr);

		coll->is_sensor = true;
		coll->SetObjOffset(fPoint(coll_w*0.5f, coll_h*0.5f));

	}
	else if (coll != nullptr && frame_num == frame_damage + 1)
	{
		coll->Destroy();
		coll = nullptr;
	}

	frame_num++;

	//Delete this object when the animation has finished
	if (curr_anim != nullptr
		&& curr_anim->Finished())
	{
		to_remove = true;
	}
	return true;
}

void Oil_Splash::SetSplashDamage(float damage)
{
	splash_damage = damage;
}