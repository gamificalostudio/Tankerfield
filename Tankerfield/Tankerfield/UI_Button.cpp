#include "UI_Button.h"
#include "M_UI.h"
#include "M_Render.h"
#include "p2Log.h"
#include "M_Input.h"
#include "UI_Label.h"
#include "M_Audio.h"
#include "App.h"


Button::Button(const fPoint position, const Button_Definition definition, Gui_Listener* listener) : UI_Object(position, definition, listener)
{
	this->definition = definition;
}


Button::~Button()
{
	if (label != nullptr)
	{
		app->ui->DeleteObject(label);
		label = nullptr;
	}
}

bool Button::Draw()
{
	ClickState state = app->ui->GetClickState();
	SDL_Rect current_frame;

	if (app->ui->GetClickedObject() == this && state != ClickState::None &&  state != ClickState::Out)
	{
		current_frame = definition.pushed_rect;
	}
	else if (hover_state != HoverState::None)
	{
		current_frame = definition.hover_rect;
	}
	else
	{
		current_frame = definition.idle_rect;
	}

	section.w = current_frame.w;
	section.h = current_frame.h;
	
	app->render->Blit(app->ui->GetAtlas(), position.x - section.w*0.5f, position.y - section.h*0.5f, &current_frame, false, 0.0f);

	return true;
}

bool Button::SetLabel(const fPoint position, const String text , const Label_Definition definition)
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

void Button::SetDefinition(Button_Definition definition)
{
	this->definition = definition;
}

bool Button::PreUpdate()
{
	if (hover_state == HoverState::On && app->ui->GetClickedObject() != this)
	{
		// TODO 1: Add SFX  
	}
	
	return true;
}

bool Button::Update(float dt)
{
	if (label == nullptr)
	{
		return true;
	}

	if (this == app->ui->GetClickedObject())
	{
		ClickState state = app->ui->GetClickState();
		
		switch (state)
		{
		case ClickState::On:
			label->SetPosition({ label->position.x, label->position.y + LABEL_PRESSED_OFFSET });
			//App->audio->PlayFx(App->ui->fx_button_clicked);

			break;
		case ClickState::Out:
			label->SetPosition({ label->position.x, label->position.y - LABEL_PRESSED_OFFSET });
			break;
		}
	}

	return true;
}


