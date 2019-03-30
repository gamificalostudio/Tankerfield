#ifndef _BUTTON_INPUT_H__
#define _BUTTON_INPUT_H__

#include "M_UI.h"
#include "UI_Object.h"
#include "p2Point.h"
#include "Animation.h"


#define LABEL_PRESSED_OFFSET 4

class Label;
struct Button_Definition;

struct Button_Definition: public UI_Object_Definition
{
public:
	Button_Definition() {}
	Button_Definition(SDL_Rect idle_rect, SDL_Rect hover_rect, SDL_Rect pushed_rect) : idle_rect(idle_rect), hover_rect(hover_rect), pushed_rect(pushed_rect) {}

	SDL_Rect idle_rect = {0,0,0,0};
	SDL_Rect pushed_rect = { 0,0,0,0 };
	SDL_Rect hover_rect = { 0,0,0,0 };
};

class Button : public UI_Object, public Gui_Listener
{

public:

	Button(const fPoint position, const Button_Definition definition , Gui_Listener * listener);

	virtual ~Button();

	bool Draw();

	bool SetLabel(const fPoint position, const String text, const Label_Definition definition);

	void SetDefinition(Button_Definition definition);

private:

	bool PreUpdate();

	bool Update(float dt);

private:

	Button_Definition	definition;

public:
	// Components =================================
	Label*			label = nullptr;

private:
	friend Module_UI;
};

#endif // _BUTTON_INPUT_H__

