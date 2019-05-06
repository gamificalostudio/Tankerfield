
#include "Brofiler/Brofiler.h"
#include "SDL/include/SDL.h"

#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "M_Window.h"

M_Window::M_Window() : Module()
{
	window = NULL;
	screen_surface = NULL;
	name.assign("window");
}

// Destructor
M_Window::~M_Window()
{
}

// Called before render is available
bool M_Window::Awake(pugi::xml_node& config)
{
	BROFILER_CATEGORY("M_WindowAwake", Profiler::Color::Khaki)
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		Uint32 flags = SDL_WINDOW_SHOWN;
		bool fullscreen = config.child("fullscreen").attribute("value").as_bool(false);
		bool borderless = config.child("borderless").attribute("value").as_bool(false);
		bool resizable = config.child("resizable").attribute("value").as_bool(false);
		bool fullscreen_window = config.child("fullscreen_window").attribute("value").as_bool(false);

		width = config.child("resolution").attribute("width").as_int(640);
		height = config.child("resolution").attribute("height").as_int(480);
		scale = config.child("resolution").attribute("scale").as_int(1);

		SDL_DisplayMode mode;
		SDL_GetCurrentDisplayMode(0, &mode);

		if (width > mode.w)
		{
			width = mode.w;
		}
		if (height > mode.h)
		{
			height = mode.h;
		}
		
		if(fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if (fullscreen_window == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		if(borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		window = SDL_CreateWindow(app->GetTitle(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

// Called before quitting
bool M_Window::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

// Set new window title
void M_Window::SetTitle(const char* new_title)
{
	SDL_SetWindowTitle(window, new_title);
}

void M_Window::GetWindowSize(uint& width, uint& height) const
{
	width = this->width;
	height = this->height;
}

fRect M_Window::GetWindowRect()
{
	return fRect( 0.f, 0.f, screen_surface->w, screen_surface->h);
}

uint M_Window::GetScale() const
{
	return scale;
}