#include "Weapon.h"
#include "App.h"
#include "M_UITest.h"
#include "M_Input.h"
#include "M_Render.h"
#include "M_ObjManager.h"
#include "Obj_Bullet.h"
#include "App.h"
#include "Log.h"

Weapon::Weapon()
{
}

Weapon::Weapon(int damage, float speed, float life, float time_betwen_bulltes, ObjectType type) :
	damage(damage),
	speed(speed),
	bullet_life_ms(life),
	bullet_type(type),
	time_between_bullets(time_betwen_bulltes)
{
}

Weapon::~Weapon()
{
}

void Weapon::Shoot(fPoint & pos, fPoint & dir)
{
	bullet = (Obj_Bullet*)app->objectmanager->CreateObject(bullet_type, pos);
	bullet->direction = dir;
	bullet->bullet_life_ms = bullet_life_ms;
	bullet->damage = damage;
	bullet->speed = speed;
}