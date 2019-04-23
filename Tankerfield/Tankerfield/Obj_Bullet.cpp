#include "Obj_Bullet.h"
#include "M_Render.h"
#include "App.h"
#include "M_Map.h"
#include "M_Collision.h"
#include "M_AnimationBank.h"
#include "Obj_Tank.h"

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

	anim.frames = app->anim_bank->LoadFrames(bullet_node.child("animations").child("rotate"));
	curr_anim = &anim;

	tex = app->tex->Load(bullet_node.child("tex").attribute("path").as_string());
	curr_tex = tex;

	if (draw_offset.IsZero())
	{
		draw_offset = { 35, 14 };
	}

	coll = app->collision->AddCollider(pos_map, .5f, .5f, Collider::TAG::BULLET,0.f,this);
  
	return true;
}

bool Obj_Bullet::Update(float dt)
{
	pos_map += direction * speed * dt;
	
	if (bullet_life_ms_timer.ReadMs() >= bullet_life_ms)
	{
		to_remove = true;
	}

	coll->AddRigidBody(Collider::BODY_TYPE::SENSOR);

	return true;
}

void Obj_Bullet::OnTrigger(Collider * collider_1)
{
	to_remove = true;
}

void Obj_Bullet::SetBulletProperties(float speed, float bullet_life_ms, float damage, fPoint direction, float angle)
{

	this->speed = speed;
	this->direction = direction;
	this->bullet_life_ms = bullet_life_ms;
	SetDamage(damage);
	this->angle = angle;
}

void Obj_Bullet::SetPlayer(Obj_Tank * player)
{
	this->player = player;
}
