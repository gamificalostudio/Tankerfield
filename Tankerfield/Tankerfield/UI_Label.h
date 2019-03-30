#ifndef __LABEL_H__
#define __LABEL_H__

#include "Module.h"
#include "M_UI.h"
#include "UI_Element.h"
#include "p2Point.h"
#include "SDL_ttf/include/SDL_ttf.h"

class UI_Button;

struct UI_LabelDef : public UI_ElementDefinition
{
	UI_LabelDef(_TTF_Font* font, SDL_Color color = { 255,255,255,255 }): font(font) ,color(color) {}

	_TTF_Font* font = nullptr;
	SDL_Color  color = { 255,255,255,255 };
};

class UI_Label : public UI_Element
{
public:

	UI_Label(const fPoint position, const String text, UI_LabelDef definition, UI_Listener* listener);

	~UI_Label();

	void SetText(String text);

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