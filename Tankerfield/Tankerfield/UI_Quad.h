#ifndef __UI_QUAD_H__
#define __UI_QUAD_H__

#include "Point.h"
#include "UI_Element.h"

struct UI_BarDef;
struct UI_QuadDef;

class UI_Quad : public UI_Element
{
public:

	UI_Quad(const fPoint position, const UI_QuadDef def, UI_Listener* listener);

	bool Draw();

public:

	SDL_Color		color = { 0, 0, 0, 0 };

};

struct UI_QuadDef : public UI_ElementDef
{
public:

	UI_QuadDef()
	{};
	UI_QuadDef( SDL_Rect  draw_section , SDL_Color color) : color(color)
	{
		sprite_section = draw_section;
	};

	SDL_Color		color = { 255, 255, 255, 255 };
};


#endif // __UI_QUAD_H__


