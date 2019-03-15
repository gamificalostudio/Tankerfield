#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "ModuleFonts.h"

#include "SDL\include\SDL.h"
#include "SDL_TTF\include\SDL_ttf.h"
#pragma comment( lib, "SDL_ttf/libx86/SDL2_ttf.lib" )

ModuleFonts::ModuleFonts() : j1Module()
{
	name.assign("fonts");
}

// Destructor
ModuleFonts::~ModuleFonts()
{}

// Called before render is available
bool ModuleFonts::Awake(pugi::xml_node& conf)
{
	LOG("Init True Type Font library");
	bool ret = true;

	if (TTF_Init() == -1)
	{
		LOG("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		ret = false;
	}
	else
	{
		const char* path = conf.child("default_font").attribute("file").as_string(DEFAULT_FONT);
		int size = conf.child("default_font").attribute("size").as_int(DEFAULT_FONT_SIZE);
		font_default = Load(path, size);
	}

	return ret;
}

// Called before quitting
bool ModuleFonts::CleanUp()
{
	LOG("Freeing True Type fonts and library");

	for (std::list<TTF_Font*>::const_iterator item = fonts.begin(); item != fonts.end(); item++)
	{
		TTF_CloseFont(*item);
	}

	fonts.clear();
	TTF_Quit();
	return true;
}

// Load new texture from file path
TTF_Font* const ModuleFonts::Load(const char* path, int size)
{
	TTF_Font* font = TTF_OpenFont(path, size);

	if (font == NULL)
	{
		LOG("Could not load TTF font with path: %s. TTF_OpenFont: %s", path, TTF_GetError());
	}
	else
	{
		LOG("Successfully loaded font %s size %d", path, size);
		fonts.assign(font);
	}

	return font;
}

// Print text using font
SDL_Texture* ModuleFonts::Print(const char* text, SDL_Color color, _TTF_Font* font)
{
	SDL_Texture* ret = NULL;
	SDL_Surface* surface = TTF_RenderText_Blended((font) ? font : font_default, text, color);

	if (surface == NULL)
	{
		LOG("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		ret = App->tex->LoadSurface(surface);
		SDL_FreeSurface(surface);
	}

	return ret;
}

// calculate size of a text
bool ModuleFonts::CalcSize(const char* text, int& width, int& height, _TTF_Font* font)
{
	bool ret = false;

	if (TTF_SizeText((font) ? font : font_default, text, &width, &height) != 0)
		LOG("Unable to calc size of text surface! SDL_ttf Error: %s\n", TTF_GetError());
	else
		ret = true;

	return ret;
}