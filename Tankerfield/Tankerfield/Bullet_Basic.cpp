#include "Bullet_Basic.h"
#include "App.h"
#include "M_AnimationBank.h"
#include "M_Collision.h"
#include "M_Textures.h"
#include "M_Audio.h"

Bullet_Basic::Bullet_Basic(fPoint pos) : Obj_Bullet(pos)
{
	anim.frames = app->anim_bank->LoadFrames(app->config.child("object").child("basic_bullet").child("animations").child("rotate"));
	curr_anim = &anim;

	tex = app->tex->Load("textures/objects/bullets/basic_bullet.png");//app->config.child("object").child("basic_bullet").child("tex").attribute("path").as_string());
	curr_tex = tex;

	draw_offset = { (int)(35 * 0.5f), 7 };

	shot_sound = app->audio->LoadFx("audio/Fx/tank/weapons/basic_shot/machine_gun.wav", 10);
}

bool Bullet_Basic::Start()
{
	app->audio->PlayFx(shot_sound);
	if (coll == nullptr)
	{
		coll = app->collision->AddCollider(pos_map, .25f, .25f, TAG::BULLET, BODY_TYPE::DYNAMIC, 0.f, this);
		coll->SetObjOffset({ -0.25f*0.5f, -0.25f*0.5f });
	}
	else
	{
		coll->SetIsTrigger(true);
	}
	bullet_life_ms_timer.Start();
	return true;
}
Bullet_Basic::~Bullet_Basic()
{
}

