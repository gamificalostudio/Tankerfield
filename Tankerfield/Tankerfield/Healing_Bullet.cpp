#include "Healing_Bullet.h"
#include "App.h"
#include "M_Collision.h"
#include "Obj_Tank.h"
#include "M_Textures.h"

Healing_Bullet::Healing_Bullet(fPoint pos) : Obj_Bullet(pos)
{

}

Healing_Bullet::~Healing_Bullet()
{
}

bool Healing_Bullet::Start()
{
		pugi::xml_node bullet_node = app->config.child("object").child("basic_bullet");

		if (anim == nullptr)
		{
			anim = new Animation;
			anim->LoadAnimation(bullet_node.child("animations").child("rotate"));
		}
		curr_anim = anim;
		if (tex == nullptr)
		{
			tex = app->tex->Load(bullet_node.child("tex").attribute("path").as_string());
		}
		curr_tex = tex;
		if (draw_offset.IsZero())
		{
			draw_offset = { 35, 14 };
		}

		coll = app->collision->AddCollider(pos_map, .5f, .5f, Collider::TAG::FRIENDLY_BULLET, 0.f, this);

		return true;
}

