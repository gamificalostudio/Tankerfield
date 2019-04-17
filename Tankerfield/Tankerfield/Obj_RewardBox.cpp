

#include "Brofiler/Brofiler.h"
#include "PugiXml\src\pugixml.hpp"

#include "Obj_RewardBox.h"
#include "App.h"
#include "M_Render.h"
#include "M_Collision.h"
#include "M_PickManager.h"



Obj_RewardBox::Obj_RewardBox(fPoint pos) : Object(pos)
{
	coll = app->collision->AddCollider(pos, 2, 2, Collider::TAG::REWARD_BOX, 0.f, this);
	coll->AddRigidBody(Collider::BODY_TYPE::STATIC);

	life = 100;
}

Obj_RewardBox::~Obj_RewardBox()
{
}

void Obj_RewardBox::OnTrigger(Collider * collider)
{
	if (collider->GetTag() == Collider::TAG::BULLET)
	{
		GetDamage(collider->damage);
	}

}

bool Obj_RewardBox::Draw(float dt, Camera* camera)
{
	app->render->DrawIsometricQuad(pos_map.x, pos_map.y, 2, 2);

	return true;
}

void Obj_RewardBox::GetDamage(float damage)
{
	if (life - damage < 0)
	{
		life = 0;
		Dead();
	}
	else
	{
		life -= damage;
	}
}

void Obj_RewardBox::Dead()
{
	app->pick_manager->CreatePickUp(pos_map);
	to_remove = true;
}

