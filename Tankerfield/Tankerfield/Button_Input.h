#ifndef _BUTTON_INPUT_H__
#define _BUTTON_INPUT_H__

#include "Module_UI.h"
#include "UI_Object.h"
#include "p2Point.h"
#include "Animation.h"


#define LABEL_PRESSED_OFFSET 4

class Label;
struct Button_Definition;

struct Button_Definition
{
public:
	Button_Definition() {}
	Button_Definition(SDL_Rect idle_rect, SDL_Rect hover_rect, SDL_Rect pushed_rect)
	{
		this->idle_rect = idle_rect;
		this->hover_rect = hover_rect;
		this->pushed_rect = pushed_rect;
	}

	SDL_Rect idle_rect = {0,0,0,0};
	SDL_Rect pushed_rect = { 0,0,0,0 };
	SDL_Rect hover_rect = { 0,0,0,0 };
};

class Button : public UI_Object, public Gui_Listener
{

public:

	Button(const iPoint position, const Button_Definition definition, SDL_Texture * texture , Gui_Listener * listener);

	virtual ~Button();

	bool Draw();

	bool SetLabel(const iPoint position, const String text, _TTF_Font* font = nullptr, const SDL_Color color = {255,255,255,255});

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

