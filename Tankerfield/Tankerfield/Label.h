#ifndef __LABEL_H__
#define __LABEL_H__

#include "j1Module.h"
#include "Module_UI.h"
#include "UI_Object.h"
#include "p2Point.h"
#include "SDL_ttf/include/SDL_ttf.h"


class Button;

class Label : public UI_Object
{
public:

	Label(const iPoint position, const String text, _TTF_Font* font, const SDL_Color color, Gui_Listener* listener);

	// Destructor
	~Label();

	// Set the text
	void SetText(String text);

	// Draw label
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