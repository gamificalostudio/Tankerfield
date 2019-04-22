#include "Brofiler/Brofiler.h"

#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "M_Render.h"
#include "M_Textures.h"


#include "SDL_image/include/SDL_image.h"
#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )

M_Textures::M_Textures() : Module()
{
	name = "textures";
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
	

	for(std::map<std::string, SDL_Texture*>::iterator iter = textures.begin(); iter != textures.end();)
	{
		SDL_DestroyTexture(iter->second);
		iter->second = nullptr;
		iter = textures.erase(iter);
	}
	textures.clear();

	for (std::list<SDL_Texture*>::iterator iter = text_textures.begin(); iter != text_textures.end();)
	{
		SDL_DestroyTexture((*iter));
		(*iter) = nullptr;
		iter = text_textures.erase(iter);
	}
	text_textures.clear();

	IMG_Quit();
	return true;
}

// Load new texture from file path
SDL_Texture* const M_Textures::Load(const char* path)
{
	SDL_Texture* texture = nullptr;

	std::map<std::string, SDL_Texture *>::iterator iter = textures.find(path);
	if (iter != textures.end())
	{
		//Return the texture if it's already been loaded onto the map
		texture = iter->second;
	}
	else
	{
		SDL_Surface* surface = IMG_Load(path);
		//Load a new texture if there isn't a texture loaded with that path
		if (surface == nullptr)
		{
			LOG("Could not load surface with path: %s. IMG_Load: %s", path, IMG_GetError());
		}
		else
		{
			texture = LoadSurface(surface, path);
			SDL_FreeSurface(surface);
		}
	}

	return texture;
}

// Unload texture
bool M_Textures::UnLoad(SDL_Texture* texture, bool is_text_texture)
{
	if (is_text_texture)
	{
		std::list<SDL_Texture*>::iterator text_find = std::find(text_textures.begin(), text_textures.end(), texture);

		if (text_find != text_textures.end())
		{
			SDL_DestroyTexture(*text_find);
			text_textures.erase(text_find);
		}
	}
	else
	{
		for (std::map<std::string, SDL_Texture *>::iterator iter = textures.begin(); iter != textures.end(); ++iter)
		{
			if ((iter->second) == texture)
			{
				SDL_DestroyTexture((iter->second));
				iter->second = nullptr;
				textures.erase(iter);
				return true;
			}
		}
	}

	return true;
}

// Translate a surface into a texture
SDL_Texture* const M_Textures::LoadSurface(SDL_Surface* surface, std::string path)
{
	BROFILER_CATEGORY("M_TexturesLoad", Profiler::Color::LightPink)
	SDL_Texture* texture = SDL_CreateTextureFromSurface(app->render->renderer, surface);

	if(texture == NULL)
	{
		LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
	}
	else
	{
		textures[path] = texture;
	}

	return texture;
}

SDL_Texture * const M_Textures::LoadTextSurface(SDL_Surface* surface)
{
	BROFILER_CATEGORY("M_TexturesTextLoad", Profiler::Color::LightPink)
		SDL_Texture* texture = SDL_CreateTextureFromSurface(app->render->renderer, surface);

	if (texture == NULL)
	{
		LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
	}
	else
	{
		text_textures.push_back(texture);
	}

	return texture;
}

// Retrieve size of a texture
void M_Textures::GetSize(const SDL_Texture* texture, uint& width, uint& height) const
{
	SDL_QueryTexture((SDL_Texture*)texture, NULL, NULL, (int*) &width, (int*) &height);
}
