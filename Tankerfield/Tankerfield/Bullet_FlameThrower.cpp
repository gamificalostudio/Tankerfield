#include "Bullet_FlameThrower.h"
#include "PerfTimer.h"
#include "App.h"
#include "M_Collision.h"
#include "M_ObjManager.h"
#include "Obj_Explosion.h"
#include "Obj_Tank.h"
#include "M_Textures.h"
#include "M_AnimationBank.h"

FlameThrower_Bullet::FlameThrower_Bullet(fPoint pos) : Obj_Bullet(pos)
{
}

FlameThrower_Bullet::~FlameThrower_Bullet()
{
}

bool FlameThrower_Bullet::Start()
{
	pugi::xml_node bullet_node = app->config.child("object").child("laser_bullet");

	anim.frames = app->anim_bank->LoadFrames(bullet_node.child("animations").child("rotate"));
	curr_anim = &anim;

	tex = app->tex->Load(bullet_node.child("tex").attribute("path").as_string());
	curr_tex = tex;

	draw_offset = { 35, 14 };

	coll = app->collision->AddCollider(pos_map, .5f, .5f, Collider::TAG::BULLET_FLAMETHROWER, 0.f, this);
	coll->SetObjOffset({ -0.25f, -0.25f });

	return true;
}


