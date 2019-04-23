#ifndef __UI_BUTTON_H__
#define __UI_BUTTON_H__

#include "M_UI.h"
#include "UI_Element.h"
#include "Point.h"
#include "Animation.h"


#define LABEL_PRESSED_OFFSET 4

class UI_Label;
struct UI_ButtonDef;

struct UI_ButtonDef: public UI_ElementDef
{
public:
	UI_ButtonDef() {}
	UI_ButtonDef(const SDL_Rect idle_rect, const  SDL_Rect pushed_rect,const  SDL_Rect normal_fx,const SDL_Rect focus_fx) : idle_rect(idle_rect), pushed_rect(pushed_rect) , normal_fx(normal_fx),focus_fx(focus_fx){}

	SDL_Rect idle_rect = {0,0,0,0};
	SDL_Rect pushed_rect = { 0,0,0,0 };
	SDL_Rect focus_fx = { 0,0,0,0 };
	SDL_Rect normal_fx = { 0,0,0,0 };
};

class UI_Button : public UI_Element, public UI_Listener
{

public:

	UI_Button(const fPoint position, const UI_ButtonDef definition , UI_Listener * listener);

	virtual ~UI_Button();

	bool PreUpdate();

	bool Draw();

	bool SetLabel(const fPoint position, const UI_LabelDef definition);

	void SetDefinition(const UI_ButtonDef def);

private:

	UI_ButtonDef	definition;

public:
	// Components =================================
	UI_Label*			label = nullptr;

private:
	friend M_UI;
};

#endif // _BUTTON_INPUT_H__

