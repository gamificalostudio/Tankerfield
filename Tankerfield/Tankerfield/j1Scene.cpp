#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "j1Map.h"

j1Scene::j1Scene() : j1Module()
{
	name.assign("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	// Load the first level of the list on first game start -------------------------
	std::list<Levels*>::iterator levelData = App->map->levels.begin();
	std::advance(levelData, current_level);
	App->map->Load((*levelData)->name.c_str());

	
	
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	if (App->input->controllers.size())
	{
		control1 = &(*App->input->controllers.begin());
	}


	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= floor(200.0f * dt);

	if (control1 && (*control1))
	{
		if ((*control1)->Get_Axis(SDL_CONTROLLER_AXIS_LEFTX) < -10000)
			cube.x -= 1;
		if ((*control1)->Get_Axis(SDL_CONTROLLER_AXIS_LEFTX) > 10000)
			cube.x += 1;

		if ((*control1)->Get_Axis(SDL_CONTROLLER_AXIS_LEFTY) < -10000)
			cube.y -= 1;
		if ((*control1)->Get_Axis(SDL_CONTROLLER_AXIS_LEFTY) > 10000)
			cube.y += 1;

		if ((*control1)->Get_Axis(SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 1000)
		{
			//SDL_HapticRumblePlay((*control1)->haptic, 1, 500);
			(*control1)->PlayRumble(0.5, 500);

			LOG("%s",SDL_GetError());
		}
	}
	// Draw ------------------
	

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;
	App->render->DrawQuad(cube, 255, 0, 0, 255);
	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");



	return true;
}
