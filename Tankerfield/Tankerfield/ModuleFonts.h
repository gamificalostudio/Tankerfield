#ifndef __MODULEFONTS_H__
#define __MODULEFONTS_H__

#include "j1Module.h"
#include "SDL\include\SDL_pixels.h"
#include <list>

#define DEFAULT_FONT "fonts/open_sans/OpenSans-Regular.ttf"
#define DEFAULT_FONT_SIZE 12

struct SDL_Texture;
struct _TTF_Font;

class ModuleFonts : public j1Module
{
public:

	ModuleFonts();
	~ModuleFonts();

public:

	bool Awake(pugi::xml_node&);
	bool CleanUp();
	_TTF_Font* const Load(const char* path, int size = 12);
	SDL_Texture* Print(const char* text, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL);
	bool CalcSize(const char* text, int& width, int& height, _TTF_Font* font = NULL);

public:

	std::list<_TTF_Font*> fonts;
	//p2List<_TTF_Font*>	fonts;
	_TTF_Font*			font_default;
};


#endif /* __MODULEFONTS_H__ */

