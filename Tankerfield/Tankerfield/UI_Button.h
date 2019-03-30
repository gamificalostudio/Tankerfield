#ifndef _BUTTON_INPUT_H__
#define _BUTTON_INPUT_H__

#include "M_UI.h"
#include "UI_Element.h"
#include "Point.h"
#include "Animation.h"


#define LABEL_PRESSED_OFFSET 4

class UI_Label;
struct UI_ButtonDef;

struct UI_ButtonDef: public UI_ElementDefinition
{
public:
	UI_ButtonDef() {}
	UI_ButtonDef(SDL_Rect idle_rect, SDL_Rect hover_rect, SDL_Rect pushed_rect) : idle_rect(idle_rect), hover_rect(hover_rect), pushed_rect(pushed_rect) {}

	SDL_Rect idle_rect = {0,0,0,0};
	SDL_Rect pushed_rect = { 0,0,0,0 };
	SDL_Rect hover_rect = { 0,0,0,0 };
};

class UI_Button : public UI_Element, public UI_Listener
{

public:

	UI_Button(const fPoint position, const UI_ButtonDef definition , UI_Listener * listener);

	virtual ~UI_Button();

	bool Draw();

	bool SetLabel(const fPoint position, const String text, const UI_LabelDef definition);

	void SetDefinition(UI_ButtonDef definition);

private:

	bool PreUpdate();

	bool Update(float dt);

private:

	UI_ButtonDef	definition;

public:
	// Components =================================
	UI_Label*			label = nullptr;

private:
	friend M_UI;
};

#endif // _BUTTON_INPUT_H__

