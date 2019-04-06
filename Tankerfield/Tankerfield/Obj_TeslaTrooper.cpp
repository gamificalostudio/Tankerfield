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
Animation * Obj_TeslaTrooper::walk = nullptr;

Obj_TeslaTrooper::Obj_TeslaTrooper(fPoint pos) : Object (pos)
{}

bool Obj_TeslaTrooper::Start()
{
	pugi::xml_node tesla_trooper_node = app->config.child("object").child("tesla_trooper");

	if (tex == nullptr)
	{
		tex = app->tex->Load("textures/Objects/shk-sheet.png");
	}
	if (walk == nullptr)
	{
		walk = new Animation;
		walk->LoadFrames(tesla_trooper_node.child("animations").child("walk"));
	}
	curr_anim = walk;
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






