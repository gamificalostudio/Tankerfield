#include "Weapon.h"
#include "App.h"
#include "M_UITest.h"
#include "M_Input.h"
#include "M_Render.h"
#include "M_ObjManager.h"
#include "Obj_Bullet.h"
#include "App.h"
#include "Log.h"
#include "Object.h"

void WeaponInfo::LoadProperties(pugi::xml_node & node)
{
	damage = node.child("damage").text().as_int();
	bullet_speed = node.child("bullet_speed").text().as_float();
	bullet_life_ms = node.child("bullet_life_ms").text().as_float();
	time_between_bullets = node.child("time_between_bullets").text().as_float();
}

//void WeaponInfo::Shoot(const fPoint & pos, fPoint & dir, float angle)
//{
//	bullet = (Obj_Bullet*)app->objectmanager->CreateObject(bullet_type, pos);
//	bullet->SetDamage(damage);
//	bullet->direction = dir;
//	bullet->bullet_life_ms = bullet_life_ms;
//	bullet->speed = speed;
//	bullet->angle = angle;
//}