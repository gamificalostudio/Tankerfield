#include "Obj_Tank.h"
#include "Bullet_Missile.h"
#include "Bullet_Healing.h"
#include "Bullet_Laser.h"
#include "App.h"
#include "M_ObjManager.h"
#include "Player_GUI.h"

void Obj_Tank::StartWeapons()
{
	//Basic weapon starting properties
	weapon_info.bullet_damage = 50;
	weapon_info.bullet_healing = 0;
	weapon_info.bullet_life_ms = 2000;
	weapon_info.bullet_speed = 10;
	weapon_info.time_between_bullets = 500;

	basic_shot_function[(uint)WEAPON::BASIC] = &Obj_Tank::ShootBasic;
	basic_shot_function[(uint)WEAPON::DOUBLE_MISSILE] = &Obj_Tank::ShootDoubleMissile;
	basic_shot_function[(uint)WEAPON::HEALING_SHOT] = &Obj_Tank::ShootHealingShot;
	basic_shot_function[(uint)WEAPON::LASER_SHOT] = &Obj_Tank::ShootLaserShot;


	charge_time = 3000.f; // Same for all bullets (player gets used to it)
	charged_shot_function[(uint)WEAPON::BASIC] = &Obj_Tank::ShootBasic;
	charged_shot_function[(uint)WEAPON::DOUBLE_MISSILE] = &Obj_Tank::ShootDoubleMissile;
	charged_shot_function[(uint)WEAPON::HEALING_SHOT] = &Obj_Tank::ShootHealingShot;
}

void Obj_Tank::SetWeapon(WEAPON type, uint level)
{
	weapon_info.level_weapon = level;
	weapon_info.type = type;

	gui->SetWeaponIcon(type);

	switch (type)
	{
	case WEAPON::BASIC:
		weapon_info.bullet_damage = 50 + level * 2;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		break;
	case WEAPON::FLAMETHROWER:
		weapon_info.bullet_damage = 50 + level * 2;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		break;
	case WEAPON::DOUBLE_MISSILE:
		weapon_info.bullet_damage = 50 + level * 2;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		break;
	case WEAPON::HEALING_SHOT:
		weapon_info.bullet_damage = 0;
		weapon_info.bullet_healing = 25 + 1 * level;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		break;
	case WEAPON::LASER_SHOT:
		weapon_info.bullet_damage = 50 + level * 2;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		break;
	}
}

void Obj_Tank::ShootDoubleMissile()
{
	fPoint double_missiles_offset = shot_dir;
	double_missiles_offset.RotateDegree(90);
	float missiles_offset = 0.2f;

	Bullet_Missile * left_missile = (Bullet_Missile*)app->objectmanager->CreateObject(ObjectType::BULLET_MISSILE, turr_pos + shot_dir * cannon_length + double_missiles_offset * missiles_offset);
	Bullet_Missile * right_missile = (Bullet_Missile*)app->objectmanager->CreateObject(ObjectType::BULLET_MISSILE, turr_pos + shot_dir * cannon_length - double_missiles_offset * missiles_offset);

	float bullet_angle = atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45;

	left_missile->SetBulletProperties(
		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		bullet_angle);

	right_missile->SetBulletProperties(
		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		bullet_angle);
}

void Obj_Tank::ShootHealingShot()
{
	Healing_Bullet * heal_bullet = (Healing_Bullet*)app->objectmanager->CreateObject(ObjectType::HEALING_BULLET, turr_pos + shot_dir * cannon_length);

	heal_bullet->SetBulletProperties(
		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45);
}

void Obj_Tank::ShootLaserShot()
{
	Laser_Bullet *	 laser_bullet = (Laser_Bullet*)app->objectmanager->CreateObject(ObjectType::BULLET_LASER, turr_pos + shot_dir * cannon_length);

	laser_bullet->SetBulletProperties(
		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45);
}

void Obj_Tank::ShootFlameThrower()
{
}

void Obj_Tank::ShootBasic()
{
	Obj_Bullet * bullet = (Obj_Bullet*)app->objectmanager->CreateObject(ObjectType::BASIC_BULLET, turr_pos + shot_dir * cannon_length);
	bullet->SetBulletProperties(
		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45);
}