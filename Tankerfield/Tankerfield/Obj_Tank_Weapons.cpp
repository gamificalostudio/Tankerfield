//File to keep all the shot functions of the different weapons
#include "Obj_Tank.h"
#include "App.h"
#include "M_ObjManager.h"
#include "Player_GUI.h"
#include "Log.h"
#include "M_Collision.h"
#include "M_AnimationBank.h"
#include "M_Textures.h"
#include "M_Map.h"
#include "M_Audio.h"
#include "M_Input.h"
#include "Obj_Emitter.h"

//Bullets
#include "Bullet_Missile.h"
#include "Healing_Bullet.h"
#include "Bullet_Laser.h"
#include "Bullet_Oil.h"
#include "Obj_OilPool.h"
#include "Obj_ElectroShotAnimation.h"
#include "HealingShot_Area.h"

void Obj_Tank::InitWeapons()
{
	//Basic weapon starting properties
	SetWeapon(WEAPON::BASIC, 1u);

	shot1_function[(uint)WEAPON::BASIC] = &Obj_Tank::ShootBasic;
	shot1_function[(uint)WEAPON::DOUBLE_MISSILE] = &Obj_Tank::ShootDoubleMissile;
	shot1_function[(uint)WEAPON::HEALING_SHOT] = &Obj_Tank::ShootHealingShot;
	shot1_function[(uint)WEAPON::LASER_SHOT] = &Obj_Tank::ShootLaserShot;
	shot1_function[(uint)WEAPON::OIL] = &Obj_Tank::ShootOil;
	shot1_function[(uint)WEAPON::ELECTRO_SHOT] = &Obj_Tank::ShootElectroShot;
	shot1_function[(uint)WEAPON::FLAMETHROWER] = &Obj_Tank::ShootFlameThrower;

	//Electro_shot--
	pugi::xml_node electro_shot_node = app->config.child("object").child("tank").child("electro_shot");

	float coll_size_init = electro_shot_node.child("collider_size").attribute("value").as_float();
	electro_shot_sound = app->audio->LoadFx(electro_shot_node.child("electro_shot_sound").child_value(),25);
	//Basic electro shot colliders
	for (uint i = 1; i <= 3/*num max of colliders*/; ++i)
	{
		float coll_size = coll_size_init * i;
		Collider* electro_shot_collider = nullptr;

		electro_shot_collider = app->collision->AddCollider(pos_map, coll_size, coll_size, TAG::ELECTRO_SHOT, BODY_TYPE::DYNAMIC, 100, this);
		electro_shot_collider->is_sensor = true;
		//electro_shot_collider->Disactivate();
		electro_shot_collider->SetIsTrigger(false);

		electric_shot_colliders_vector.push_back(electro_shot_collider);
	}

	coll_size_init *= 2.f;

	//charged electro shot colliders
	for (uint i = 1; i <= 3/*num max of colliders*/; ++i)
	{
		float coll_size = coll_size_init * i;
		Collider* electro_shot_collider_charged = nullptr;

		electro_shot_collider_charged = app->collision->AddCollider(pos_map, coll_size, coll_size, TAG::ELECTRO_SHOT, BODY_TYPE::DYNAMIC, 100, this);
		electro_shot_collider_charged->is_sensor = true;
		//electro_shot_collider_charged->Disactivate();
		electro_shot_collider_charged->SetIsTrigger(false);

		electric_shot_colliders_charged_vector.push_back(electro_shot_collider_charged);
	}
  
	charge_time = 1500.f; // Same for all bullets (player gets used to it)
	shot2_function[(uint)WEAPON::BASIC] = &Obj_Tank::ShootBasic;
	shot2_function[(uint)WEAPON::DOUBLE_MISSILE] = &Obj_Tank::ShootDoubleMissileCharged;
	shot2_function[(uint)WEAPON::HEALING_SHOT] = &Obj_Tank::ShootHealingShotCharged;
	shot2_function[(uint)WEAPON::LASER_SHOT] = &Obj_Tank::ShootLaserShotCharged;
	shot2_function[(uint)WEAPON::OIL] = &Obj_Tank::ShootOilCharged;
	shot2_function[(uint)WEAPON::ELECTRO_SHOT] = &Obj_Tank::ShootElectroShotCharged;
	shot2_function[(uint)WEAPON::FLAMETHROWER] = &Obj_Tank::ShootFlameThrower;

	release_shot[(uint)WEAPON::FLAMETHROWER] = &Obj_Tank::ReleaseFlameThrower;
	release_shot[(uint)WEAPON::BASIC] = &Obj_Tank::ReleaseBasicShot;

	
}

void Obj_Tank::UpdateWeaponsWithoutBullets(float dt)
{
	if (weapon_info.weapon == WEAPON::ELECTRO_SHOT)
	{
		//with the animation get frame?? only 1 frame
		if (electro_shot_timer.ReadMs() >= weapon_info.shot1.bullet_life_ms && (*electric_shot_colliders_vector.begin())->GetIsTrigger())
		{
			for (std::vector<Collider*>::iterator iter = electric_shot_colliders_vector.begin(); iter != electric_shot_colliders_vector.end(); ++iter)
			{
				(*iter)->SetIsTrigger(false);
			}
		}
		else if (electro_shot_timer.ReadMs() >= weapon_info.shot1.bullet_life_ms && (*electric_shot_colliders_charged_vector.begin())->GetIsTrigger())
		{
			for (std::vector<Collider*>::iterator iter = electric_shot_colliders_charged_vector.begin(); iter != electric_shot_colliders_charged_vector.end(); ++iter)
			{
				(*iter)->SetIsTrigger(false);
			}
		}

		if (hit_no_enemie)
		{
			hit_no_enemie = false;
			Eletro_Shot_Animation* electro_anim = (Eletro_Shot_Animation*)app->objectmanager->CreateObject(ObjectType::ELECTRO_SHOT_ANIMATION, pos_map);
			app->audio->PlayFx(electro_shot_sound);
			electro_anim->tank = this;
			electro_anim->draw_offset -= (iPoint)app->map->MapToScreenF(GetShotDir());
			electro_anim->hit_no_enemie = true;
		}
	}
	else if (weapon_info.weapon == WEAPON::FLAMETHROWER)
	{
		float turr_length_flamethrower = 1.5f;
		flame_emitter->pos_map = pos_map + shot_dir * turr_length_flamethrower;
		float shot_dir_angle = atan2(-shot_dir.y, shot_dir.x) * RADTODEG;
		float angle_variation = 20;
		flame_emitter->angleRange = {
			shot_dir_angle - angle_variation * 0.5f,
			shot_dir_angle + angle_variation * 0.5f};
	}
}

void Obj_Tank::SetWeapon(WEAPON type, uint level)
{
	weapon_info.level_weapon = level;
	weapon_info.weapon = type;

	gui->SetWeapon(type, level);

	switch (type)
	{
	case WEAPON::BASIC:
		weapon_info.type = WEAPON_TYPE::SUSTAINED;
		weapon_info.quick_shot_time = 0.f;
		weapon_info.shot1.bullet_damage = app->objectmanager->basic_weapon_info.damage_multiplier * pow(app->objectmanager->basic_weapon_info.damage_exponential_base, level - 1);
		weapon_info.shot1.explosion_damage = 0;
		weapon_info.shot1.bullet_healing = 0;
		weapon_info.shot1.bullet_life_ms = 2000;
		weapon_info.shot1.bullet_speed = app->objectmanager->basic_weapon_info.speed;
		weapon_info.shot1.time_between_bullets = 100;
		weapon_info.shot1.trauma = weapon_info.shot2.trauma = 0.f;
		weapon_info.shot1.rumble_strength = weapon_info.shot2.rumble_strength = 0.2f;
		weapon_info.shot1.rumble_duration = weapon_info.shot2.rumble_duration = 100;
		weapon_info.shot1.smoke_particle = weapon_info.shot2.smoke_particle = ObjectType::MAX;
		weapon_info.shot1.recoil = 25;
		break;
	case WEAPON::FLAMETHROWER:
		weapon_info.type = WEAPON_TYPE::SUSTAINED;
		weapon_info.quick_shot_time = 500.f;
		weapon_info.shot1.bullet_damage = app->objectmanager->flamethrower_info.damage_multiplier * pow(app->objectmanager->flamethrower_info.damage_exponential_base, level - 1);;
		weapon_info.shot1.explosion_damage = 0;
		weapon_info.shot1.bullet_healing = 0;
		weapon_info.shot1.bullet_life_ms = 2000;
		weapon_info.shot1.bullet_speed = 10;
		weapon_info.shot1.time_between_bullets = 500;
		weapon_info.shot1.trauma = 0.0f;
		weapon_info.shot2.trauma = 0.f;
		weapon_info.shot1.rumble_strength = 0.92f;
		weapon_info.shot1.rumble_duration = 250;
		weapon_info.shot2.rumble_strength = 0.3f;
		weapon_info.shot2.rumble_duration = 400;
		weapon_info.shot1.smoke_particle = ObjectType::CANNON_FIRE;
		weapon_info.shot2.smoke_particle = ObjectType::CANNON_FIRE;
		weapon_info.shot1.recoil = 0;
		break;
	case WEAPON::DOUBLE_MISSILE:
		weapon_info.type = WEAPON_TYPE::CHARGED;
		weapon_info.shot1.bullet_damage = 0;
		weapon_info.shot1.explosion_damage = app->objectmanager->double_missile_info.damage_multiplier * pow(app->objectmanager->double_missile_info.damage_exponential_base, level - 1);;
		weapon_info.shot1.bullet_healing = 0;
		weapon_info.shot1.bullet_life_ms = 2000;
		weapon_info.shot1.bullet_speed = weapon_info.shot2.bullet_speed = app->objectmanager->double_missile_info.speed;
		weapon_info.shot1.time_between_bullets = 500;
		weapon_info.shot1.trauma = 0.54f;
		weapon_info.shot2.trauma = 0.76f;
		weapon_info.shot1.rumble_strength = 0.92f;
		weapon_info.shot1.rumble_duration = 250;
		weapon_info.shot2.rumble_strength = 1.0f;
		weapon_info.shot2.rumble_duration = 400;
		weapon_info.shot1.smoke_particle = ObjectType::CANNON_FIRE;
		weapon_info.shot2.smoke_particle = ObjectType::CANNON_FIRE;
		weapon_info.shot1.recoil = 25;
		weapon_info.shot2.recoil = 50;
		break;
	case WEAPON::HEALING_SHOT:
		weapon_info.type = WEAPON_TYPE::CHARGED;
		weapon_info.shot1.bullet_damage = app->objectmanager->healing_shot_info.damage_multiplier * pow(app->objectmanager->healing_shot_info.damage_exponential_base, level - 1);;
		weapon_info.shot1.explosion_damage = 0;
		weapon_info.shot1.bullet_healing = 5 + level;
		weapon_info.shot1.bullet_life_ms = 2000;
		weapon_info.shot1.bullet_speed = 10;
		weapon_info.shot1.time_between_bullets = 500;
		weapon_info.shot1.trauma = 0.54f;
		weapon_info.shot2.trauma = 0;
		weapon_info.shot1.rumble_strength = 0.8f;
		weapon_info.shot1.rumble_duration = 250;
		weapon_info.shot2.rumble_strength = 0.25f;
		weapon_info.shot2.rumble_duration = 250;
		weapon_info.shot2.bullet_healing = 15 + level;
		weapon_info.shot1.smoke_particle = ObjectType::CANNON_FIRE;
		weapon_info.shot2.smoke_particle = ObjectType::MAX;
		weapon_info.shot1.recoil = 25;
		weapon_info.shot2.recoil = 0;
		break;
	case WEAPON::LASER_SHOT:
		weapon_info.type = WEAPON_TYPE::CHARGED;
		weapon_info.shot1.bullet_damage = app->objectmanager->laser_info.damage_multiplier * pow(app->objectmanager->laser_info.damage_exponential_base, level - 1);;
		weapon_info.shot1.explosion_damage = 0;
		weapon_info.shot1.bullet_healing = 0;
		weapon_info.shot1.bullet_life_ms = 2000;
		weapon_info.shot1.bullet_speed = 25;
		weapon_info.shot1.time_between_bullets = 500;
		weapon_info.shot1.trauma = 0.f;
		weapon_info.shot2.trauma = 0.f;
		weapon_info.shot1.rumble_strength = 0.92f;
		weapon_info.shot1.rumble_duration = 250;
		weapon_info.shot2.rumble_strength = 1.0f;
		weapon_info.shot2.rumble_duration = 400;
		weapon_info.shot1.smoke_particle = ObjectType::MAX;
		weapon_info.shot2.smoke_particle = ObjectType::MAX;
		weapon_info.shot1.recoil = 0;
		weapon_info.shot2.recoil = 0;
		break;
	case WEAPON::OIL:
		weapon_info.type = WEAPON_TYPE::CHARGED;
		weapon_info.shot1.bullet_damage = 25 + level * 2;
		weapon_info.shot1.bullet_healing = 0;
		weapon_info.shot1.bullet_life_ms = 2000;
		weapon_info.shot1.bullet_speed = weapon_info.shot2.bullet_speed = app->objectmanager->oil_weapon_info.speed;
		weapon_info.shot1.time_between_bullets = 500;
		weapon_info.shot1.trauma = 0.54f;
		weapon_info.shot2.trauma = 0.76f;
		weapon_info.shot1.rumble_strength = 0.92f;
		weapon_info.shot1.rumble_duration = 250;
		weapon_info.shot2.rumble_strength = 1.0f;
		weapon_info.shot2.rumble_duration = 400;
		weapon_info.shot1.smoke_particle = weapon_info.shot2.smoke_particle = ObjectType::MAX;
		weapon_info.shot1.recoil = 25;
		weapon_info.shot2.recoil = 50;
		break;
	case WEAPON::ELECTRO_SHOT:
		weapon_info.type = WEAPON_TYPE::CHARGED;
		weapon_info.shot1.bullet_damage = app->objectmanager->electro_shot_info.damage_multiplier * pow(app->objectmanager->electro_shot_info.damage_exponential_base, level - 1);
		weapon_info.shot2.bullet_damage = app->objectmanager->electro_shot_info.damage_multiplier * pow(app->objectmanager->electro_shot_info.damage_exponential_base, level - 1);
		weapon_info.shot1.bullet_healing = 0;
		weapon_info.shot1.bullet_life_ms = 100;
		weapon_info.shot1.bullet_speed = 0;
		weapon_info.shot1.time_between_bullets = 1000;
		weapon_info.shot1.trauma = 0.405f;
		weapon_info.shot2.trauma = 0.57f;
		weapon_info.shot1.rumble_strength = 0.92f;
		weapon_info.shot1.rumble_duration = 250;
		weapon_info.shot2.rumble_strength = 1.0f;
		weapon_info.shot2.rumble_duration = 400;
		weapon_info.shot1.smoke_particle = ObjectType::MAX;
		weapon_info.shot2.smoke_particle = ObjectType::MAX;
		weapon_info.shot1.recoil = 0;
		weapon_info.shot2.recoil = 0;

		//add width and height here?
		for (std::vector<Collider*>::iterator iter = electric_shot_colliders_vector.begin(); iter != electric_shot_colliders_vector.end(); ++iter)
		{
			(*iter)->damage = weapon_info.shot1.bullet_damage;
		}

		for (std::vector<Collider*>::iterator iter = electric_shot_colliders_charged_vector.begin(); iter != electric_shot_colliders_charged_vector.end(); ++iter)
		{
			(*iter)->damage = weapon_info.shot2.bullet_damage;
		}
		break;
	}

		if (weapon_info.type == WEAPON_TYPE::CHARGED)
		{
			curr_text_charging = text_charging;
			curr_anim_charging = anim_charging;
		}
	
	
}

void Obj_Tank::ShootBasic()
{
	if (shot_timer_basic_bullet.ReadMs() >= weapon_info.shot1.time_between_bullets)
	{
		Obj_Bullet * bullet = (Obj_Bullet*)app->objectmanager->GetObjectFromPool(ObjectType::BASIC_BULLET, turr_pos);
		bullet->SetBulletProperties(
			weapon_info.shot1.bullet_speed,
			weapon_info.shot1.bullet_life_ms,
			weapon_info.shot1.bullet_damage,
			shot_dir,
			atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45);

		bullet->SetPlayer(this);
		shot_timer_basic_bullet.Start();
	}
}

void Obj_Tank::ShootDoubleMissile()
{
	fPoint double_missiles_offset = shot_dir;
	double_missiles_offset.RotateDegree(90);
	float missiles_offset = 0.2f;
	float bullet_angle = atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45;
	Bullet_Missile * missile_ptr = nullptr;

	missile_ptr = (Bullet_Missile*)app->objectmanager->GetObjectFromPool(ObjectType::BULLET_MISSILE, turr_pos + double_missiles_offset * missiles_offset);
	missile_ptr->SetPlayer(this);
	missile_ptr->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		bullet_angle);
	missile_ptr->explosion_damage = weapon_info.shot1.explosion_damage;

	missile_ptr = (Bullet_Missile*)app->objectmanager->GetObjectFromPool(ObjectType::BULLET_MISSILE, turr_pos - double_missiles_offset * missiles_offset);
	missile_ptr->SetPlayer(this);
	missile_ptr->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		bullet_angle);
	missile_ptr->explosion_damage = weapon_info.shot1.explosion_damage;
}

void Obj_Tank::ShootDoubleMissileCharged()
{
	fPoint double_missiles_offset = shot_dir;
	double_missiles_offset.RotateDegree(90);
	float missiles_offset = 0.2f;
	float bullet_angle = atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45;
	Bullet_Missile * missile_ptr = nullptr;

	missile_ptr = (Bullet_Missile*)app->objectmanager->GetObjectFromPool(ObjectType::BULLET_MISSILE, turr_pos + double_missiles_offset * missiles_offset);
	missile_ptr->SetPlayer(this);
	missile_ptr->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		bullet_angle);
	missile_ptr->explosion_damage = weapon_info.shot1.explosion_damage;

	missile_ptr = (Bullet_Missile*)app->objectmanager->GetObjectFromPool(ObjectType::BULLET_MISSILE, turr_pos - double_missiles_offset * missiles_offset);
	missile_ptr->SetPlayer(this);
	missile_ptr->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		bullet_angle);
	missile_ptr->explosion_damage = weapon_info.shot1.explosion_damage;

	missile_ptr = (Bullet_Missile*)app->objectmanager->GetObjectFromPool(ObjectType::BULLET_MISSILE, turr_pos + double_missiles_offset * missiles_offset * 3 - shot_dir * 1.5);
	missile_ptr->SetPlayer(this);
	missile_ptr->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		bullet_angle);
	missile_ptr->explosion_damage = weapon_info.shot1.explosion_damage;

	missile_ptr = (Bullet_Missile*)app->objectmanager->GetObjectFromPool(ObjectType::BULLET_MISSILE, turr_pos - double_missiles_offset * missiles_offset * 3 - shot_dir * 1.5);
	missile_ptr->SetPlayer(this);
	missile_ptr->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		bullet_angle);
	missile_ptr->explosion_damage = weapon_info.shot1.explosion_damage;
}

void Obj_Tank::ShootHealingShot()
{
	Healing_Bullet * heal_bullet = (Healing_Bullet*)app->objectmanager->GetObjectFromPool(ObjectType::HEALING_BULLET, turr_pos + shot_dir);

	heal_bullet->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45);

	heal_bullet->SetPlayer(this);
}

void Obj_Tank::ShootHealingShotCharged()
{
	HealingShot_Area * heal_area = (HealingShot_Area*)app->objectmanager->GetObjectFromPool(ObjectType::HEALING_AREA_SHOT, pos_map);

	heal_area->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45);

	heal_area->tank_parent = this;

}

void Obj_Tank::ShootLaserShot()
{

	Laser_Bullet *	 laser_bullet = (Laser_Bullet*)app->objectmanager->GetObjectFromPool(ObjectType::BULLET_LASER, turr_pos + shot_dir);

	laser_bullet->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45);

	laser_bullet->SetPlayer(this);
}

void Obj_Tank::ShootLaserShotCharged()
{

	Laser_Bullet *	 laser_bullet = (Laser_Bullet*)app->objectmanager->GetObjectFromPool(ObjectType::BULLET_LASER, turr_pos + shot_dir);

	laser_bullet->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45,
		true);

	laser_bullet->SetPlayer(this);

}

void Obj_Tank::ShootFlameThrower()
{
	flame_release_time.Start();

	if(coll_flame->GetIsTrigger() == false)
	{
		coll_flame->SetIsTrigger(true);
	}

	float coll_w_init;
	float coll_h_init;

	fPoint distance{ 1.f, 1.f };
	fPoint increment{ coll_w_init, coll_h_init };

	float coll_w;
	float coll_h;
	coll_flame->GetSize(coll_w, coll_h);


	fPoint offset{ -coll_w * 0.5f, -coll_h * 0.5f };

	fPoint dir_distance = GetShotDir() * distance * fPoint{2,2};
	distance += increment;

	coll_flame->SetObjOffset(offset + dir_distance);
	coll_flame->SetPosToObj();

	coll_flame->SetIsTrigger(true);
}

void Obj_Tank::ShootOil()
{
	Bullet_Oil * bullet = (Bullet_Oil*)app->objectmanager->GetObjectFromPool(ObjectType::BULLET_OIL, turr_pos + shot_dir);

	bullet->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45);

	bullet->splash_damage = weapon_info.shot1.bullet_damage;

	bullet->SetPlayer(this);

}

void Obj_Tank::ShootOilCharged()
{
	fPoint pool_pos = turr_pos + shot_dir * 2.5F;
	pool_pos -= fPoint(2.5f, 2.5f);
	Obj_OilPool* pool = (Obj_OilPool*)app->objectmanager->GetObjectFromPool(ObjectType::OIL_POOL, pool_pos);
}

void Obj_Tank::ShootElectroShot()
{
	hit_no_enemie = true;
	enemies_hitted.clear();
	float coll_w_init;
	float coll_h_init;
	(*electric_shot_colliders_vector.begin())->GetSize(coll_w_init, coll_h_init);
	fPoint distance{1.f, 1.f};
	fPoint increment{ coll_w_init, coll_h_init };
	
	for (std::vector<Collider*>::iterator iter = electric_shot_colliders_vector.begin();iter != electric_shot_colliders_vector.end(); ++iter)
	{
		float coll_w;
		float coll_h;
		(*iter)->GetSize(coll_w, coll_h);
		

		fPoint offset{ -coll_w * 0.5f, -coll_h * 0.5f };
		
		

		fPoint dir_distance = GetShotDir() * distance;
		distance += increment;
		
		(*iter)->SetObjOffset(offset + dir_distance);
		(*iter)->SetPosToObj();

		(*iter)->SetIsTrigger(true);
	}
	electro_shot_timer.Start();
	is_electro_shot_charged = false;
}

void Obj_Tank::ShootElectroShotCharged()
{
	hit_no_enemie = true;
	enemies_hitted.clear();
	float coll_w_init;
	float coll_h_init;
	(*electric_shot_colliders_charged_vector.begin())->GetSize(coll_w_init, coll_h_init);
	fPoint distance{1.f, 1.f};
	fPoint increment{ coll_w_init, coll_h_init };

	for (std::vector<Collider*>::iterator iter = electric_shot_colliders_charged_vector.begin(); iter != electric_shot_colliders_charged_vector.end(); ++iter)
	{
		float coll_w;
		float coll_h;
		(*iter)->GetSize(coll_w, coll_h);


		fPoint offset{ -coll_w * 0.5f, -coll_h * 0.5f };

		

		fPoint dir_distance = GetShotDir() * distance;
		distance += increment;

		(*iter)->SetObjOffset(offset + dir_distance);
		(*iter)->SetPosToObj();

		(*iter)->SetIsTrigger(true);
	}
	electro_shot_timer.Start();
	is_electro_shot_charged = true;
}

bool Obj_Tank::GetIsElectroShotCharged() const
{
	return is_electro_shot_charged;
}

std::vector<Object*>* Obj_Tank::GetEnemiesHitted() 
{
	return &enemies_hitted;
}

void Obj_Tank::ReleaseFlameThrower()
{
	flame_emitter->StopEmission();
	coll_flame->SetIsTrigger(false);
}


