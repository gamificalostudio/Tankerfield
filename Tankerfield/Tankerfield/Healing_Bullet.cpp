#include "Healing_Bullet.h"
#include "App.h"
#include "M_Collision.h"
#include "Obj_Tank.h"
#include "M_Textures.h"
#include "M_AnimationBank.h"

Healing_Bullet::Healing_Bullet(fPoint pos) : Obj_Bullet(pos)
{

}

Healing_Bullet::~Healing_Bullet()
{
}

bool Healing_Bullet::Start()
{
		anim.frames=app->anim_bank->LoadFrames(app->config.child("object").child("bullet").child("animations").child("rotate"));
		curr_anim = &anim;

		tex = app->tex->Load(app->config.child("object").child("healing_bullet").child("tex").attribute("path").as_string());
		curr_tex = tex;

		draw_offset = { 35, 14 };

		coll = app->collision->AddCollider(pos_map, .5f, .5f, TAG::FRIENDLY_BULLET, BODY_TYPE::DYNAMIC, 0.f, this);
		coll->SetObjOffset({-0.25f, -0.25f});

		return true;
}

void Healing_Bullet::OnTriggerEnter(Collider * collider, float dt)
{
	if (collider->GetObj() != player)
	{
		to_remove = true;
	}
}
