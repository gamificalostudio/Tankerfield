#include "WeaponInfo.h"
#include "App.h"
#include "M_UITest.h"
#include "M_Input.h"
#include "M_Render.h"
#include "M_ObjManager.h"
#include "Obj_Bullet.h"
#include "App.h"
#include "Log.h"
#include "Object.h"

void WeaponInfo::LoadProperties(const pugi::xml_node & node)
{
	bullet_damage = node.child("bullet_damage").attribute("value").as_int();
	bullet_speed = node.child("bullet_speed").attribute("value").as_float();
	bullet_life_ms = node.child("bullet_life_ms").attribute("value").as_float();
	time_between_bullets = node.child("time_between_bullets").attribute("value").as_float();
	bullet_healing = node.child("bullet_healing").attribute("value").as_int();
}