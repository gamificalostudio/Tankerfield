//File to keep all the shot functions of the different weapons
#include "Obj_Tank.h"
#include "App.h"
#include "M_ObjManager.h"
#include "Player_GUI.h"

//Bullets
#include "Bullet_Missile.h"
#include "Healing_Bullet.h"
#include "Bullet_Laser.h"
#include "Bullet_FlameThrower.h"

void Obj_Tank::InitWeapons()
{
	//Basic weapon starting properties
	SetWeapon(WEAPON::FLAMETHROWER, 1u);

	basic_shot_function[(uint)WEAPON::BASIC] = &Obj_Tank::ShootBasic;
	basic_shot_function[(uint)WEAPON::DOUBLE_MISSILE] = &Obj_Tank::ShootDoubleMissile;
	basic_shot_function[(uint)WEAPON::HEALING_SHOT] = &Obj_Tank::ShootHealingShot;
	basic_shot_function[(uint)WEAPON::LASER_SHOT] = &Obj_Tank::ShootLaserShot;

	charge_time = 3000.f; // Same for all bullets (player gets used to it)
	charged_shot_function[(uint)WEAPON::BASIC] = &Obj_Tank::ShootBasic;
	charged_shot_function[(uint)WEAPON::DOUBLE_MISSILE] = &Obj_Tank::ShootDoubleMissile;
	charged_shot_function[(uint)WEAPON::HEALING_SHOT] = &Obj_Tank::ShootHealingShot;
	charged_shot_function[(uint)WEAPON::LASER_SHOT] = &Obj_Tank::ShootLaserShot;
}

//if (controller != nullptr) { (*controller)->PlayRumble(0.92f, 250); }
//if (controller != nullptr) { (*controller)->PlayRumble(1.0f, 400); }

void Obj_Tank::SetWeapon(WEAPON type, uint level)
{
	weapon_info.level_weapon = level;
	weapon_info.type = type;

	gui->SetWeaponIcon(type);

	switch (type)
	{
	case WEAPON::BASIC:
		weapon_info.bullet_damage = 25 + level * 2;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		weapon_info.basic_shot_trauma = 0.54f;
		weapon_info.charged_shot_trauma = 0.76f;
		weapon_info.basic_rumble_strength = 0.92f;
		weapon_info.basic_rumble_duration = 250;
		weapon_info.charged_rumble_strength = 1.0f;
		weapon_info.charged_rumble_duration = 400;
		break;
	case WEAPON::FLAMETHROWER:
		weapon_info.bullet_damage = 50 + level * 2;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		weapon_info.basic_shot_trauma = 0.54f;
		weapon_info.charged_shot_trauma = 0.76f;
		weapon_info.basic_rumble_strength = 0.92f;
		weapon_info.basic_rumble_duration = 250;
		weapon_info.charged_rumble_strength = 1.0f;
		weapon_info.charged_rumble_duration = 400;
		break;
	case WEAPON::DOUBLE_MISSILE:
		weapon_info.bullet_damage = 0;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		weapon_info.basic_shot_trauma = 0.54f;
		weapon_info.charged_shot_trauma = 0.76f;
		weapon_info.basic_rumble_strength = 0.92f;
		weapon_info.basic_rumble_duration = 250;
		weapon_info.charged_rumble_strength = 1.0f;
		weapon_info.charged_rumble_duration = 400;
		break;
	case WEAPON::HEALING_SHOT:
		weapon_info.bullet_damage = 25 + level;
		weapon_info.bullet_healing = 5 + level;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		weapon_info.basic_shot_trauma = 0.54f;
		weapon_info.charged_shot_trauma = 0.76f;
		weapon_info.basic_rumble_strength = 0.92f;
		weapon_info.basic_rumble_duration = 250;
		weapon_info.charged_rumble_strength = 1.0f;
		weapon_info.charged_rumble_duration = 400;
		break;
	case WEAPON::LASER_SHOT:
		weapon_info.bullet_damage = 10 + level * 2;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 20;
		weapon_info.time_between_bullets = 500;
		weapon_info.basic_shot_trauma = 0.405f;
		weapon_info.charged_shot_trauma = 0.57f;
		weapon_info.basic_rumble_strength = 0.92f;
		weapon_info.basic_rumble_duration = 250;
		weapon_info.charged_rumble_strength = 1.0f;
		weapon_info.charged_rumble_duration = 400;
		break;
	}
}

void Obj_Tank::ShootBasic()
{
	Obj_Bullet * bullet = (Obj_Bullet*)app->objectmanager->CreateObject(ObjectType::BASIC_BULLET, turr_pos);
	bullet->SetBulletProperties(
		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45);
}

void Obj_Tank::ShootDoubleMissile()
{
	fPoint double_missiles_offset = shot_dir;
	double_missiles_offset.RotateDegree(90);
	float missiles_offset = 0.2f;

	Bullet_Missile * left_missile = (Bullet_Missile*)app->objectmanager->CreateObject(ObjectType::BULLET_MISSILE, turr_pos + double_missiles_offset * missiles_offset);
	left_missile->SetPlayer(this);

	Bullet_Missile * right_missile = (Bullet_Missile*)app->objectmanager->CreateObject(ObjectType::BULLET_MISSILE, turr_pos - double_missiles_offset * missiles_offset);
	right_missile->SetPlayer(this);

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
	Healing_Bullet * heal_bullet = (Healing_Bullet*)app->objectmanager->CreateObject(ObjectType::HEALING_BULLET, turr_pos + shot_dir);

	heal_bullet->SetBulletProperties(
		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45);

	heal_bullet->tank_parent = this;
}

void Obj_Tank::ShootLaserShot()
{
	Laser_Bullet *	 laser_bullet = (Laser_Bullet*)app->objectmanager->CreateObject(ObjectType::BULLET_LASER, turr_pos + shot_dir);

	laser_bullet->SetBulletProperties(
		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45);
}

void Obj_Tank::ShootFlameThrower()
{
	FlameThrower_Bullet *	 flamethrower_bullet = (FlameThrower_Bullet*)app->objectmanager->CreateObject(ObjectType::FIRE_DEAD, turr_pos + shot_dir);

	flamethrower_bullet->SetBulletProperties(
		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45);
}