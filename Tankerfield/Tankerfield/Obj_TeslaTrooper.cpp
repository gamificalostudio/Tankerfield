#include <list>

//#include "Brofiler/Brofiler.h"
#include "PugiXml\src\pugixml.hpp"
#include "Point.h"
#include "Log.h"

#include "App.h"
#include "Object.h"
#include "M_Textures.h"
#include "M_ObjManager.h"
#include "M_Render.h"
#include "M_Scene.h"
#include "M_Pathfinding.h"
#include "Obj_TeslaTrooper.h"
#include "M_Input.h"
#include "Animation.h"
//#include "j1Map.h"
//#include "j1Collision.h"

//Static variables inicialization
SDL_Texture * Obj_TeslaTrooper::tex = nullptr;

Obj_TeslaTrooper::Obj_TeslaTrooper(fPoint pos) : Object (pos)
{
	if (tex == nullptr)
	{
		tex = app->tex->Load("textures/Objects/shk-sheet.png");
	}
}

Obj_TeslaTrooper::~Obj_TeslaTrooper()
{
}

bool Obj_TeslaTrooper::PreUpdate()
{
	return true;
}

bool Obj_TeslaTrooper::Update(float dt)
{
	if(app->input->GetKey(SDL_SCANCODE_W)==KEY_DOWN)
	{
		angle += 45;
	}
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
	{
		angle -= 45;
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
	{
		life -= 100;
		LOG("life: %i", life);
	}

	if (life <= 0)
	{
		death = true;
		to_remove = true;
	}

	return true;
}






