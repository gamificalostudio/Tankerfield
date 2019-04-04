
#include "Brofiler/Brofiler.h"
#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_timer.h"

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


M_SceneManager::M_SceneManager():Module()
{
	screen = { 0, 0, 1024,640 }; //revisar hardcode TODO
}

M_SceneManager::~M_SceneManager()
{}

// Load assets
bool M_SceneManager::Start()
{
	LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(app->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

// Update: draw background
bool M_SceneManager::Update(float dt)
{
	BROFILER_CATEGORY("M_SceneManagerUpdate", Profiler::Color::Purple)
	if (current_step == fade_step::none)
	{
		return true;
	}

	Uint32 now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch (current_step)
	{
	case fade_step::fade_to_black:
	{
		if (now >= total_time)
		{
			module_off->Disable();
			module_on->Enable();

			total_time += total_time;
			start_time = SDL_GetTicks();
			current_step = fade_step::fade_from_black;
		}
	} break;

	case fade_step::fade_from_black:
	{
		normalized = 1.0f - normalized;

		if (now >= total_time)
			current_step = fade_step::none;
	} break;
	}

	// Finally render the black square with alpha on the screen
	SDL_SetRenderDrawColor(app->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(app->render->renderer, &screen);
	
	return true;
}

// Fade to black. At mid point deactivate one module, then activate the other
bool M_SceneManager::FadeToBlack(Module* module_off, Module* module_on, float time)
{
	BROFILER_CATEGORY("M_SceneManagerFadeToBlack", Profiler::Color::Purple)
	bool ret = false;

	module_off = module_off;
	module_on = module_on;

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;
	}

	return ret;
}