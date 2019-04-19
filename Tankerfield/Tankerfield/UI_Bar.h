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

struct UI_BarDef : public UI_ElementDef
{
	UI_BarDef()
	{};
	UI_BarDef(UI_Bar::DIR direction, float value, SDL_Color color_1, SDL_Color color_2) : direction(direction), value(value), color_1(color_1), color_2(color_2)
	{};

	UI_Bar::DIR		direction = UI_Bar::DIR::UP;
	SDL_Color		color_1 = { 255, 255, 255, 255 };
	SDL_Color		color_2 = { 0, 0, 0, 255 };
	float           value = 1.0f;
};


#endif // __UI_BAR_H__

