#include "Bullet_Missile.h"
#include "PerfTimer.h"
#include "App.h"
#include "M_Audio.h"
#include "M_Collision.h"
#include "M_ObjManager.h"
#include "Obj_Explosion.h"
#include "Obj_Tank.h"

Bullet_Missile::Bullet_Missile(fPoint pos) : Obj_Bullet(pos)
{
	shot_sound = app->audio->LoadFx("audio/Fx/tank/weapons/double_missile.wav", 128);
	app->audio->PlayFx(shot_sound);
}

void Bullet_Missile::OnTriggerEnter(Collider * collider_1)
{
	if (player != nullptr)
	{
		Obj_Explosion* explosion_obj = (Obj_Explosion*)app->objectmanager->CreateObject(ObjectType::EXPLOSION, pos_map);
		explosion_obj->SetExplosionDamage(explosion_damage);
	}
	return_to_pool = true;
}

bool Bullet_Missile::Update(float dt)
{
	pos_map += direction * speed * dt;

	if (bullet_life_ms_timer.ReadMs() >= bullet_life_ms)
	{
		Obj_Explosion* explosion_obj = (Obj_Explosion*)app->objectmanager->CreateObject(ObjectType::EXPLOSION, pos_map);
		explosion_obj->SetExplosionDamage(explosion_damage);

		return_to_pool = true;
	}


	return true;
}



