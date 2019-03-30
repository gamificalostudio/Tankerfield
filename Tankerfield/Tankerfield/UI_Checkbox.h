#ifndef _CHECKBOX_H__
#define _CHECKBOX_H__

#include "M_UI.h"
#include "UI_Element.h"
#include "p2Point.h"
#include "Animation.h"
#include "UI_Button.h"

class Gui_Listener;

struct Checkbox_Definition: public UI_Object_Definition
{
	bool                default_value = false;
	Button_Definition   check_on_button;
	Button_Definition   check_off_button;
};

class Checkbox : public UI_Object, public Gui_Listener
{
public:

	Checkbox(const fPoint position, const Checkbox_Definition definition, Gui_Listener * listener);

	virtual ~Checkbox();

	void SetValue(const bool value);

	bool GetValue();

private:

	bool OutClick(UI_Object* object);

private:

	Checkbox_Definition     definition;
	bool                    value = false;

	// Components =================================

	Button* button = nullptr;   

private:
	friend M_UI;
};

#endif // _CHECKBOX_H__
