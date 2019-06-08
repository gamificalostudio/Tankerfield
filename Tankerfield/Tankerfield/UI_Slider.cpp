#include "UI_Slider.h"
#include "M_Render.h"
#include "Log.h"
#include "M_Input.h"
#include "M_UI.h"
#include "App.h"
#include "UI_Button.h"

UI_Slider::UI_Slider(const fPoint position, const UI_SliderDef definition, UI_Listener* listener) : UI_Element(position, definition, listener)
{
	this->definition = definition;

	thumb = app->ui->CreateButton(position, definition.thumb_definition, this);
	thumb->SetParent(this);
	//thumb->is_draggable = true;

	point_A = position.x - definition.distance * 0.5f;
	point_B = position.x + definition.distance * 0.5f;
	current_value = definition.default_value;
	SetValue(current_value);
}


UI_Slider::~UI_Slider()
{
	if (app->on_clean_up == false && thumb != nullptr)
	{
		thumb->Destroy();
		thumb = nullptr;
	}
}

bool UI_Slider::Draw()
{
	for (std::vector<Camera*>::iterator item_cam = app->render->cameras.begin(); item_cam != app->render->cameras.end(); item_cam++)
	{
		app->render->Blit(app->ui->GetAtlas(), position.x - definition.rail_draw_rect.w * 0.5f, position.y - definition.rail_draw_rect.h * 0.5f, (*item_cam), &definition.rail_draw_rect);
	}
	return true;
}

int UI_Slider::GetValue()
{
	return current_value;
}

void UI_Slider::SetValue(int value)
{
	if (value > definition.max_value)
	{
		current_value = definition.max_value;
	}
	else
	{
		current_value = value;
	}

	current_value = value;
	int button_axis_value = point_A + (current_value* definition.distance) / 100;
	thumb->position = fPoint( (float)button_axis_value, position.y);
}

bool UI_Slider::Update(float dt)
{
	point_A = position.x - definition.distance * 0.5f;
	point_B = position.x + definition.distance * 0.5f;

	if (thumb->position.x <= point_A)
	{
		thumb->position = fPoint(point_A, position.y);
	}
	else if (thumb->position.x >= point_B)
	{
		thumb->position = fPoint(point_B, position.y);
	}
	else
	{
		thumb->position = fPoint( thumb->position.x, position.y);
	}

	current_value =   definition.max_value * ( ( (float) (thumb->position.x - point_A) * 1.0f) / (float)definition.distance ) ;

	return true;
}

bool UI_Slider::RepeatClick(UI_Element * object)
{
	if (object == thumb)
	{
		//if (listener != nullptr)
		//{
		//	listener->ClickRepeat(this);
		//}
	}

	return true;
}



