#include "p2Defs.h"
#include "p2Log.h"
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

M_Scene::M_Scene() : Module()
{
	name.assign("scene");
}

// Destructor
M_Scene::~M_Scene()
{}

// Called before render is available
bool M_Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool M_Scene::Start()
{
	// Load the first level of the list on first game start -------------------------
	std::list<Levels*>::iterator levelData = App->map->levels.begin();
	std::advance(levelData, current_level);
	App->map->Load((*levelData)->name.c_str());

	App->objectmanager->CreateObject(ObjectType::TANK, 0, 0);
	
	return true;
}

// Called each loop iteration
bool M_Scene::PreUpdate()
{
	if (App->input->controllers.size())
	{
		control1 = &(*App->input->controllers.begin());
	}


	return true;
}

// Called each loop iteration
bool M_Scene::Update(float dt)
{
	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= floor(200.0f * dt);

	return true;
}

// Called each loop iteration
bool M_Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;
	
	return ret;
}

// Called before quitting
bool M_Scene::CleanUp()
{
	LOG("Freeing scene");



	return true;
}
