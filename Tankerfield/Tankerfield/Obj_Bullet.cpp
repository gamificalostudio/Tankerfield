#include "Obj_Bullet.h"
#include "M_Render.h"
#include "App.h"
#include "M_Map.h"
#include "M_Collision.h"
#include "M_AnimationBank.h"
#include "Obj_Tank.h"

Obj_Bullet::Obj_Bullet(fPoint pos) : Object(pos)
{
	pugi::xml_node bullet_node = app->config.child("object").child("bullet");
	anim.frames = app->anim_bank->LoadFrames(bullet_node.child("animations").child("rotate"));
	tex = app->tex->Load(bullet_node.child("tex").attribute("path").as_string());
	draw_offset = { 35, 14 };
}

Obj_Bullet::~Obj_Bullet()
{
}

bool Obj_Bullet::Start()
{
	curr_anim = &anim;
	curr_tex = tex;
	

	if (coll == nullptr)
	{
		float coll_w = 0.5f;
		float coll_h = 0.5f;
		coll = app->collision->AddCollider(pos_map, coll_w, coll_h, TAG::BULLET, BODY_TYPE::DYNAMIC, 0.f, this);
		coll->SetObjOffset({ -coll_w * 0.5f, -coll_h * 0.5f });
		coll->is_sensor = true;
	}
	else
	{
		coll->SetIsTrigger(true);
	}
	bullet_life_ms_timer.Start();

	return true;
}

bool Obj_Bullet::Update(float dt)
{
	pos_map += direction * speed * dt;
	
	if (bullet_life_ms_timer.ReadMs() >= bullet_life_ms)
	{
		return_to_pool = true;
	}

	return true;
}

void Obj_Bullet::OnTriggerEnter(Collider * collider_1)
{
	return_to_pool = true;
}

void Obj_Bullet::SetBulletProperties(float speed, float bullet_life_ms, float damage, fPoint direction, float angle, bool charged)
{
	this->speed = speed;
	this->direction = direction;
	this->bullet_life_ms = bullet_life_ms;
	SetDamage(damage);
	this->angle = angle;
	this->charged = charged;
}

void Obj_Bullet::SetPlayer(Obj_Tank * player)
{
	this->player = player;
}
