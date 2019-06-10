

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
	coll = app->collision->AddCollider(pos, 1.2F, 1.2F, TAG::PICK_UP, BODY_TYPE::DYNAMIC, 0.f, this);
	coll->is_sensor = true;
	coll->SetObjOffset(fPoint( -0.6F, -0.6F));

	coll_corrector = app->collision->AddCollider(pos, 2.F, 2.F, TAG::PICK_UP, BODY_TYPE::DYNAMIC, 0.f, this);
	coll_corrector->SetIsTrigger(false);
	coll_corrector->SetObjOffset(fPoint(-1.F, -1.F));

	frame.w = 1;
	frame.h = 1;
	Timer_to_delete.Start();
}

Obj_PickUp::~Obj_PickUp()
{
}

void Obj_PickUp::CreatePickUpUI()
{
	UI_InGameElementDef element_def;
	element_def.pointed_obj = this;

	switch (type_of_pick_up)
	{
	case PICKUP_TYPE::ITEM:
		in_game_element = app->ui->CreateInGameItem(pos_map, element_def);
		break;
	case PICKUP_TYPE::WEAPON:
		in_game_element = app->ui->CreateInGameWeapon(pos_map, element_def);
		break;
	default:
		LOG("Pick up type not set correctly.");
		break;
	}
}

bool Obj_PickUp::Update(float dt)
{	
	if (Timer_to_delete.ReadSec() >= 30)
	{
		for (uint i = 0; i < 4; ++i)
		{
			app->objectmanager->obj_tanks[i]->tutorial_pick_up->SetStateToBranch(ELEMENT_STATE::HIDDEN);
		}
		DeletePickUp();
	}
	
	if (coll_corrector != nullptr)
	{
		coll_corrector->SetPosToObj();
	}

	return true;
}

bool Obj_PickUp::Draw(float dt, Camera* camera)
{
	app->render->DrawIsometricQuad(pos_map.x, pos_map.y, 1, 1, {255,255,255,255}, camera);

	return true;
}

void Obj_PickUp::DeletePickUp()
{
	if (in_game_element != nullptr)
	{
		in_game_element->Destroy();
	}
	to_remove = true;

	if (coll != nullptr)
	{
		coll->Destroy();
		coll = nullptr;
	}

	if (coll_corrector != nullptr)
	{
		coll_corrector->Destroy();
		coll_corrector = nullptr;
	}

}