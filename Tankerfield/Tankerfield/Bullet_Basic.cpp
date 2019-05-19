#include "Bullet_Basic.h"
#include "App.h"
#include "M_AnimationBank.h"
#include "M_Collision.h"
#include "M_Textures.h"

Bullet_Basic::Bullet_Basic(fPoint pos) : Obj_Bullet(pos)
{

}

bool Bullet_Basic::Start()
{
	anim.frames = app->anim_bank->LoadFrames(app->config.child("object").child("basic_bullet").child("animations").child("rotate"));
	curr_anim = &anim;

	tex = app->tex->Load("textures/objects/bullets/basic_bullet.png");//app->config.child("object").child("basic_bullet").child("tex").attribute("path").as_string());
	curr_tex = tex;

	draw_offset = { (int)(35*0.5f), 7 };


	coll = app->collision->AddCollider(pos_map, .25f, .25f, TAG::BULLET, BODY_TYPE::DYNAMIC, 0.f, this);
	coll->SetObjOffset({ -0.25f*0.5f, -0.25f*0.5f });

	return true;
}
Bullet_Basic::~Bullet_Basic()
{
}

