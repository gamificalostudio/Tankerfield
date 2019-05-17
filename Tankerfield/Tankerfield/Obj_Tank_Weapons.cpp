//File to keep all the shot functions of the different weapons
#include "Obj_Tank.h"
#include "App.h"
#include "M_ObjManager.h"
#include "Player_GUI.h"
#include "M_Collision.h"
#include "M_AnimationBank.h"
#include "M_Textures.h"
#include "M_Map.h"

//Bullets
#include "Bullet_Missile.h"
#include "Healing_Bullet.h"
#include "Bullet_Laser.h"
#include "ElectroShotAnimation.h"


void Obj_Tank::InitWeapons()
{
	//Basic weapon starting properties
	SetWeapon(WEAPON::BASIC, 1u);

	shot1_function[(uint)WEAPON::BASIC] = &Obj_Tank::ShootBasic;
	shot1_function[(uint)WEAPON::DOUBLE_MISSILE] = &Obj_Tank::ShootDoubleMissile;
	shot1_function[(uint)WEAPON::HEALING_SHOT] = &Obj_Tank::ShootHealingShot;
	shot1_function[(uint)WEAPON::LASER_SHOT] = &Obj_Tank::ShootLaserShot;
	shot1_function[(uint)WEAPON::ELECTRO_SHOT] = &Obj_Tank::ShootElectroShot;
	shot1_function[(uint)WEAPON::FLAMETHROWER] = &Obj_Tank::ShootFlameThrower;

	//Electro_shot--
	pugi::xml_node electro_shot_node = app->config.child("object").child("tank").child("electro_shot");

	float coll_size_init = electro_shot_node.child("collider_size").attribute("value").as_float();

	//Basic electro shot colliders
	for (uint i = 1; i <= 3/*num max of colliders*/; ++i)
	{
		float coll_size = coll_size_init * i;
		Collider* electro_shot_collider = nullptr;

		electro_shot_collider = app->collision->AddCollider(pos_map, coll_size, coll_size, TAG::ELECTRO_SHOT, BODY_TYPE::DYNAMIC, 100, this);
		electro_shot_collider->is_sensor = true;
		//electro_shot_collider->Disactivate();
		electro_shot_collider->ActiveOnTrigger(false);

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
		electro_shot_collider_charged->ActiveOnTrigger(false);

		electric_shot_colliders_charged_vector.push_back(electro_shot_collider_charged);
	}
  
	charge_time = 2500.f; // Same for all bullets (player gets used to it)
	quick_shot_time = 500.f;
	shot2_function[(uint)WEAPON::BASIC] = &Obj_Tank::ShootBasic;
	shot2_function[(uint)WEAPON::DOUBLE_MISSILE] = &Obj_Tank::ShootDoubleMissileCharged;
	shot2_function[(uint)WEAPON::HEALING_SHOT] = &Obj_Tank::ShootHealingShot;
	shot2_function[(uint)WEAPON::LASER_SHOT] = &Obj_Tank::ShootLaserShotCharged;
	shot2_function[(uint)WEAPON::ELECTRO_SHOT] = &Obj_Tank::ShootElectroShotCharged;
	shot2_function[(uint)WEAPON::FLAMETHROWER] = &Obj_Tank::ShootFlameThrower;
}

void Obj_Tank::UpdateWeaponsWithoutBullets(float dt)
{
	if (weapon_info.weapon == WEAPON::ELECTRO_SHOT)
	{
		//with the animation get frame?? only 1 frame
		if (electro_shot_timer.ReadMs() >= weapon_info.shot1.bullet_life_ms && (*electric_shot_colliders_vector.begin())->GetIsActivated())
		{
			for (std::vector<Collider*>::iterator iter = electric_shot_colliders_vector.begin(); iter != electric_shot_colliders_vector.end(); ++iter)
			{
				(*iter)->ActiveOnTrigger(false);
			}
		}
		else if (electro_shot_timer.ReadMs() >= weapon_info.shot1.bullet_life_ms && (*electric_shot_colliders_charged_vector.begin())->GetIsActivated())
		{
			for (std::vector<Collider*>::iterator iter = electric_shot_colliders_charged_vector.begin(); iter != electric_shot_colliders_charged_vector.end(); ++iter)
			{
				(*iter)->ActiveOnTrigger(false);
			}
		}

		if (hit_no_enemie)
		{
			hit_no_enemie = false;
			Eletro_Shot_Animation* electro_anim = (Eletro_Shot_Animation*)app->objectmanager->CreateObject(ObjectType::ELECTRO_SHOT_ANIMATION, pos_map);
			electro_anim->draw_offset -= (iPoint)app->map->MapToScreenF(GetShotDir());
			electro_anim->distance = 25.f;
			electro_anim->player_enemy_distance_point = app->map->MapToScreenF(GetShotDir());
		}

	}

	if (weapon_info.weapon == WEAPON::FLAMETHROWER)
	{
		//with the animation get frame?? only 1 frame
		if (flame_release_time.ReadSec() >= 1.f && coll_flame->GetIsActivated())
		{
				coll_flame->ActiveOnTrigger(false);
		}

	}


	//test:

	//float coll_w_init;
	//float coll_h_init;
	//(*electric_shot_colliders_vector.begin())->GetSize(coll_w_init, coll_h_init);
	//fPoint increment{ coll_w_init, coll_h_init};
	//fPoint distance{0,0};
	//

	//for (std::vector<Collider*>::iterator iter = electric_shot_colliders_vector.begin(); iter != electric_shot_colliders_vector.end(); ++iter)
	//{
	//	float coll_w;
	//	float coll_h;
	//	(*iter)->GetSize(coll_w, coll_h);
	//	//fPoint increment{ coll_w*0.5f, coll_h*0.5f };  alternative

	//	fPoint offset{ -coll_w * 0.5f, -coll_h * 0.5f };

	//	distance += increment;

	//	fPoint dir_distance = GetShotDir() * distance;


	//	(*iter)->SetObjOffset(offset + dir_distance);
	//	(*iter)->SetPosToObj();

	//	//(*iter)->Activate();
	//}

}

void Obj_Tank::SetWeapon(WEAPON type, uint level)
{
	weapon_info.level_weapon = level;
	weapon_info.weapon = type;

	gui->SetWeaponIcon(type);

	switch (type)
	{
	case WEAPON::BASIC:
		weapon_info.type = WEAPON_TYPE::CHARGED;
		weapon_info.shot1.bullet_damage = 25 + level * 2;
		weapon_info.shot1.explosion_damage = 0;
		weapon_info.shot1.bullet_healing = 0;
		weapon_info.shot1.bullet_life_ms = 2000;
		weapon_info.shot1.bullet_speed = 10;
		weapon_info.shot1.time_between_bullets = 500;
		weapon_info.shot1.trauma = 0.54f;
		weapon_info.shot2.trauma = 0.76f;
		weapon_info.shot1.rumble_strength = 0.92f;
		weapon_info.shot1.rumble_duration = 250;
		weapon_info.shot2.rumble_strength = 1.0f;
		weapon_info.shot2.rumble_duration = 400;
		weapon_info.shot1.smoke_particle = ObjectType::CANNON_FIRE;
		weapon_info.shot2.smoke_particle = ObjectType::CANNON_FIRE;
		break;
	case WEAPON::FLAMETHROWER:
		weapon_info.type = WEAPON_TYPE::SUSTAINED;
		weapon_info.shot1.bullet_damage = 50 + level * 2;
		weapon_info.shot1.explosion_damage = 0;
		weapon_info.shot1.bullet_healing = 0;
		weapon_info.shot1.bullet_life_ms = 2000;
		weapon_info.shot1.bullet_speed = 10;
		weapon_info.shot1.time_between_bullets = 500;
		weapon_info.shot1.trauma = 0.54f;
		weapon_info.shot2.trauma = 0.76f;
		weapon_info.shot1.rumble_strength = 0.92f;
		weapon_info.shot1.rumble_duration = 250;
		weapon_info.shot2.rumble_strength = 1.0f;
		weapon_info.shot2.rumble_duration = 400;
		weapon_info.shot1.smoke_particle = ObjectType::CANNON_FIRE;
		weapon_info.shot2.smoke_particle = ObjectType::CANNON_FIRE;
		break;
	case WEAPON::DOUBLE_MISSILE:
		weapon_info.type = WEAPON_TYPE::CHARGED;
		weapon_info.shot1.bullet_damage = 0;
		weapon_info.shot1.explosion_damage = level * 100;
		weapon_info.shot1.bullet_healing = 0;
		weapon_info.shot1.bullet_life_ms = 2000;
		weapon_info.shot1.bullet_speed = 10;
		weapon_info.shot1.time_between_bullets = 500;
		weapon_info.shot1.trauma = 0.54f;
		weapon_info.shot2.trauma = 0.76f;
		weapon_info.shot1.rumble_strength = 0.92f;
		weapon_info.shot1.rumble_duration = 250;
		weapon_info.shot2.rumble_strength = 1.0f;
		weapon_info.shot2.rumble_duration = 400;
		weapon_info.shot1.smoke_particle = ObjectType::CANNON_FIRE;
		weapon_info.shot2.smoke_particle = ObjectType::CANNON_FIRE;
		break;
	case WEAPON::HEALING_SHOT:
		weapon_info.type = WEAPON_TYPE::CHARGED;
		weapon_info.shot1.bullet_damage = 25 + level;
		weapon_info.shot1.explosion_damage = 0;
		weapon_info.shot1.bullet_healing = 5 + level;
		weapon_info.shot1.bullet_life_ms = 2000;
		weapon_info.shot1.bullet_speed = 10;
		weapon_info.shot1.time_between_bullets = 500;
		weapon_info.shot1.trauma = 0.54f;
		weapon_info.shot2.trauma = 0.76f;
		weapon_info.shot1.rumble_strength = 0.92f;
		weapon_info.shot1.rumble_duration = 250;
		weapon_info.shot2.rumble_strength = 1.0f;
		weapon_info.shot2.rumble_duration = 400;
		weapon_info.shot1.smoke_particle = ObjectType::CANNON_FIRE;
		weapon_info.shot2.smoke_particle = ObjectType::CANNON_FIRE;
		break;
	case WEAPON::LASER_SHOT:
		weapon_info.type = WEAPON_TYPE::CHARGED;
		weapon_info.shot1.bullet_damage = 10 + level * 2;
		weapon_info.shot1.explosion_damage = 0;
		weapon_info.shot1.bullet_healing = 0;
		weapon_info.shot1.bullet_life_ms = 2000;
		weapon_info.shot1.bullet_speed = 20;
		weapon_info.shot1.time_between_bullets = 500;
		weapon_info.shot1.trauma = 0.405f;
		weapon_info.shot2.trauma = 0.57f;
		weapon_info.shot1.rumble_strength = 0.92f;
		weapon_info.shot1.rumble_duration = 250;
		weapon_info.shot2.rumble_strength = 1.0f;
		weapon_info.shot2.rumble_duration = 400;
		weapon_info.shot1.smoke_particle = ObjectType::CANNON_FIRE;
		weapon_info.shot2.smoke_particle = ObjectType::CANNON_FIRE;
		break;
	case WEAPON::ELECTRO_SHOT:
		weapon_info.type = WEAPON_TYPE::CHARGED;
		weapon_info.shot1.bullet_damage = 100 + level * 2;
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
		weapon_info.shot1.smoke_particle = ObjectType::NO_TYPE;
		weapon_info.shot2.smoke_particle = ObjectType::NO_TYPE;
		//electro_shot_collider->damage = weapon_info.bullet_damage;
		//add with and height here?
		break;
	
	}
}

void Obj_Tank::SetColor(const SDL_Color new_color)
{
	tank_color = new_color;
}

void Obj_Tank::ShootBasic()
{
	Obj_Bullet * bullet = (Obj_Bullet*)app->objectmanager->CreateObject(ObjectType::BASIC_BULLET, turr_pos);
	bullet->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
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
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		bullet_angle);
	missile_ptr->explosion_damage = weapon_info.shot1.explosion_damage;

	missile_ptr = (Bullet_Missile*)app->objectmanager->CreateObject(ObjectType::BULLET_MISSILE, turr_pos - double_missiles_offset * missiles_offset);
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

	missile_ptr = (Bullet_Missile*)app->objectmanager->CreateObject(ObjectType::BULLET_MISSILE, turr_pos + double_missiles_offset * missiles_offset);
	missile_ptr->SetPlayer(this);
	missile_ptr->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		bullet_angle);
	missile_ptr->explosion_damage = weapon_info.shot1.explosion_damage;

	missile_ptr = (Bullet_Missile*)app->objectmanager->CreateObject(ObjectType::BULLET_MISSILE, turr_pos - double_missiles_offset * missiles_offset);
	missile_ptr->SetPlayer(this);
	missile_ptr->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		bullet_angle);
	missile_ptr->explosion_damage = weapon_info.shot1.explosion_damage;

	missile_ptr = (Bullet_Missile*)app->objectmanager->CreateObject(ObjectType::BULLET_MISSILE, turr_pos + double_missiles_offset * missiles_offset * 3 - shot_dir * 1.5);
	missile_ptr->SetPlayer(this);
	missile_ptr->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		bullet_angle);
	missile_ptr->explosion_damage = weapon_info.shot1.explosion_damage;

	missile_ptr = (Bullet_Missile*)app->objectmanager->CreateObject(ObjectType::BULLET_MISSILE, turr_pos - double_missiles_offset * missiles_offset * 3 - shot_dir * 1.5);
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
	Healing_Bullet * heal_bullet = (Healing_Bullet*)app->objectmanager->CreateObject(ObjectType::HEALING_BULLET, turr_pos + shot_dir);

	heal_bullet->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45);

	heal_bullet->tank_parent = this;
}

void Obj_Tank::ShootLaserShot()
{

	Laser_Bullet *	 laser_bullet = (Laser_Bullet*)app->objectmanager->CreateObject(ObjectType::BULLET_LASER, turr_pos + shot_dir);

	laser_bullet->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45);
}

void Obj_Tank::ShootLaserShotCharged()
{

	Laser_Bullet *	 laser_bullet = (Laser_Bullet*)app->objectmanager->CreateObject(ObjectType::BULLET_LASER, turr_pos + shot_dir);

	laser_bullet->SetBulletProperties(
		weapon_info.shot1.bullet_speed,
		weapon_info.shot1.bullet_life_ms,
		weapon_info.shot1.bullet_damage,
		shot_dir,
		atan2(-shot_dir.y, shot_dir.x) * RADTODEG - 45,
		true);
}

void Obj_Tank::ShootFlameThrower()
{
	flame_release_time.Start();

	if(coll_flame->GetIsActivated() == false)
	{
		coll_flame->ActiveOnTrigger(true);
	}
	
	


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

		(*iter)->ActiveOnTrigger(true);
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

		(*iter)->ActiveOnTrigger(true);
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


