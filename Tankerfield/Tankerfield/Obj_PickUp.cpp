

#include "Brofiler/Brofiler.h"
#include "PugiXml\src\pugixml.hpp"

#include "Obj_PickUp.h"
#include "App.h"
#include "M_Render.h"
#include "M_Map.h"
#include "M_Collision.h"
#include "Obj_Tank.h"


Obj_PickUp::Obj_PickUp(fPoint pos) : Object(pos)
{
	coll = app->collision->AddCollider(pos, 1, 1, Collider::TAG::PICK_UP, 0.f, this);
	coll->AddRigidBody(Collider::BODY_TYPE::SENSOR);
	
	type_of_pick_up = RandomPickUp();

	if (type_of_pick_up == PICKUP_TYPE::ITEM)
	{
		type_of_item = RandomItem();
	}
	else if (type_of_pick_up == PICKUP_TYPE::WEAPON)
	{
		type_of_weapon = RandomWeapon();
	}
}

Obj_PickUp::~Obj_PickUp()
{
}

bool Obj_PickUp::Update(float dt)
{	

	return true;
}

bool Obj_PickUp::Draw(float dt, Camera* camera)
{
	app->render->DrawIsometricQuad(pos_map.x, pos_map.y, 1,1);

	return true;
}

PICKUP_TYPE Obj_PickUp::RandomPickUp() 
{
	return (PICKUP_TYPE)(rand() % 2);
}

WEAPON Obj_PickUp::RandomWeapon() const
{
	return (WEAPON)(rand() % 1/*number of weapons*/);
}

ObjectType Obj_PickUp::RandomItem() const
{
	return (ObjectType)(rand() % 1 /*number of items*/);
}

void Obj_PickUp::DeletePickUp()
{
	to_remove = true;
}