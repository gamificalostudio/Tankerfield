#include "Checkbox.h"
#include "j1Render.h"
#include  "p2Log.h"
#include "j1Input.h"
#include "Button_Input.h"
#include "j1App.h"

Checkbox::Checkbox(const iPoint position, const Checkbox_Definition definition, Gui_Listener* listener) : UI_Object(position, listener)
{
	this->definition = definition;

	Button_Definition def;

	if (definition.default_value)
	{
		def = definition.check_on_button;
	}
	else
	{
		def = definition.check_off_button;
	}

	button = App->ui->CreateButton(position, def, this);
	button->SetParent(this);
}

Checkbox::~Checkbox()
{
	if (button != nullptr)
	{
		App->ui->DeleteObject(button);
		button = nullptr;
	}
}

void Checkbox::SetValue(const bool value)
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

bool Checkbox::GetValue()
{
	return value;
}

bool Checkbox::OutClick(UI_Object * object)
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

