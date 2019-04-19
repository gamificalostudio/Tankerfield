#ifndef __UI_LABEL_H__
#define __UI_LABEL_H__

#include "Module.h"
#include "M_UI.h"
#include "UI_Element.h"
#include "Point.h"
#include "SDL_ttf/include/SDL_ttf.h"

class UI_Button;

struct UI_LabelDef : public UI_ElementDef
{
	UI_LabelDef(const String text,  _TTF_Font* font, const SDL_Color color = { 255,255,255,255 }): text(text),font(font) ,color(color) {}

	_TTF_Font*   font = nullptr;
	SDL_Color    color = { 255,255,255,255 };
	String       text;
};

class UI_Label : public UI_Element
{
public:

	UI_Label(const fPoint position, const UI_LabelDef definition, UI_Listener* listener);

	void Destroy();

	void SetText(const String text);

	bool Draw();

private:

	SDL_Texture*        label_texture = nullptr;
	_TTF_Font*			font = nullptr;
	String	    	    text;
	SDL_Color           color = { 255,255,255,255 };

private:

	friend UI_Button;
};

#endif // __LABEL_H__