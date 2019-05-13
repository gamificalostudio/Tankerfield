#include "UI_InteractiveGroup.h"
#include "M_Input.h"
#include "App.h"
#include "Obj_Tank.h"
#include "Log.h"

#include "UI_Quad.h"

UI_InteractiveGroup::UI_InteractiveGroup(const fPoint position, const UI_InteractiveGroupDef definition, UI_Listener * listener) : UI_Element( position, definition, listener) , columns(definition.columns), rows(definition.rows)
{
	group_elements = new UI_Element*[rows * columns];

	for ( int i = 0 ; i < rows * columns ; ++i)
	{
		group_elements[i] = nullptr;
	}
}

UI_InteractiveGroup::~UI_InteractiveGroup()
{

}

bool UI_InteractiveGroup::Update(float dt)
{
	for (std::vector<PlayerFocus*>::iterator iter = players.begin(); iter != players.end(); ++iter)
	{
		if ( (*iter)->controller == nullptr)
		{
			(*iter)->controller = app->input->GetAbleController();
		}

		if ((*iter)->controller != nullptr && (*(*iter)->controller) != nullptr)
		{
			Controller* controller = (*(*iter)->controller);

			if (controller->GetButtonState(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP) == KEY_DOWN)
			{
				(*iter)->element_pos = GetNearestElement((*iter), CONTROLLER_DIR::UP);
			}
			if (controller->GetButtonState(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN) == KEY_DOWN)
			{
				(*iter)->element_pos = GetNearestElement((*iter), CONTROLLER_DIR::DOWN);
			}
			if (controller->GetButtonState(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == KEY_DOWN)
			{
				(*iter)->element_pos = GetNearestElement((*iter), CONTROLLER_DIR::RIGHT);
			}
			if (controller->GetButtonState(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT) == KEY_DOWN)
			{
				(*iter)->element_pos = GetNearestElement((*iter), CONTROLLER_DIR::LEFT);
			}

			UI_Element* element = GetElement((*iter)->element_pos);

			(*iter)->quad->SetPos(element->position);
			(*iter)->quad->SetParent(element);

			LOG(" x : %i y: %i", (*iter)->element_pos.x, (*iter)->element_pos.y);

		}
	}

	return true;
}

void UI_InteractiveGroup::SetElement(UI_Element* element, const iPoint position)
{
	group_elements[ (position.y * columns ) + position.x] = element;
}

iPoint UI_InteractiveGroup::GetNearestElement(PlayerFocus * players, CONTROLLER_DIR dir)
{
	iPoint new_focus_pos = players->element_pos;

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

	//if ( ElementIsFocused(new_focus_pos) == false)
	//{
		return new_focus_pos;
	//}
	//else
	//{
	//	/*return  current_element;*/
	//}
}

bool UI_InteractiveGroup::ElementIsFocused(iPoint element_pos)
{
	for (std::vector<PlayerFocus*>::iterator iter = players.begin(); iter != players.end(); ++iter)
	{
		if ((*iter)->element_pos == element_pos)
		{
			return true;
		}
	}

	return false;
}

iPoint UI_InteractiveGroup::GetFirstAvailableElement()
{
	for (int y = 0; y < rows; ++y)
	{
		for (int x = 0; x < columns; ++x)
		{
			if (ElementIsFocused(iPoint(x, y))== false && GetElement(iPoint(x, y)) != nullptr )
			{
				return iPoint(x, y);
			}
		}
	}

	LOG("Interactive Group : No avaliable elements");
	return iPoint(-1, -1);
}

UI_Element* UI_InteractiveGroup::GetElement(iPoint position)
{
	return group_elements[(position.y * columns ) + position.x];
}

void UI_InteractiveGroup::AddPlayer(int player_id)
{
	PlayerFocus* new_focus = new PlayerFocus;
	new_focus->element_pos = GetFirstAvailableElement();
	UI_Element* element = GetElement(new_focus->element_pos);
	new_focus->quad = app->ui->CreateQuad({0.F,0.F}, UI_QuadDef({ 0, 0, element->sprite_rect.w,  element->sprite_rect.h }, { 255,255,255,255 }));
	new_focus->quad->SetParent(this);
	new_focus->player_id = player_id;
	players.push_back(new_focus);
}