#include "Obj_Bullet_RocketLauncher.h"
#include "M_Render.h"
#include "App.h"
#include "M_Map.h"
#include "M_Collision.h"
#include "M_AnimationBank.h"
#include "Obj_Tank.h"

Obj_Bullet_RocketLauncher::Obj_Bullet_RocketLauncher(fPoint pos) : Object(pos)
{
	bullet_life_ms_timer.Start();
}

Obj_Bullet_RocketLauncher::~Obj_Bullet_RocketLauncher()
{
}

bool Obj_Bullet_RocketLauncher::Start()
{
	pugi::xml_node bullet_node = app->config.child("object").child("basic_bullet_rocketlauncher");

	anim.frames = app->anim_bank->LoadFrames(bullet_node.child("animations").child("rotate"));
	curr_anim = &anim;

	tex = app->tex->Load(bullet_node.child("tex").attribute("path").as_string());
	curr_tex = tex;

	draw_offset = { 35, 14 };

	float coll_w = 0.5f;
	float coll_h = 0.5f;
	coll = app->collision->AddCollider(pos_map, coll_w, coll_h, TAG::BULLET, BODY_TYPE::DYNAMIC, 0.f, this);
	coll->SetObjOffset({ -coll_w * 0.5f, -coll_h * 0.5f });
	coll->is_sensor = true;

	return true;
}

bool Obj_Bullet_RocketLauncher::Update(float dt)
{
	pos_map += direction * speed * dt;

	if (bullet_life_ms_timer.ReadMs() >= bullet_life_ms)
	{
		to_remove = true;

		if (coll != nullptr)
		{
			coll->to_destroy = true;
		}

	}


	return true;
}

void Obj_Bullet_RocketLauncher::OnTriggerEnter(Collider * collider_1)
{
	to_remove = true;
}

void Obj_Bullet_RocketLauncher::SetBulletProperties(float speed, float bullet_life_ms, float damage, fPoint direction, float angle, bool charged)
{
	this->speed = speed;
	this->direction = direction;
	this->bullet_life_ms = bullet_life_ms;
	SetDamage(damage);
	this->angle = angle;
	this->charged = charged;
}

void Obj_Bullet_RocketLauncher::SetPlayer(Obj_Tank * player)
{
	this->player = player;
}