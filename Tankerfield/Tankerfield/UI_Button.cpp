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
	this->is_interactive = true;
	SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	section_width = definition.idle_rect.w;
	section_height = definition.idle_rect.h;
	sprite_rect = definition.idle_rect;
}

void UI_Button::Destroy()
{
	if (label != nullptr)
	{
		label->Destroy();
		label = nullptr;
	}

	to_destroy = true;
}

bool UI_Button::Draw()
{
	FocusState state = app->ui->GetClickState();

	if (app->ui->GetInputType() == UI_INPUT_TYPE::MOUSE)
	{
		if (app->ui->GetFocusedElement() == this && state != FocusState::NONE &&  state != FocusState::EXIT)
		{
			sprite_rect = definition.pushed_rect;
		}
		else
		{
			sprite_rect = definition.idle_rect;
		}

		if (hover_state == HoverState::REPEAT)
		{
			app->render->BlitUI(app->ui->GetAtlas(), position.x - definition.focus_fx.w * 0.5f, position.y - definition.focus_fx.h * 0.5f, &definition.focus_fx, app->ui->current_camera, (int)alpha);
		}
		else
		{
			app->render->BlitUI(app->ui->GetAtlas(), position.x - definition.normal_fx.w * 0.5f, position.y - definition.normal_fx.h * 0.5f, &definition.normal_fx, app->ui->current_camera, (int)alpha);
		}
	}
	else if (app->ui->GetInputType() == UI_INPUT_TYPE::CONTROLLER)
	{
	
		sprite_rect = definition.idle_rect;


		if (app->ui->GetFocusedElement() == this)
		{
			app->render->BlitUI(app->ui->GetAtlas(), position.x - definition.focus_fx.w * 0.5f, position.y - definition.focus_fx.h * 0.5f, &definition.focus_fx, app->ui->current_camera, (int)alpha);
		}
		else
		{
			app->render->BlitUI(app->ui->GetAtlas(), position.x - definition.normal_fx.w * 0.5f, position.y - definition.normal_fx.h * 0.5f, &definition.normal_fx, app->ui->current_camera, (int)alpha);
		}
	}


	SDL_Rect draw_rect = GetDrawRect();

	app->render->BlitUI(app->ui->GetAtlas(), draw_rect.x, draw_rect.y, &sprite_rect, app->ui->current_camera, (int)alpha);


	return true;
}

bool UI_Button::SetLabel(const fPoint position , const UI_LabelDef definition)
{
	if (label != nullptr)
	{
		label->Destroy();
		label = nullptr;
	}

	label = app->ui->CreateLabel(position + this->position, definition, this);
	label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	label->SetParent(this);

	return true;
}

bool UI_Button::PreUpdate()
{
	if (hover_state == HoverState::ENTER && app->ui->GetFocusedElement() != this)
	{
		// TODO 1: Add SFX  
	}
	
	return true;
}

void UI_Button::SetDefinition(const UI_ButtonDef def)
{
	this->definition = def;
}



