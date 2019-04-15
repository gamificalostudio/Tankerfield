#ifndef __UI_CHECKBOX_H__
#define __UI_CHECKBOX_H__

#include "M_UI.h"
#include "UI_Element.h"
#include "Point.h"
#include "Animation.h"
#include "UI_Button.h"

class UI_Listener;

struct UI_CheckboxDef: public UI_ElementDef
{
	bool                default_value = false;
	UI_ButtonDef   check_on_button;
	UI_ButtonDef   check_off_button;
};

class UI_Checkbox : public UI_Element, public UI_Listener
{
public:

	UI_Checkbox(const fPoint position, const UI_CheckboxDef definition, UI_Listener * listener);

	virtual ~UI_Checkbox();

	void SetValue(const bool value);

	bool GetValue();

private:

	bool OutClick(UI_Element* object);

private:

	UI_CheckboxDef     definition;
	bool                    value = false;

	// Components =================================

	UI_Button* button = nullptr;   

private:
	friend M_UI;
};

#endif // _CHECKBOX_H__
