#include "Bullet_Laser.h"
#include "PerfTimer.h"
#include "App.h"
#include "M_Collision.h"
#include "M_ObjManager.h"
#include "Obj_Explosion.h"

Bullet_Laser::Bullet_Laser(fPoint pos) : Obj_Bullet(pos)
{
}

Bullet_Laser::~Bullet_Laser()
{
}

void Bullet_Laser::OnTrigger(Collider * collider_1)
{
	app->objectmanager->CreateObject(ObjectType::EXPLOSION, pos_map);
	to_remove = true;
}

