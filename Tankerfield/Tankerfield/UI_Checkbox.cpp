#include "UI_Checkbox.h"
#include "M_Render.h"
#include "Log.h"
#include "M_Input.h"
#include "UI_Button.h"
#include "App.h"

UI_Checkbox::UI_Checkbox(const fPoint position, const UI_CheckboxDef definition, UI_Listener* listener) : UI_Element(position, definition, listener)
{
	this->definition = definition;

	UI_ButtonDef def;

	if (definition.default_value)
	{
		def = definition.check_on_button;
	}
	else
	{
		def = definition.check_off_button;
	}

	button = app->ui->CreateButton(position, def, this);
	button->SetParent(this);
}

UI_Checkbox::~UI_Checkbox()
{
	if (app->on_clean_up == false && button != nullptr)
	{
		button->Destroy();
		button = nullptr;
	}
}

void UI_Checkbox::SetValue(const bool value)
{
	this->value = value;

	if (value)
	{
		button->SetDefinition(definition.check_on_button);
	}
	else
	{
		button->SetDefinition(definition.check_off_button);
	}

}

bool UI_Checkbox::GetValue()
{
	return value;
}

bool UI_Checkbox::OutClick(UI_Element * object)
{
	if (object == button)
	{
		value = !value;

		if (value)
		{
			button->SetDefinition(definition.check_on_button);
		}
		else
		{
			button->SetDefinition(definition.check_off_button);
		}

		if (listener != nullptr)
		{
			listener->OutClick(this);
		}
	}
	return true;
}

