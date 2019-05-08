#include "Bullet_FlameThrower.h"
#include "PerfTimer.h"
#include "App.h"
#include "M_Collision.h"
#include "M_ObjManager.h"
#include "Obj_FlameThrowerFlame.h"
#include "Obj_Tank.h"
#include "M_Textures.h"
#include "M_AnimationBank.h"

FlameThrower_Bullet::FlameThrower_Bullet(fPoint pos) : Obj_Bullet(pos)
{
}

FlameThrower_Bullet::~FlameThrower_Bullet()
{
}

void FlameThrower_Bullet::OnTrigger(Collider * collider_1)
{
	if (player != nullptr)
	{
		Obj_FlamethrowerFlame* flame_obj = (Obj_FlamethrowerFlame*)app->objectmanager->CreateObject(ObjectType::FLAMETHROWER_FLAME, pos_map);
		//explosion_obj->SetExplosionDamage(player->GetWeaponInfo());
	}
	to_remove = true;
}



