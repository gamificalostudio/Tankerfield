#ifndef _SLIDER_H__
#define _SLIDER_H__

#include "p2Point.h"
#include "Animation.h"
#include "UI_Object.h"
#include "Module_UI.h"
#include "Button_Input.h"

struct Slider_Definition;
class Button;

struct Slider_Definition
{
	int                 distance = 100;
	int                 fixed_y = 0;
	SDL_Rect            rail_draw_rect = {0,0,0,0};
	int                 default_value = 0;
	Button_Definition   thumb_definition;
	int                 max_value = 100;
};


class Slider : public UI_Object, public Gui_Listener
{

public:

	Slider(iPoint const position, const Slider_Definition definition, SDL_Texture * texture, Gui_Listener * listener);

	virtual ~Slider();

	bool Draw();

	int GetValue();

	void SetValue(int value);

private:
	bool Update(float dt);

	bool RepeatClick(UI_Object* object);

private:
	Button*       thumb = nullptr;
	Slider_Definition   definition;
	int					point_A = 0;
	int					point_B = 0;
	int                 value = 0;
	int                 current_value = 0;
	int                 points_distance = 0;

private:
	friend Module_UI;
	friend Gui_Listener;
};

#endif // _SLIDER_H__
