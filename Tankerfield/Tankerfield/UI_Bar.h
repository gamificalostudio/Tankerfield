#ifndef __UI_BAR_H__
#define __UI_BAR_H__

#include "Point.h"
#include "Animation.h"
#include "UI_Element.h"

struct UI_BarDef;

class UI_Bar : public UI_Element
{
public:

	UI_Bar(const fPoint position, const UI_BarDef def, UI_Listener* listener);

	virtual ~UI_Bar();

	bool Draw();

public:

	enum class DIR
	{
		LEFT,
		RIGHT,
		UP,
		DOWN
	}				direction = UI_Bar::DIR::UP;
	SDL_Color		color_1 = { 0, 0, 0, 0 };
	SDL_Color		color_2 = { 0, 0, 0, 0 };
	float           value = 1.f;

};

struct UI_BarDef : public UI_ElementDefinition
{
	UI_Bar::DIR		direction = UI_Bar::DIR::UP;
	SDL_Color		color_1 = { 0, 0, 0, 0 };
	SDL_Color		color_2 = { 0, 0, 0, 0 };
};


#endif // __UI_BAR_H__

