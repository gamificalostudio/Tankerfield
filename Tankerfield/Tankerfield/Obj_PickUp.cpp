

#include "Brofiler/Brofiler.h"
#include "PugiXml\src\pugixml.hpp"

#include "Obj_PickUp.h"
#include "App.h"
#include "M_Render.h"
#include "M_Map.h"
#include "M_Collision.h"
#include "Obj_Tank.h"

#include "M_UI.h"
#include "UI_InGameElement.h"


Obj_PickUp::Obj_PickUp(fPoint pos) : Object(pos)
{
	coll = app->collision->AddCollider(pos, 1, 1, Collider::TAG::PICK_UP, 0.f, this);
	coll->AddRigidBody(Collider::BODY_TYPE::SENSOR);
	type_of_pick_up = RandomPickUp();

	UI_InGameElementDef element_def;
	element_def.pointed_obj = this;

	switch (type_of_pick_up)
	{
	case PICKUP_TYPE::NO_TYPE:
		break;
	case PICKUP_TYPE::ITEM:
		in_game_element = app->ui->CreateInGameItem({ 0.f,0.f} , element_def);
		break;
	case PICKUP_TYPE::WEAPON:
		in_game_element = app->ui->CreateInGameWeapon({ 0.f,0.f }, element_def);
		break;
	}
}

Obj_PickUp::~Obj_PickUp()
{
}

bool Obj_PickUp::Update(float dt)
{	

	return true;
}

//Lo esta haciendo el player ahora
//
void Obj_PickUp::OnTrigger(Collider* collider)
{
	//if (collider->GetTag() == Collider::TAG::PLAYER)
	//{
 //		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN /*|| app->input->GetKey(gamepad_interact) == KEY_DOWN*/)
	//	{
	//		Obj_Tank* player = (Obj_Tank*)collider->GetObj();
	//		player->SetPickUp(this);
	//	}
	//}
}

bool Obj_PickUp::Draw(float dt, Camera* camera)
{
	app->render->DrawIsometricQuad(pos_map.x, pos_map.y, 1,1);

	return true;
}

PICKUP_TYPE Obj_PickUp::RandomPickUp() 
{
	PICKUP_TYPE type;

	type_of_weapon = RandomWeapon();
	type_of_item = RandomItem();

	return type = (PICKUP_TYPE)(rand() % 2);
}

WEAPON Obj_PickUp::RandomWeapon() const
{
	WEAPON type;
	return type = (WEAPON)(rand() % 1/*number of weapons*/);
}

ObjectType Obj_PickUp::RandomItem() const
{
	ObjectType type;
	return type = (ObjectType)(rand() % 2 /*number of items*/);
}

void Obj_PickUp::DeletePickUp()
{
	in_game_element->Destroy();
	to_remove = true;
}

//Lo esta haciendo el player
//void Obj_PickUp::SetPickUp(Obj_Tank* player)
//{
//	if (type_of_pick_up == PICKUP_TYPE::ITEM)
//	{
//		player->SetItem(type_of_item);
//	}
//	else
//	{
//		player->SetWeapon(type_of_weapon);
//	}
//
//	DeletePickUp();
//}
