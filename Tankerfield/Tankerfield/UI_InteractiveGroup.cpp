#include "UI_InteractiveGroup.h"
#include "M_Input.h"
#include "App.h"
#include "Obj_Tank.h"
#include "Log.h"
#include "UI_Image.h"
#include <assert.h>


UI_InteractiveGroup::UI_InteractiveGroup(const fPoint position, const UI_InteractiveGroupDef definition, UI_Listener * listener) : UI_Element( position, definition, listener) , focus_indicator(definition.focus_indicator), focused_controller(definition.focused_controller)
{
	app->ui->interactive_groups.push_back(this);
}

UI_InteractiveGroup::~UI_InteractiveGroup()
{
	
}

bool UI_InteractiveGroup::Update(float dt)
{
	
	return true;
}

void UI_InteractiveGroup::AddElement(UI_Element* element)
{
	if (element != nullptr)
	{
		element->SetParent(this);
		element->SetListener(this);
		element->section_height = element->sprite_rect.h;
		element->section_width = element->sprite_rect.w;
		group_elements_list.push_back(element);
	}
}

void  UI_InteractiveGroup::Active()
{
	if (first_focus == true)
	{
		focused_element = GetFirstAbleElement();
		listener->UI_OnHoverEnter(focused_element);
		focused_element->is_focused = true;
		first_focus = false;
	}

	if (focus_indicator != nullptr && focused_element != nullptr)
	{
		focus_indicator->SetMenuState(ELEMENT_STATE::VISIBLE);
		SetFocusImage(focused_element);
	}

	is_active = true;

	app->ui->prevent_double_select = true;
}

void  UI_InteractiveGroup::Desactive()
{
	if (focus_indicator != nullptr)
	{
		focus_indicator->SetMenuState(ELEMENT_STATE::HIDDEN);
	}

	is_active = false;

	app->ui->prevent_double_select = true;
}

void UI_InteractiveGroup::SetControllers(int controller_id)
{
	focused_controller = controller_id;
}

void UI_InteractiveGroup::SetFocusImage( UI_Element* element)
{
	focus_indicator->SetPos(element->position);
	focus_indicator->SetParent(element);
}

UI_Element* UI_InteractiveGroup::GetFocusedElement()
{
	return focused_element;
}

std::list<UI_Element*>* UI_InteractiveGroup::GetGroupList()
{
	return &group_elements_list;
}

void UI_InteractiveGroup::MouseSelection()
{
	if (focused_element != nullptr)
	{
		if (app->input->GetMouseButton(1) == KEY_UP)
		{
			listener->UI_Selected(focused_element);
		}
	}
}

void UI_InteractiveGroup::MouseNavigation()
{
	fRect section;
	fPoint mouse_position = app->ui->mouse_position;
	bool mouse_is_focusing = false;

	for (list<UI_Element*>::iterator iter = group_elements_list.begin(); iter != group_elements_list.end(); ++iter)
	{
		if ((*iter)->state != ELEMENT_STATE::VISIBLE)
		{
			continue;
		}

		section = (*iter)->GetSection();

		if (mouse_position.x >= section.GetLeft() && mouse_position.x <= section.GetRight() && mouse_position.y >= section.GetTop() && mouse_position.y <= section.GetBottom() && (*iter)->listener != nullptr)
		{
			mouse_is_focusing = true;

			if (focused_element != (*iter))
			{
				if (focused_element != nullptr)
				{
					listener->UI_OnHoverExit((*iter));
					focused_element->is_focused = false;
				}

				focused_element = (*iter);

				if (focused_element != nullptr)
				{
					listener->UI_OnHoverEnter((*iter));
					focused_element->is_focused = true;

					if (focus_indicator != nullptr)
					{
						focus_indicator->SetMenuState(ELEMENT_STATE::VISIBLE);
						SetFocusImage(focused_element);
					}
				}
			}
			else
			{
				if (focused_element != nullptr)
				{
					listener->UI_OnHoverRepeat((*iter));
				}
			}
			break;
		}
	}

	if (mouse_is_focusing == false && focused_element != nullptr)
	{
		listener->UI_OnHoverExit(focused_element);
		focused_element->is_focused = false;
		focused_element = nullptr;

		if (focus_indicator != nullptr)
		{
			focus_indicator->SetMenuState(ELEMENT_STATE::HIDDEN);
		}
	}
}

void UI_InteractiveGroup::ControllerSelection()
{
	bool ret = false;

	if (focused_element != nullptr)
	{
		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
			if ((focused_controller != -1 && focused_controller != i) || app->input->IsConnectedController(i) == false)
			{
				continue;
			}

			if (app->input->GetControllerButtonState(i, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A) == KEY_DOWN)
			{
				listener->UI_Selected(focused_element);
				break;
			}
		}
	}
}

void UI_InteractiveGroup::ControllersNavigation()
{
	UI_Element* nearest_element = nullptr;
	UI_Element* last_focused_element = focused_element;

	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		if ((focused_controller != -1 && focused_controller != i) || app->input->IsConnectedController(i) == false)
		{
			continue;
		}

		for (uint input_dir = (uint)INPUT_DIR::RIGHT; input_dir < (uint)INPUT_DIR::MAX; ++input_dir)
		{
			if (app->input->GetControllerJoystickState(i, Joystick::LEFT, (INPUT_DIR)input_dir) == KEY_DOWN)
			{
				if (focused_element != nullptr)
				{
					nearest_element = GetNearestElement((INPUT_DIR)input_dir);
					break;
				}
				else
				{
					nearest_element = GetFirstAbleElement();
					break;
				}
			}
		}
	}

	if (nearest_element != nullptr)
	{
		focused_element = nearest_element;
	}

	if (focused_element != last_focused_element)
	{
		if (last_focused_element != nullptr)
		{
			listener->UI_OnHoverExit(last_focused_element);
			last_focused_element->is_focused = false;
		}

		if (focused_element != nullptr)
		{
			listener->UI_OnHoverEnter(focused_element);
			focused_element->is_focused = true;

			if (focus_indicator != nullptr)
			{
				focus_indicator->SetMenuState(ELEMENT_STATE::VISIBLE);
				SetFocusImage(focused_element);
			}
		}
	}
	else
	{
		if (focused_element != nullptr)
		{
			listener->UI_OnHoverRepeat(focused_element);
		}
	}

}

UI_Element*  UI_InteractiveGroup::GetNearestElement(INPUT_DIR input_dir)
{
	int distance_preference = 0;
	int current_distance = 0;
	UI_Element* nearest_element = nullptr;

	for (list<UI_Element*>::iterator iter = group_elements_list.begin(); iter != group_elements_list.end(); ++iter)
	{
		if ((*iter)->state != ELEMENT_STATE::VISIBLE || (*iter) == focused_element)
		{
			continue;
		}

		current_distance = 0;

		switch (input_dir)
		{
		case INPUT_DIR::RIGHT:
			if (focused_element->position.x >= (*iter)->position.x)
			{
				continue;
			}

			current_distance = abs(focused_element->position.x - (*iter)->position.x);
			break;
		case INPUT_DIR::LEFT:
			if (focused_element->position.x <= (*iter)->position.x)
			{
				continue;
			}

			current_distance = abs(focused_element->position.x - (*iter)->position.x);
			break;
		case INPUT_DIR::DOWN:
			if (focused_element->position.y >= (*iter)->position.y)
			{
				continue;
			}

			current_distance = abs(focused_element->position.y - (*iter)->position.y);
			break;
		case INPUT_DIR::UP:
			if (focused_element->position.y <= (*iter)->position.y)
			{
				continue;
			}

			current_distance = abs(focused_element->position.y - (*iter)->position.y);
			break;
		}

		current_distance += focused_element->position.DistanceTo((*iter)->position);

		if (distance_preference == 0 || current_distance < distance_preference)
		{
			distance_preference = current_distance;
			nearest_element = (*iter);
		}
	}

	return nearest_element;
}

UI_Element*  UI_InteractiveGroup::GetFirstAbleElement()
{
	for (list<UI_Element*>::iterator iter = group_elements_list.begin(); iter != group_elements_list.end(); ++iter)
	{
		return (*iter);
	}

	return nullptr;
}
void  UI_InteractiveGroup::Destroy()
{
	to_destroy = true;
}