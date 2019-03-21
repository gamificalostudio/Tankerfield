#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "SceneTest2.h"
#include "SceneManager.h"

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
	LOG("Scene1");
	
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{


	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
 	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		App->scmanager->FadeToBlack(App->scene, App->test2, 0.5f);
		LOG("Scene1 off");
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

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");



	return true;
}
