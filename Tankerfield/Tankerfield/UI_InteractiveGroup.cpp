#include "UI_InteractiveGroup.h"
#include "M_Input.h"
#include "App.h"
#include "Obj_Tank.h"
#include "Log.h"
#include "UI_Image.h"
#include <assert.h>


UI_InteractiveGroup::UI_InteractiveGroup(const fPoint position, const UI_InteractiveGroupDef definition, UI_Listener * listener) : UI_Element( position, definition, listener) , columns(definition.columns), rows(definition.rows)
{
	group_elements = new UI_Element*[rows * columns];

	for ( int i = 0 ; i < rows * columns ; ++i)
	{
		group_elements[i] = nullptr;
	}

	focus_image = app->ui->CreateImage({ 0.F,0.F }, UI_ImageDef({ 255, 265, 126,126 }));
	focus_image->SetParent(this);
}

UI_InteractiveGroup::~UI_InteractiveGroup()
{
	if (app->on_clean_up == false)
	{
		focus_image->Destroy();
	}
}

bool UI_InteractiveGroup::Update(float dt)
{

	if ( current_controller != nullptr && (*current_controller) != nullptr)
	{
		Controller* controller = (*current_controller);

		if (controller->GetButtonState(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP) == KEY_DOWN)
		{
			current_focus = GetNearestElement(current_focus, CONTROLLER_DIR::UP);
		}
		if (controller->GetButtonState(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN) == KEY_DOWN)
		{
			current_focus = GetNearestElement(current_focus, CONTROLLER_DIR::DOWN);
		}
		if (controller->GetButtonState(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == KEY_DOWN)
		{
			current_focus = GetNearestElement(current_focus, CONTROLLER_DIR::RIGHT);
		}
		if (controller->GetButtonState(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT) == KEY_DOWN)
		{
			current_focus = GetNearestElement(current_focus, CONTROLLER_DIR::LEFT);
		}

		UI_Element* element = GetElement(current_focus);

		focus_image->SetPos(element->position);
		focus_image->SetParent(element);
	}

	return true;
}

void UI_InteractiveGroup::SetElement(UI_Element* element, const iPoint position)
{
	group_elements[ (position.y * columns ) + position.x] = element;
	element->SetParent(this);
}

void UI_InteractiveGroup::SetController(Controller ** controller)
{
	current_controller = controller;
}

iPoint UI_InteractiveGroup::GetNearestElement( const iPoint current_focus,  const CONTROLLER_DIR dir)
{
	iPoint new_focus_pos = current_focus;

	switch (dir)
	{
	case CONTROLLER_DIR::UP:
		new_focus_pos.y -= 1;
		break;
	case CONTROLLER_DIR::DOWN:
		new_focus_pos.y += 1;
		break;
	case CONTROLLER_DIR::RIGHT:
		new_focus_pos.x += 1;
		break;
	case CONTROLLER_DIR::LEFT:
		new_focus_pos.x -= 1;
		break;
	}


	if (new_focus_pos.x < 0)
	{
		new_focus_pos.x = columns - 1;
	}
	else if (new_focus_pos.x > columns -1)
	{
		new_focus_pos.x = 0;
	}

	if (new_focus_pos.y < 0)
	{
		new_focus_pos.y = rows - 1;
	}
	else if (new_focus_pos.y > rows - 1)
	{
		new_focus_pos.y = 0;
	}

	return new_focus_pos;
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