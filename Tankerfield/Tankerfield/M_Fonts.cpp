
#include "SDL\include\SDL.h"
#include "SDL_TTF\include\SDL_ttf.h"
#pragma comment( lib, "SDL_ttf/libx86/SDL2_ttf.lib" )

#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "M_Textures.h"
#include "M_Fonts.h"
#include "Brofiler/Brofiler.h"





M_Fonts::M_Fonts() : Module()
{
	name = "fonts";
}

// Destructor
M_Fonts::~M_Fonts()
{}

// Called before render is available
bool M_Fonts::Awake(pugi::xml_node& conf)
{
	LOG("Init True Type Font library");
	bool ret = true;

	if(TTF_Init() == -1)
	{
		LOG("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		ret = false;
	}
	else
	{
		const char* path = conf.child("default_font").attribute("file").as_string(DEFAULT_FONT);
		int size = conf.child("default_font").attribute("size").as_int(DEFAULT_FONT_SIZE);
		default_font = Load(path, size);
	}

	return ret;
}

bool M_Fonts::Start()
{
	font_open_sants_bold_12 = app->font->Load("fonts/open_sans/OpenSans-Bold.ttf");
	rounds_font = app->font->Load("fonts/round_font.ttf", 35);
	button_font_22 = app->font->Load("fonts/round_font.ttf", 22);
	button_font_40 = app->font->Load("fonts/round_font.ttf", 40);
	label_font_24 = app->font->Load("fonts/labels.ttf", 24);
	label_font_38 = app->font->Load("fonts/labels.ttf", 38);

	return true;
}

// Called before quitting
bool M_Fonts::CleanUp()
{
	LOG("Freeing True Type fonts and library");

	for (std::list<TTF_Font*>::iterator iter = fonts.begin(); iter != fonts.end(); ++iter)
	{
		TTF_CloseFont(*iter);
	}

	font_open_sants_bold_12 = nullptr;
	rounds_font = nullptr;
	button_font_22 = nullptr;
	label_font_24 = nullptr;

	fonts.clear();
	TTF_Quit();
	return true;
}

// Load new texture from file path
TTF_Font* const M_Fonts::Load(const char* path, int size)
{
	BROFILER_CATEGORY("M_FontsLoad", Profiler::Color::Yellow)

	//TTF_Font* font = nullptr;
	//
	//std::map<std::string, TTF_Font *>::iterator iter = fonts.find(path);

	//if (iter != fonts.end())
	//{
	//	//Return the texture if it's already been loaded onto the map
	//	font = iter->second;
	//}
	//else
	//{
	//	font = TTF_OpenFont(path, size);

	//	//Load a new texture if there isn't a texture loaded with that path
	//	if (font == nullptr)
	//	{
	//		LOG("Could not load TTF font with path: %s. TTF_OpenFont: %s", path, TTF_GetError());
	//	}
	//	else
	//	{
	//		fonts[path] = font;
	//	}
	//}

	TTF_Font* font = nullptr;

	font = TTF_OpenFont(path, size);

	//Load a new texture if there isn't a texture loaded with that path
	if (font == nullptr)
	{
		LOG("Could not load TTF font with path: %s. TTF_OpenFont: %s", path, TTF_GetError());
	}
	else
	{
		fonts.push_back(font);
	}

	return font;
}

bool M_Fonts::Unload(_TTF_Font * font)
{
	list<_TTF_Font*>::iterator item;
	std::list< TTF_Font *>::iterator iter = std::find(fonts.begin(), fonts.end(), font);

	if (iter != fonts.end())
	{
		TTF_CloseFont(*iter);
		iter = fonts.erase(iter);
		return true;
	}

	LOG("Font to unload not found");
	return false;
}

// Print text using font
SDL_Texture* M_Fonts::Print(const char* text, SDL_Color color, _TTF_Font* font)
{
	if (font == nullptr)
	{
		LOG("Default font printed");
	}
	SDL_Texture* ret = NULL;
	SDL_Surface* surface = TTF_RenderText_Blended((font) ? font : default_font, text, color);
	
	if(surface == NULL)
	{
		LOG("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		ret = app->tex->LoadTextSurface(surface);
		SDL_FreeSurface(surface);
	}

	return ret;
}

// calculate size of a text
bool M_Fonts::CalcSize(const char* text, int& width, int& height, _TTF_Font* font) const
{
	bool ret = false;

	if(TTF_SizeText((font) ? font : default_font, text, &width, &height) != 0)
		LOG("Unable to calc size of text surface! SDL_ttf Error: %s\n", TTF_GetError());
	else
		ret = true;

	return ret;
}