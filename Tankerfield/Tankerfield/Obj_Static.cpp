#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

#include "app.h"
#include "Obj_Static.h"
#include "M_Textures.h"
#include "M_Render.h"
#include "M_Collision.h"
#include "M_Input.h"
#include "Log.h"
#include "M_Map.h"
#include "M_ObjManager.h"

SDL_Texture * Obj_Static::garage = nullptr;

Obj_Static::Obj_Static() : Object()
{

}

Obj_Static::Obj_Static(fPoint pos) : Object(pos)
{
	
}

Obj_Static::~Obj_Static()
{
}

bool Obj_Static::Awake(pugi::xml_node & static_node)
{
	return true;
}

bool Obj_Static::Start()
{
	frame = { 324, 504, 203, 164 };
	draw_offset = { 105, 116 };

	if(garage == nullptr)
	{
		garage = app->tex->Load("Maps/Garage-Sheet.png"); //Load object texture
	}
	curr_tex = garage;

	coll = app->collision->AddCollider(pos_map, 4.f, 4.f, Collider::TAG::WALL, 0.f, this);
	coll->AddRigidBody(Collider::BODY_TYPE::STATIC);
	coll->SetObjOffset({ -2.f, -2.f });

	return true;
}
