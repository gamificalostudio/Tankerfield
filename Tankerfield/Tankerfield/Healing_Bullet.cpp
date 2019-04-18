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
		pugi::xml_node bullet_node = app->config.child("object").child("basic_bullet");

		anim.frames=app->anim_bank->LoadFrames(bullet_node.child("animations").child("rotate"));
		curr_anim = &anim;

		tex = app->tex->Load(bullet_node.child("tex").attribute("path").as_string());
		curr_tex = tex;

		draw_offset = { 35, 14 };

		coll = app->collision->AddCollider(pos_map, .5f, .5f, Collider::TAG::FRIENDLY_BULLET, 0.f, this);
		coll->SetObjOffset({-0.25f, -0.25f});

		return true;
}
