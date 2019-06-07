#include "UI_InteractiveGroup.h"
#include "M_Input.h"
#include "App.h"
#include "Obj_Tank.h"
#include "Log.h"
#include "UI_Image.h"
#include <assert.h>


UI_InteractiveGroup::UI_InteractiveGroup(const fPoint position, const UI_InteractiveGroupDef definition, UI_Listener * listener) : UI_Element( position, definition, listener) , focus_indicator(definition.focus_indicator)
{
	
}

bool UI_InteractiveGroup::Update(float dt)
{



	return true;
}

bool UI_InteractiveGroup::UI_OnHoverEnter(UI_Element * element)
{
	SetFocusImage(element);

	if (listener != nullptr)
	{
		listener->UI_OnHoverEnter(element);
	}


	return true;
}


bool UI_InteractiveGroup::UI_OnHoverRepeat(UI_Element * element)
{
	if (listener != nullptr)
	{
		listener->UI_OnHoverRepeat(element);
	}

	return true;
}

bool UI_InteractiveGroup::UI_OnHoverExit(UI_Element * element)
{
	if (listener != nullptr)
	{
		listener->UI_OnHoverExit(element);
	}

	return true;
}

bool UI_InteractiveGroup::UI_Selected(UI_Element * element)
{
	if (listener != nullptr)
	{
		listener->UI_Selected(element);
	}

	return true;
}

void UI_InteractiveGroup::SetFocusImage( UI_Element* element)
{
	focus_indicator->SetPos(element->position);
	focus_indicator->SetParent(element);
}

void UI_InteractiveGroup::SetElement(UI_Element* element)
{
	if (element != nullptr)
	{
		element->SetParent(this);
		group_elements_list.push_back(element);
	}
}

std::list<UI_Element*>* UI_InteractiveGroup::GetElementsList()
{
	return &group_elements_list;
}

void  UI_InteractiveGroup::Destroy()
{
	to_destroy = true;
}