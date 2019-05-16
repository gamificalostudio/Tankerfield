#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

#include "app.h"
#include "Obj_Building.h"
#include "M_Textures.h"
#include "M_Render.h"
#include "M_Collision.h"
#include "M_Input.h"
#include "Log.h"
#include "M_Map.h"
#include "M_ObjManager.h"

SDL_Texture * Obj_Building::texture = nullptr;


Obj_Building::Obj_Building(fPoint pos) : Object(pos)
{
	
}

Obj_Building::~Obj_Building()
{
}


void Obj_Building::SetTexture(const char* path, fPoint rect_collider)
{
	texture = app->tex->Load(path);
	curr_tex = texture;
	frame.x = 0;
	frame.y = 0;
	
	SDL_QueryTexture(texture, NULL, NULL, &frame.w, &frame.h);

	coll = app->collision->AddCollider(pos_map, rect_collider.x, rect_collider.y, Collider::TAG::WALL, 0.f, this);
	coll->AddRigidBody(Collider::BODY_TYPE::STATIC);
	
}

