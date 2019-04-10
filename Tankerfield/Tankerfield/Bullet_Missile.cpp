#include "Bullet_Missile.h"
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
	explosion=app->collision->AddCollider(pos_map, 5.f, 5.f, Collider::TAG::EXPLOSION, 200.f, this);
	explosion->AddRigidBody(Collider::BODY_TYPE::SENSOR);
	to_remove = true;
}

