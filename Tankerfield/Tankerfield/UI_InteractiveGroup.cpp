#include "UI_InteractiveGroup.h"
#include "M_Input.h"
#include "App.h"
#include "Obj_Tank.h"
#include "Log.h"
#include "UI_Image.h"
#include <assert.h>


UI_InteractiveGroup::UI_InteractiveGroup(const fPoint position, const UI_InteractiveGroupDef definition, UI_Listener * listener) : UI_Element( position, definition, listener) , 
columns(definition.columns), rows(definition.rows) ,focus_indicator(definition.focus_indicator)
{
	group_elements = new UI_Element*[rows * columns];

	for ( int i = 0 ; i < rows * columns ; ++i)
	{
		group_elements[i] = nullptr;
	}

	if (focus_indicator != nullptr)
	{
		focus_indicator->SetParent(this);
	}
	
}
bool UI_InteractiveGroup::OnHoverEnter(UI_Element * element)
{
	current_focus_pos = GetPos(element);
	SetFocusImage(current_focus_pos);
	app->ui->SetFocusedElement(GetElement(current_focus_pos));

	if (listener != nullptr)
	{
		listener->OnHoverEnter(this);
	}
	

	return true;
}

bool UI_InteractiveGroup::HandleControllerINavigation(Controller** controller )
{
	bool ret = false;

	for (uint input_dir = (uint)INPUT_DIR::RIGHT; input_dir < (uint)INPUT_DIR::MAX; ++input_dir)
	{
		if (app->input->GetControllerJoystickState(controller,Joystick::LEFT, (INPUT_DIR)input_dir) == KEY_DOWN)
		{
			SetNearestElement((INPUT_DIR)input_dir);
			ret = true;
		}
	}

	return ret;
}

bool UI_InteractiveGroup::HandleKeyboardNavigation()
{
	bool ret = false;

	if (app->input->GetKey(SDL_Scancode::SDL_SCANCODE_UP) == KEY_DOWN)
	{
		SetNearestElement(INPUT_DIR::UP);
		ret = true;
	}
	if (app->input->GetKey(SDL_Scancode::SDL_SCANCODE_DOWN) == KEY_DOWN)
	{
		SetNearestElement(INPUT_DIR::DOWN);
		ret = true;
	}
	if (app->input->GetKey(SDL_Scancode::SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		SetNearestElement(INPUT_DIR::RIGHT);
		ret = true;
	}
	if (app->input->GetKey(SDL_Scancode::SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		SetNearestElement(INPUT_DIR::LEFT);
		ret = true;
	}

	return ret;
}


bool UI_InteractiveGroup::OnHoverRepeat(UI_Element * object)
{
	if (listener != nullptr)
	{
		listener->OnHoverRepeat(this);
	}

	return true;
}

bool UI_InteractiveGroup::OnHoverExit(UI_Element * object)
{
	if (listener != nullptr)
	{
		listener->OnHoverExit(this);
	}

	app->ui->SetFocusedElement(nullptr);

	return true;
}


void UI_InteractiveGroup::SetFocusImage(iPoint point)
{
	if (focus_indicator == nullptr)
	{
		return;
	}

	current_focus_pos = point;

	UI_Element* element = GetElement(current_focus_pos);
	focus_indicator->SetPos(element->position);
	focus_indicator->SetParent(element);
}

void UI_InteractiveGroup::SetElement(UI_Element* element, const iPoint position)
{

	if (element == nullptr || position.x >= columns || position.y >= rows)
	{
		LOG("Imposible Set Element");
		return;
	}

	group_elements[ (position.y * columns ) + position.x] = element;
	element->SetParent(this);
	element->SetListener(this);
	element->section_width = element->sprite_rect.w;
	element->section_height= element->sprite_rect.h;

	app->ui->AddInteractiveElement(element);
}

void UI_InteractiveGroup::SetNearestElement( const INPUT_DIR dir)
{
	switch (dir)
	{
	case INPUT_DIR::UP:
		current_focus_pos.y -= 1;
		break;
	case INPUT_DIR::DOWN:
		current_focus_pos.y += 1;
		break;
	case INPUT_DIR::RIGHT:
		current_focus_pos.x += 1;
		break;
	case INPUT_DIR::LEFT:
		current_focus_pos.x -= 1;
		break;
	}


	if (current_focus_pos.x < 0)
	{
		current_focus_pos.x = columns - 1;
	}
	else if (current_focus_pos.x > columns -1)
	{
		current_focus_pos.x = 0;
	}

	if (current_focus_pos.y < 0)
	{
		current_focus_pos.y = rows - 1;
	}
	else if (current_focus_pos.y > rows - 1)
	{
		current_focus_pos.y = 0;
	}

	app->ui->SetFocusedElement(GetElement(current_focus_pos));
	SetFocusImage(current_focus_pos);
}

iPoint UI_InteractiveGroup::GetFirstAvailableElement()
{
	for (int y = 0; y < rows; ++y)
	{
		for (int x = 0; x < columns; ++x)
		{
			if ( GetElement(iPoint(x, y)) != nullptr )
			{				
				return iPoint(x, y);
			}
		}
	}

	assert("Interactive Group : No avaliable elements");
	return iPoint();
}

UI_Element* UI_InteractiveGroup::GetElement(iPoint position)
{
	return group_elements[(position.y * columns ) + position.x];
}

UI_Element * UI_InteractiveGroup::GetFocusedElement()
{
	return GetElement(current_focus_pos);
}

iPoint UI_InteractiveGroup::GetPos(UI_Element * element)
{
	for (int y = 0; y < rows; ++y)
	{
		for (int x = 0; x < columns; ++x)
		{
			if (group_elements[y * columns + x] == element)
			{
				return iPoint(x, y);
			}
		}
	}

	assert("Interactive Group : No avaliable elements");
	return iPoint();
}

void  UI_InteractiveGroup::Destroy()
{
	to_destroy = true;

	if (focus_indicator != nullptr)
	{
		focus_indicator->Destroy();
	}

}