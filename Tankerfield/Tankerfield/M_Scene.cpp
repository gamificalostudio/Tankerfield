#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "M_Input.h"
#include "M_Textures.h"
#include "M_Audio.h"
#include "M_Render.h"
#include "M_Window.h"
#include "M_Scene.h"
#include "M_SceneManager.h"
#include "M_Map.h"
#include "M_ObjManager.h"
#include "M_Collision.h"
#include "Point.h"
#include "Brofiler/Brofiler.h"

M_Scene::M_Scene() : Module()
{
	name.assign("scene");
}

// Destructor
M_Scene::~M_Scene()
{}

// Called before render is available
bool M_Scene::Awake(pugi::xml_node&)
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool M_Scene::Start()
{
	// Load the first level of the list on first game start -------------------------
	std::list<Levels*>::iterator levelData = app->map->levels.begin();
	std::advance(levelData, current_level);
	app->map->Load((*levelData)->name.c_str());

	tank_1 = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(20.f, 20.f));

	app->objectmanager->CreateObject(ObjectType::TESLA_TROOPER, fPoint(-10.f, -10.f));


	return true;
}

// Called each loop iteration
bool M_Scene::PreUpdate()
{
	if (app->input->controllers.size())
	{
		control1 = &(*app->input->controllers.begin());
	}
	return true;
}

// Called each loop iteration
bool M_Scene::Update(float dt)
{
	BROFILER_CATEGORY("M_SceneUpdate", Profiler::Color::Blue)
	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		app->render->camera.y -= floor(200.0f * dt);
	}
	if(app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		app->render->camera.y += floor(200.0f * dt);
	}
	if(app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		app->render->camera.x -= floor(200.0f * dt);
	}
	if(app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		app->render->camera.x += floor(200.0f * dt);
	}

	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KeyState::KEY_DOWN)
	{
		++current_level;

		if (current_level == app->map->GetMaxLevels())
			current_level = 0;

		app->scmanager->FadeToBlack(app->scene, app->scene, 1.F);
	}
	return true;
}

// Called each loop iteration
bool M_Scene::PostUpdate(float dt)
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;
	
	return ret;
}

// Called before quitting
bool M_Scene::CleanUp()
{
	LOG("Freeing scene");
	app->map->Unload();
	app->objectmanager->DeleteObjects();

	return true;
}
