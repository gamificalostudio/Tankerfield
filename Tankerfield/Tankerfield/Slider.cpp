#include "Slider.h"
#include "M_Render.h"
#include "p2Log.h"
#include "M_Input.h"
#include "M_UI.h"
#include "App.h"
#include"Button_Input.h"

Slider::Slider(const fPoint position, const Slider_Definition definition, Gui_Listener* listener) : UI_Object(position, definition, listener)
{
	this->definition = definition;

	section.w = definition.rail_draw_rect.w;
	section.h = definition.rail_draw_rect.h;

	thumb = App->ui->CreateButton(position, definition.thumb_definition, this);
	thumb->SetParent(this);
	thumb->IsDraggable(true);

	point_A = position.x - definition.distance * 0.5f;
	point_B = position.x + definition.distance * 0.5f;
	current_value = definition.default_value;
	SetValue(current_value);
}


Slider::~Slider()
{
	if (thumb != nullptr)
	{
		App->ui->DeleteObject(thumb);
		thumb = nullptr;
	}
}

bool Slider::Draw()
{
	App->render->Blit(App->ui->GetAtlas(), position.x - definition.rail_draw_rect.w * 0.5f , position.y - definition.rail_draw_rect.h * 0.5f, &definition.rail_draw_rect, false, 0.0f);
	return true;
}

int Slider::GetValue()
{
	return current_value;
}

void Slider::SetValue(int value)
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
	thumb->SetPosition(fPoint( (float)button_axis_value, position.y));
}

bool Slider::Update(float dt)
{
	point_A = position.x - definition.distance * 0.5f;
	point_B = position.x + definition.distance * 0.5f;

	fPoint pos = thumb->GetPosition();

	if (pos.x <= point_A)
	{
		thumb->SetPosition(fPoint(point_A, position.y));
	}
	else if (pos.x >= point_B)
	{
		thumb->SetPosition(fPoint(point_B, position.y));
	}
	else
	{
		thumb->SetPosition(fPoint( thumb->GetPosition().x, position.y));
	}

	current_value =   definition.max_value * ( ( (float) (pos.x - point_A) * 1.0f) / (float)definition.distance ) ;

	return true;
}

bool Slider::RepeatClick(UI_Object * object)
{
	if (object == thumb)
	{
		if (listener != nullptr)
		{
			listener->RepeatClick(this);
		}
	}

	return true;
}



