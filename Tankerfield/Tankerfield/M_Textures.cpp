#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "M_Render.h"
#include "M_Textures.h"
#include "Brofiler/Brofiler.h"

#include "SDL_image/include/SDL_image.h"
#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )

M_Textures::M_Textures() : Module()
{
	name.assign("textures");
}

// Destructor
M_Textures::~M_Textures()
{}

// Called before render is available
bool M_Textures::Awake(pugi::xml_node& config)
{
	LOG("Init Image library");
	bool ret = true;
	// load support for the PNG image format
	int flags = IMG_INIT_PNG;
	int init = IMG_Init(flags);

	if((init & flags) != flags)
	{
		LOG("Could not initialize Image lib. IMG_Init: %s", IMG_GetError());
		ret = false;
	}

	return ret;
}

// Called before the first frame
bool M_Textures::Start()
{
	LOG("start textures");
	bool ret = true;
	return ret;
}

// Called before quitting
bool M_Textures::CleanUp()
{
	LOG("Freeing textures and Image library");
	std::list<SDL_Texture*>::const_iterator item;

	for(item = textures.begin(); item != textures.end(); item++)
	{
		SDL_DestroyTexture(*item);
	}

	textures.clear();
	IMG_Quit();
	return true;
}

// Load new texture from file path
SDL_Texture* const M_Textures::Load(const char* path)
{
	SDL_Texture* texture = NULL;
	SDL_Surface* surface = IMG_Load(path);

	if(surface == NULL)
	{
		LOG("Could not load surface with path: %s. IMG_Load: %s", path, IMG_GetError());
	}
	else
	{
		texture = LoadSurface(surface);
		SDL_FreeSurface(surface);
	}

	return texture;
}

// Unload texture
bool M_Textures::UnLoad(SDL_Texture* texture)
{
	std::list<SDL_Texture*>::const_iterator item;

	for(item = textures.begin(); item != textures.end(); item++)
	{
		if(texture == (*item))
		{
			SDL_DestroyTexture(texture);
			textures.remove(texture);
			return true;
		}
	}

	return false;
}

// Translate a surface into a texture
SDL_Texture* const M_Textures::LoadSurface(SDL_Surface* surface)
{
	//BROFILER_CATEGORY("M_TexturesLoad", Profiler::Color::LightPink)
	SDL_Texture* texture = SDL_CreateTextureFromSurface(app->render->renderer, surface);

	if(texture == NULL)
	{
		LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
	}
	else
	{
		textures.push_back(texture);
	}

	return texture;
}

// Retrieve size of a texture
void M_Textures::GetSize(const SDL_Texture* texture, uint& width, uint& height) const
{
	SDL_QueryTexture((SDL_Texture*)texture, NULL, NULL, (int*) &width, (int*) &height);
}
