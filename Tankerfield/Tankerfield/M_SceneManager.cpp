
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
#include "M_Options_Menu.h"
#include "MathUtils.h"
#include <math.h>


M_SceneManager::M_SceneManager():Module()
{

}

M_SceneManager::~M_SceneManager()
{}

// Load assets
bool M_SceneManager::Start()
{
	LOG("Preparing Fade Screen");
	screen = (SDL_Rect)app->win->GetWindowRect();
	SDL_SetRenderDrawBlendMode(app->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

// Update: draw background
bool M_SceneManager::PostUpdate(float dt)
{
	BROFILER_CATEGORY("M_SceneManagerPostUpdate", Profiler::Color::Purple)
	if (current_step == fade_step::none)
	{
		return true;
	}


	switch (current_step)
	{
	case fade_step::fade_to_black:

		if (balck_rect_alpha == target_value)
		{
			module_off->Disable();

			for (std::vector<Module*>::iterator iter = modules_to_reset.begin(); iter != modules_to_reset.end(); ++iter)
			{
				(*iter)->Reset();
			}
			module_off->Reset();
			module_on->Enable();
			current_step = fade_step::fade_from_black;
			std::swap(init_value, target_value);
			aux = 0.f;
			break;
		}
		break;

	case fade_step::fade_from_black:

		if (balck_rect_alpha == target_value)
		{
			current_step = fade_step::none;
			target_value = 255.f;
			init_value = 0.f;
			aux = 0.f;
		}
		break;

	}

	balck_rect_alpha = lerp(init_value, target_value, aux);

	if (dt > 0.16f)
	{
		dt = 0.16f;
	}

	if (current_step == fade_step::fade_to_black)
	{
		aux += dt * rate_time_fade_to;
	}
	else
	{
		aux += dt * rate_time_fade_from;
	}
	

	// Finally render the black square with alpha on the screen
	SDL_SetRenderDrawColor(app->render->renderer, 0, 0, 0, balck_rect_alpha);
	SDL_RenderFillRect(app->render->renderer, &screen);
	
	return true;
}

// Fade to black. At mid point deactivate one module, then activate the other
bool M_SceneManager::FadeToBlack(Module* module_off, Module* module_on, float seconds_fade_to, float seconds_fade_from, std::vector<Module*> modules_to_reset )
{
	BROFILER_CATEGORY("M_SceneManagerFadeToBlack", Profiler::Color::Purple)
	bool ret = false;

	if (current_step == fade_step::none)
	{
		this->module_off = module_off;
		this->module_on = module_on;
		this->modules_to_reset = modules_to_reset;
		this->rate_time_fade_to = 1.f / (seconds_fade_to);
		this->rate_time_fade_from = 1.f / (seconds_fade_from);

		current_step = fade_step::fade_to_black;
		ret = true;
	}

	return ret;
}