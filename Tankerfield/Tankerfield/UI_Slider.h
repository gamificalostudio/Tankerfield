#ifndef __UI_SLIDER_H__
#define __UI_SLIDER_H__

#include "Point.h"
#include "Animation.h"
#include "UI_Element.h"
#include "M_UI.h"
#include "UI_Button.h"

class UI_Button;

struct UI_SliderDef: public UI_ElementDef
{
	int                 distance = 100;
	int                 fixed_y = 0;
	SDL_Rect            rail_draw_rect = {0,0,0,0};
	int                 default_value = 0;
	UI_ButtonDef   thumb_definition;
	int                 max_value = 100;
};


class UI_Slider : public UI_Element, public UI_Listener
{

public:

	UI_Slider(const fPoint position, const UI_SliderDef definition, UI_Listener * listener);

	virtual ~UI_Slider();

	bool Draw();

	int GetValue();

	void SetValue(int value);

private:
	bool Update(float dt);

	bool RepeatClick(UI_Element* object);

private:
	UI_Button*       thumb = nullptr;

	UI_SliderDef   definition;
	int					point_A = 0;
	int					point_B = 0;
	int                 value = 0;
	int                 current_value = 0;
	int                 points_distance = 0;

private:
	friend M_UI;
	friend UI_Listener;
};

#endif // _SLIDER_H__
