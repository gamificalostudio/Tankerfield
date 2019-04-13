

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
//	pugi::xml_node tesla_trooper_node = app->config.child("object").child("tesla_trooper");

//	if (tex == nullptr)
//	{
//		tex = app->tex->Load("textures/Objects/shk-sheet.png");
//	}
//	curr_tex = tex;

//	speed = 1.5F;


	//coll = app->collision->AddCollider(pos, 0.5f, 0.5f, Collider::TAG::ENEMY, 0.f, this);
//	coll->AddRigidBody(Collider::BODY_TYPE::DYNAMIC);
//	draw_offset = { 32, 38 };
//	coll->SetObjOffset({ -.25f, -.25f });
//	timer.Start();
	frame.x = pos_map.x;
	frame.y = pos_map.y;
	frame.w = 100;
	frame.h = 100;

	//number 0 == no_type, rand() can be 0 or 1, we need to + 1 to select between item == 1 or weapon == 2
	type_of_pick_up = RandomPickUp();
	

	coll = app->collision->AddCollider(pos, 1, 1, Collider::TAG::PICK_UP, 0.f, this);
	coll->AddRigidBody(Collider::BODY_TYPE::SENSOR);

	
}

Obj_PickUp::~Obj_PickUp()
{
}

bool Obj_PickUp::Update(float dt)
{	

	return true;
}


void Obj_PickUp::OnTrigger(Collider* collider)
{
	if (collider->GetTag() == Collider::TAG::PLAYER)
	{
		Obj_Tank* player = collider->GetObj<Obj_Tank>();
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			if (type_of_pick_up == PICKUP_TYPE::ITEM)
			{
				player->SetItem(type_of_item);
			}
			else
			{
				player->SetWeapon(type_of_weapon);
			}
		}
	}
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
	return type = (WEAPON)(rand() % 2/*number of weapons*/);
}

ObjectType Obj_PickUp::RandomItem() const
{
	ObjectType type;
	//The first number equal to basic shoot, and it won't be de basic never. we need to + 1.
	return type = (ObjectType)(rand() % 1 /*number of items*/);
}







//elements.insert(std::pair <ObjectType, Object*>(type_of_pick_up, cutscene_element));