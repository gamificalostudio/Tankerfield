#include "Bullet_Missile.h"
#include "PerfTimer.h"
#include "App.h"
#include "M_Collision.h"
#include "M_ObjManager.h"
#include "Obj_Explosion.h"

Bullet_Missile::Bullet_Missile(fPoint pos) : Obj_Bullet(pos)
{
}

Bullet_Missile::~Bullet_Missile()
{
}

void Bullet_Missile::OnTrigger(Collider * collider_1)
{
	app->objectmanager->CreateObject(ObjectType::EXPLOSION, pos_map-explosion_offset);
	to_remove = true;
}

