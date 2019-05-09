

#include "Brofiler/Brofiler.h"
#include "PugiXml\src\pugixml.hpp"

#include "Obj_PickUp.h"
#include "App.h"
#include "M_Render.h"
#include "M_Map.h"
#include "M_Collision.h"
#include "Obj_Tank.h"
#include "M_Scene.h"

#include "M_UI.h"
#include "UI_InGameElement.h"


Obj_PickUp::Obj_PickUp(fPoint pos) : Object(pos)
{
	coll = app->collision->AddCollider(pos, 1, 1, Collider::TAG::PICK_UP, 0.f, this);
	coll->AddRigidBody(Collider::BODY_TYPE::SENSOR);
	type_of_pick_up = RandomPickUp();

	frame.w = 1;
	frame.h = 1;
	
}

Obj_PickUp::~Obj_PickUp()
{
}

void Obj_PickUp::GenerationOfPickUp(PICKUP_TYPE type_of_pick_up, uint levels_to_add)
{

	UI_InGameElementDef element_def;
	element_def.pointed_obj = this;


	if (type_of_pick_up != PICKUP_TYPE::NO_TYPE)
	{
		this->type_of_pick_up = type_of_pick_up;
	}

	switch (this->type_of_pick_up)
	{
	case PICKUP_TYPE::ITEM:
		type_of_item = RandomItem();
		in_game_element = app->ui->CreateInGameItem(pos_map, element_def);
		break;
	case PICKUP_TYPE::WEAPON:
		type_of_weapon = RandomWeapon();
		level_of_weapon += levels_to_add;
		in_game_element = app->ui->CreateInGameWeapon(pos_map, element_def);
		break;
	}
	
}

bool Obj_PickUp::Update(float dt)
{	

	return true;
}

bool Obj_PickUp::Draw(float dt, Camera* camera)
{
	app->render->DrawIsometricQuad(pos_map.x, pos_map.y, 1, 1, {255,255,255,255}, camera);

	return true;
}

PICKUP_TYPE Obj_PickUp::RandomPickUp() const
{
	return (PICKUP_TYPE)(rand() % (uint)PICKUP_TYPE::MAX_TYPES);
}

WEAPON Obj_PickUp::RandomWeapon() 
{
	level_of_weapon = app->scene->round;
	return (WEAPON)(rand() % ((uint)WEAPON::MAX_WEAPONS - 1)+1 /*The plus 1 is because the basic shoot is the number 0, and it can't be created, and the -1 is because the max weapon include the basic bullet*/);
}

ItemType Obj_PickUp::RandomItem() const
{
	return (ItemType)(rand() % (uint)ItemType::MAX_ITEMS);
}

void Obj_PickUp::DeletePickUp()
{
	if (in_game_element != nullptr)
	{
		in_game_element->Destroy();
	}
	to_remove = true;
}