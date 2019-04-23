#ifndef __M_FONTS_H__
#define __M_FONTS_H__

#include <list>

#include "SDL\include\SDL_pixels.h"

#include "Module.h"

#define DEFAULT_FONT "fonts/open_sans/OpenSans-Regular.ttf"
#define DEFAULT_FONT_SIZE 12

using namespace std;
struct SDL_Texture;
struct _TTF_Font;

class M_Fonts : public Module
{
public:

	M_Fonts();

	// Destructor
	virtual ~M_Fonts();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	bool Start();
	// Called before quitting
	bool CleanUp();

	// Load Font
	_TTF_Font* const Load(const char* path, int size = 12);

	bool Unload(_TTF_Font* font);

	// Create a surface from text
	SDL_Texture* Print(const char* text, SDL_Color color = {255, 255, 255, 255}, _TTF_Font* font = NULL);

	bool CalcSize(const char* text, int& width, int& height, _TTF_Font* font = NULL) const;

public:

	list <_TTF_Font*>	fonts;

	_TTF_Font*			        default_font = nullptr;
	_TTF_Font*					font_open_sants_bold_12 = nullptr;
	_TTF_Font*					rounds_font = nullptr;
	_TTF_Font*					button_font = nullptr;
	_TTF_Font*					label_font_24 = nullptr;
	_TTF_Font*					label_font_38 = nullptr;
};


#endif // __j1FONTS_H__