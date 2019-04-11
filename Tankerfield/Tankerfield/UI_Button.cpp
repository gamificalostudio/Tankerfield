#include "UI_Button.h"
#include "M_UI.h"
#include "M_Render.h"
#include "Log.h"
#include "M_Input.h"
#include "UI_Label.h"
#include "M_Audio.h"
#include "App.h"


UI_Button::UI_Button(const fPoint position, const UI_ButtonDef definition, UI_Listener* listener) : UI_Element(position, definition, listener)
{
	this->definition = definition;
}


UI_Button::~UI_Button()
{
	if (label != nullptr)
	{
		app->ui->DeleteObject(label);
		label = nullptr;
	}
}

bool UI_Button::Draw()
{
	ClickState state = app->ui->GetClickState();
	SDL_Rect current_frame;

	if (app->ui->GetClickedObject() == this && state != ClickState::NONE &&  state != ClickState::EXIT)
	{
		current_frame = definition.pushed_rect;
	}
	else if (hover_state != HoverState::NONE)
	{
		current_frame = definition.hover_rect;
	}
	else
	{
		current_frame = definition.idle_rect;
	}


	return true;
}

bool UI_Button::SetLabel(const fPoint position, const String text , const UI_LabelDef definition)
{
	if (label != nullptr)
	{
		app->ui->DeleteObject(label);
		label = nullptr;
	}

	label = app->ui->CreateLabel(position + this->position, text, definition, this);
	label->SetParent(this);

	return true;
}

void UI_Button::SetDefinition(UI_ButtonDef definition)
{
	this->definition = definition;
}

bool UI_Button::PreUpdate()
{
	if (hover_state == HoverState::ENTER && app->ui->GetClickedObject() != this)
	{
		// TODO 1: Add SFX  
	}
	
	return true;
}



