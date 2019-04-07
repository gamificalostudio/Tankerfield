#include "Obj_Bullet.h"
#include "M_Render.h"
#include "App.h"
#include "M_Map.h"
#include "Animation.h"
#include "M_Collision.h"

SDL_Texture * Obj_Bullet::tex = nullptr;
Animation * Obj_Bullet::anim = nullptr;

Obj_Bullet::Obj_Bullet(fPoint pos) : Object(pos)
{
	bullet_life_ms_timer.Start();
}

Obj_Bullet::~Obj_Bullet()
{
}

bool Obj_Bullet::Start()
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
		draw_offset = {35, 14};
	}

	coll = app->collision->AddCollider(pos_map, .5f, .5f, Collider::TAG::BULLET, this);

	return true;
}

bool Obj_Bullet::Update(float dt)
{
	pos_map += direction * speed * dt;
	
	coll->AddRigidBody(Collider::BODY_TYPE::SENSOR);

	if (bullet_life_ms_timer.ReadMs() >= bullet_life_ms)
	{
		to_remove = true;
	}
	return true;
}

void Obj_Bullet::OnTrigger(Collider * collider_1)
{
	to_remove = true;
}
