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

	shot1_function[(uint)WEAPON::BASIC] = &Obj_Tank::ShootBasic;
	shot1_function[(uint)WEAPON::DOUBLE_MISSILE] = &Obj_Tank::ShootDoubleMissile;
	shot1_function[(uint)WEAPON::HEALING_SHOT] = &Obj_Tank::ShootHealingShot;
	shot1_function[(uint)WEAPON::LASER_SHOT] = &Obj_Tank::ShootLaserShot;
	shot1_function[(uint)WEAPON::FLAMETHROWER] = &Obj_Tank::ShootFlameThrower;

	charge_time = 3000.f; // Same for all bullets (player gets used to it)
	quick_shot_time = 500.f;

	shot2_function[(uint)WEAPON::BASIC] = &Obj_Tank::ShootBasic;
	shot2_function[(uint)WEAPON::DOUBLE_MISSILE] = &Obj_Tank::ShootDoubleMissileCharged;
	shot2_function[(uint)WEAPON::HEALING_SHOT] = &Obj_Tank::ShootHealingShot;
	shot2_function[(uint)WEAPON::LASER_SHOT] = &Obj_Tank::ShootLaserShotCharged;
	shot2_function[(uint)WEAPON::FLAMETHROWER] = &Obj_Tank::ShootFlameThrower;
}

//if (controller != nullptr) { (*controller)->PlayRumble(0.92f, 250); }
//if (controller != nullptr) { (*controller)->PlayRumble(1.0f, 400); }

void Obj_Tank::SetWeapon(WEAPON type, uint level)
{
	weapon_info.level_weapon = level;
	weapon_info.weapon = type;

	gui->SetWeaponIcon(type);

	switch (type)
	{
	case WEAPON::BASIC:
		weapon_info.type = WEAPON_TYPE::CHARGED;
		weapon_info.bullet_damage = 25 + level * 2;
		weapon_info.explosion_damage = 0;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		weapon_info.basic_shot_trauma = 0.54f;
		weapon_info.charged_shot_trauma = 0.76f;
		weapon_info.shot1_rumble_strength = 0.92f;
		weapon_info.shot1_rumble_duration = 250;
		weapon_info.shot2_rumble_strength = 1.0f;
		weapon_info.shot2_rumble_duration = 400;
		break;
	case WEAPON::FLAMETHROWER:
		weapon_info.type = WEAPON_TYPE::SUSTAINED;
		weapon_info.bullet_damage = 50 + level * 2;
		weapon_info.explosion_damage = 0;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		weapon_info.basic_shot_trauma = 0.54f;
		weapon_info.charged_shot_trauma = 0.76f;
		weapon_info.shot1_rumble_strength = 0.92f;
		weapon_info.shot1_rumble_duration = 250;
		weapon_info.shot2_rumble_strength = 1.0f;
		weapon_info.shot2_rumble_duration = 400;
		break;
	case WEAPON::DOUBLE_MISSILE:
		weapon_info.type = WEAPON_TYPE::CHARGED;
		weapon_info.bullet_damage = 0;
		weapon_info.explosion_damage = level * 100;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		weapon_info.basic_shot_trauma = 0.54f;
		weapon_info.charged_shot_trauma = 0.76f;
		weapon_info.shot1_rumble_strength = 0.92f;
		weapon_info.shot1_rumble_duration = 250;
		weapon_info.shot2_rumble_strength = 1.0f;
		weapon_info.shot2_rumble_duration = 400;
		break;
	case WEAPON::HEALING_SHOT:
		weapon_info.type = WEAPON_TYPE::CHARGED;
		weapon_info.bullet_damage = 25 + level;
		weapon_info.explosion_damage = 0;
		weapon_info.bullet_healing = 5 + level;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 10;
		weapon_info.time_between_bullets = 500;
		weapon_info.basic_shot_trauma = 0.54f;
		weapon_info.charged_shot_trauma = 0.76f;
		weapon_info.shot1_rumble_strength = 0.92f;
		weapon_info.shot1_rumble_duration = 250;
		weapon_info.shot2_rumble_strength = 1.0f;
		weapon_info.shot2_rumble_duration = 400;
		break;
	case WEAPON::LASER_SHOT:
		weapon_info.type = WEAPON_TYPE::CHARGED;
		weapon_info.bullet_damage = 10 + level * 2;
		weapon_info.explosion_damage = 0;
		weapon_info.bullet_healing = 0;
		weapon_info.bullet_life_ms = 2000;
		weapon_info.bullet_speed = 20;
		weapon_info.time_between_bullets = 500;
		weapon_info.basic_shot_trauma = 0.405f;
		weapon_info.charged_shot_trauma = 0.57f;
		weapon_info.shot1_rumble_strength = 0.92f;
		weapon_info.shot1_rumble_duration = 250;
		weapon_info.shot2_rumble_strength = 1.0f;
		weapon_info.shot2_rumble_duration = 400;
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
	float bullet_angle = atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45;
	Bullet_Missile * missile_ptr = nullptr;

	missile_ptr = (Bullet_Missile*)app->objectmanager->CreateObject(ObjectType::BULLET_MISSILE, turr_pos + double_missiles_offset * missiles_offset);
	missile_ptr->SetPlayer(this);
	missile_ptr->SetBulletProperties(
		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		bullet_angle);
	missile_ptr->explosion_damage = weapon_info.explosion_damage;

	missile_ptr = (Bullet_Missile*)app->objectmanager->CreateObject(ObjectType::BULLET_MISSILE, turr_pos - double_missiles_offset * missiles_offset);
	missile_ptr->SetPlayer(this);
	missile_ptr->SetBulletProperties(
		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		bullet_angle);
	missile_ptr->explosion_damage = weapon_info.explosion_damage;
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

void Obj_Tank::ShootLaserShotCharged()
{

	Laser_Bullet *	 laser_bullet = (Laser_Bullet*)app->objectmanager->CreateObject(ObjectType::BULLET_LASER, turr_pos + shot_dir);

	laser_bullet->SetBulletProperties(
		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45,
		true);
	//Se podría mirar de juntar las dos funciones (cargada y sin cargar) del láser y pasarle el parámetro en la funcion si está charged o no y pasarlo al SetBulletsProperties. No sé si para el resto de armas es útil o no.

}

void Obj_Tank::ShootFlameThrower()
{

	FlameThrower_Bullet *	 flamethrower_bullet = (FlameThrower_Bullet*)app->objectmanager->CreateObject(ObjectType::FLAMETHROWER_FLAME, turr_pos + shot_dir);

	flamethrower_bullet->SetBulletProperties(

}

void Obj_Tank::ShootDoubleMissileCharged()
{
	fPoint double_missiles_offset = shot_dir;
	double_missiles_offset.RotateDegree(90);
	float missiles_offset = 0.2f;

	Bullet_Missile * left_missile = (Bullet_Missile*)app->objectmanager->CreateObject(ObjectType::BULLET_MISSILE, turr_pos + double_missiles_offset * missiles_offset);
	left_missile->SetPlayer(this);

	Bullet_Missile * right_missile = (Bullet_Missile*)app->objectmanager->CreateObject(ObjectType::BULLET_MISSILE, turr_pos - double_missiles_offset * missiles_offset);
	right_missile->SetPlayer(this);

	Bullet_Missile * left_missile2 = (Bullet_Missile*)app->objectmanager->CreateObject(ObjectType::BULLET_MISSILE, turr_pos + double_missiles_offset * missiles_offset*3-shot_dir*1.5);
	left_missile2->SetPlayer(this);

	Bullet_Missile * right_missile2 = (Bullet_Missile*)app->objectmanager->CreateObject(ObjectType::BULLET_MISSILE, turr_pos - double_missiles_offset * missiles_offset*3-shot_dir*1.5);
	right_missile2->SetPlayer(this);

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

	left_missile2->SetBulletProperties(
		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		bullet_angle);

	right_missile2->SetBulletProperties(

		weapon_info.bullet_speed,
		weapon_info.bullet_life_ms,
		weapon_info.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45);
		bullet_angle);
}