#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "SceneTest2.h"
#include "j1Scene.h"
#include "SceneManager.h"


SceneTest2::SceneTest2() : j1Module()
{
	name.assign("scene2");
}

// Destructor
SceneTest2::~SceneTest2()
{}

// Called before render is available
bool SceneTest2::Awake()
{
	
	bool ret = true;

	return ret;
}

// Called before the first frame
bool SceneTest2::Start()
{
	LOG("Scene2");

	return true;
}

// Called each loop iteration
bool SceneTest2::PreUpdate()
{


	return true;
}

// Called each loop iteration
bool SceneTest2::Update(float dt)
{

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		App->scmanager->FadeToBlack(App->test2, App->scene, 0.5f);
		LOG("Scene2 off");
	}

	// Draw ------------------


	return true;
}

// Called each loop iteration
bool SceneTest2::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool SceneTest2::CleanUp()
{
	LOG("Freeing scene");



	return true;
}
