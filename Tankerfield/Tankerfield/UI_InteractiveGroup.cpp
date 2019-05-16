#include "UI_InteractiveGroup.h"
#include "M_Input.h"
#include "App.h"
#include "Obj_Tank.h"


UI_InteractiveGroup::UI_InteractiveGroup(const fPoint position, const UI_InteractiveGroupDef definition, UI_Listener * listener) : UI_Element( position, definition, listener) , columns(definition.columns), rows(definition.rows)
{
	for (int i = 0; i < rows* columns; ++i)
	{
		group_elements.push_back(nullptr);
	}
}

UI_InteractiveGroup::~UI_InteractiveGroup()
{

}

bool UI_InteractiveGroup::Update(float dt)
{

	for (std::vector<Obj_Tank*>::iterator iterator = app->objectmanager->obj_tanks.begin(); iterator != app->objectmanager->obj_tanks.end(); ++iterator)
	{
		Controller* controller = (*iterator)->GetController();

		(*iterator)->GetTankNum();

		if (controller == nullptr)
		{
			continue;
		}
		else
		{

		}
	}

	return true;
}

void UI_InteractiveGroup::SetElement(UI_Element* element, const iPoint position)
{
	group_elements[position.y * columns + position.x] = element;
}

UI_Element* UI_InteractiveGroup::GetNearestElement(PlayerFocus * player_focus, CONTROLLER_DIR dir)
{
	UI_Element* current_element = GetElement( player_focus->focused_element);
	UI_Element* focused_element = nullptr;

	switch (dir)
	{
	case CONTROLLER_DIR::UP:


		break;

	case CONTROLLER_DIR::DOWN:


		break;

	case CONTROLLER_DIR::RIGHT:


		break;

	case CONTROLLER_DIR::LEFT:

		break;
	}

	if ( ElementIsFocused(focused_element) == false)
	{
		return focused_element;
	}
	else
	{
		return  current_element;
	}
}

bool UI_InteractiveGroup::ElementIsFocused(UI_Element * element)
{
	for (std::vector<PlayerFocus*>::iterator iter = player_focus.begin(); iter != player_focus.end(); ++iter)
	{

	}

	return false;
}

UI_Element* UI_InteractiveGroup::GetElement(iPoint position)
{
	return group_elements[ position.y * columns + position.x];
}