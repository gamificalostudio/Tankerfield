#include "Bullet_Laser.h"
#include "PerfTimer.h"
#include "App.h"
#include "M_Collision.h"
#include "M_ObjManager.h"
#include "Obj_Explosion.h"
#include "Obj_Tank.h"
#include "M_Textures.h"
#include "M_AnimationBank.h"
#include "M_Audio.h"

Laser_Bullet::Laser_Bullet(fPoint pos) : Obj_Bullet(pos)
{
	pugi::xml_node bullet_node = app->config.child("object").child("laser_bullet");

	anim.frames = app->anim_bank->LoadFrames(bullet_node.child("animations").child("rotate"));
	curr_anim = &anim;

	tex = app->tex->Load(bullet_node.child("tex").attribute("path").as_string());
	curr_tex = tex;
	draw_offset = { 35, 14 };

	kill_counter_max = bullet_node.child("kill_counter_max").attribute("value").as_int();

	sound = app->audio->LoadFx("audio/Fx/tank/weapons/tank_laser_shot.wav", 10);
}

Laser_Bullet::~Laser_Bullet()
{
}

bool Laser_Bullet::Start()
{
	
	if (coll)
	{
		coll->SetIsTrigger(true);
	}
	else
	{
		coll = app->collision->AddCollider(pos_map, .5f, .5f, TAG::BULLET_LASER, BODY_TYPE::DYNAMIC, 0.f, this);
		coll->is_sensor = true;

		coll->SetObjOffset({ -0.25f, -0.25f });
	}

	bullet_life_ms_timer.Start();

	app->audio->PlayFx(sound);
	
	return true;
}


