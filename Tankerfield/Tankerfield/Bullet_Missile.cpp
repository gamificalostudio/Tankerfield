#include "Bullet_Missile.h"
#include "PerfTimer.h"
#include "App.h"
#include "M_Collision.h"

Bullet_Missile::Bullet_Missile(fPoint pos) : Obj_Bullet(pos)
{
}

Bullet_Missile::~Bullet_Missile()
{
}

void Bullet_Missile::OnTrigger(Collider * collider_1)
{
	fPoint explosion_offset = { 1.5f,1.5f };
	explosion=app->collision->AddCollider(pos_map-explosion_offset, 3.f, 3.f, Collider::TAG::BULLET, 200.f, nullptr);
	explosion->AddRigidBody(Collider::BODY_TYPE::SENSOR);
	to_remove = true;
	explosion->to_destroy = true;
}

