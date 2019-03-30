#ifndef __LABEL_H__
#define __LABEL_H__

#include "Module.h"
#include "M_UI.h"
#include "UI_Object.h"
#include "p2Point.h"
#include "SDL_ttf/include/SDL_ttf.h"

class Button;

struct Label_Definition : public UI_Object_Definition
{
	Label_Definition(_TTF_Font* font, SDL_Color color = { 255,255,255,255 }): font(font) ,color(color) {}

	_TTF_Font* font = nullptr;
	SDL_Color  color = { 255,255,255,255 };
};

class Label : public UI_Object
{
public:

	Label(const fPoint position, const String text, Label_Definition definition, Gui_Listener* listener);

	~Label();

	void SetText(String text);

	bool Draw();

private:

	SDL_Texture*        label_texture = nullptr;
	_TTF_Font*			font = nullptr;
	String	    	    text;
	SDL_Color           color = { 255,255,255,255 };

private:

	friend Button;
};

#endif // __LABEL_H__