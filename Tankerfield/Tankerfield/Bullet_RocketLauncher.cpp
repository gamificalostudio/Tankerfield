#include "Bullet_RocketLauncher.h"
#include "PerfTimer.h"
#include "App.h"
#include "M_Collision.h"
#include "M_ObjManager.h"
#include "Obj_Explosion.h"
#include "Obj_Tank.h"
#include "M_Textures.h"
#include "M_AnimationBank.h"

Bullet_RocketLauncher::Bullet_RocketLauncher(fPoint pos) : Obj_Bullet(pos)
{
}

Bullet_RocketLauncher::~Bullet_RocketLauncher()
{
}

bool Bullet_RocketLauncher::Start()
{
	pugi::xml_node bullet_node = app->config.child("object").child("basic_bullet_rocketlauncher");

	anim.frames = app->anim_bank->LoadFrames(bullet_node.child("animations").child("rotate"));
	curr_anim = &anim;

	tex = app->tex->Load(bullet_node.child("tex").attribute("path").as_string());
	curr_tex = tex;

	draw_offset = { 35, 14 };
	if (coll)
	{
		coll->SetIsTrigger(true);
	}
	else
	{
		coll = app->collision->AddCollider(pos_map, .5f, .5f, TAG::BULLET_ENEMY, BODY_TYPE::DYNAMIC, 0.f, this);
		coll->is_sensor = true;
		coll->SetObjOffset({ -0.25f, -0.25f });
	}

	attack_damage = app->objectmanager->rocket_launcher_info.attack_damage;
	bullet_life_ms_timer.Start();
	return true;
}
