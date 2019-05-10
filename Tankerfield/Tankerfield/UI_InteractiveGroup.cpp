#include "UI_InteractiveGroup.h"

UI_InteractiveGroup::UI_InteractiveGroup(const fPoint position, const UI_ElementDef definition, UI_Listener * listener) : UI_Element( position, definition, listener)
{
}

UI_InteractiveGroup::~UI_InteractiveGroup()
{
}

void UI_InteractiveGroup::AddElement(UI_Element * element)
{
}

UI_Element* UI_InteractiveGroup::GetNearestElement(UI_Element* element, CONTROLLER_DIR dir)
{
	UI_Element* nearest_element = nullptr;
	float nearest_dis = 0;

	for (std::list<UI_Element*>::iterator iter = group_elements.begin(); iter != group_elements.end(); ++iter)
	{
		if (element == (*iter))
		{
			continue;
		}

		switch (dir)
		{
		case CONTROLLER_DIR::UP:

			if ((*iter)->position.y < element->position.y)
			{
				if (nearest_dis == 0)
				{
					nearest_element = (*iter);
					nearest_dis = element->position.y - (*iter)->position.y;
				}
				else if (nearest_dis > element->position.y - (*iter)->position.y)
				{
					nearest_element = (*iter);
					nearest_dis = element->position.y - (*iter)->position.y;
				}
			}
			break;

		case CONTROLLER_DIR::DOWN:

			if ((*iter)->position.y > element->position.y)
			{
				if (nearest_dis == 0)
				{
					nearest_element = (*iter);
					nearest_dis = (*iter)->position.y - element->position.y;
				}
				else if (nearest_dis > (*iter)->position.y - element->position.y)
				{
					nearest_element = (*iter);
					nearest_dis = (*iter)->position.y - element->position.y;
				}
			}
			break;

		case CONTROLLER_DIR::RIGHT:

			if ((*iter)->position.x > element->position.x)
			{
				if (nearest_dis == 0)
				{
					nearest_element = (*iter);
					nearest_dis = element->position.x - (*iter)->position.x;
				}
				else if (nearest_dis > element->position.x - (*iter)->position.x)
				{
					nearest_element = (*iter);
					nearest_dis = element->position.x - (*iter)->position.x;
				}
			}
			break;

		case CONTROLLER_DIR::LEFT:

			if ((*iter)->position.x < element->position.x)
			{
				if (nearest_dis == 0)
				{
					nearest_element = (*iter);
					nearest_dis = element->position.x - (*iter)->position.x;
				}
				else if (nearest_dis > element->position.x - (*iter)->position.x)
				{
					nearest_element = (*iter);
					nearest_dis = element->position.x - (*iter)->position.x;
				}
			}
			break;
		}
	}

	if (nearest_element != nullptr)
	{
		return nearest_element;
	}
	else
	{
		return element;
	}
}