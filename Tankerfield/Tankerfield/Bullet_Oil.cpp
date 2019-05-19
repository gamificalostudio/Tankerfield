#include "Bullet_Oil.h"
#include "App.h"
#include "M_Collision.h"
#include "M_AnimationBank.h"
#include "M_Textures.h"

Bullet_Oil::Bullet_Oil(fPoint pos) : Obj_Bullet(pos)
{
}

Bullet_Oil::~Bullet_Oil()
{
}

bool Bullet_Oil::Start()
{
	pugi::xml_node bullet_node = app->config.child("object").child("oil_bullet");

	anim.frames = app->anim_bank->LoadFrames(bullet_node.child("animations").child("rotate"));
	curr_anim = &anim;

	tex = app->tex->Load(bullet_node.child("tex").attribute("path").as_string());
	curr_tex = tex;

	draw_offset = { 35, 14 };

	coll = app->collision->AddCollider(pos_map, .5f, .5f, TAG::BULLET_OIL, BODY_TYPE::DYNAMIC, 0.f, this);
	coll->SetObjOffset({ -0.25f, -0.25f });
	coll->is_sensor = true;
	return true;
}
