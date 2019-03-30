#include "M_UI.h"
#include "UI_Element.h"
#include "App.h"
#include "Log.h"

UI_Element::UI_Element(const fPoint position, UI_ElementDefinition definition, UI_Listener *listener) 
	: position(position), listener(listener), section(definition.section), pivot(definition.pivot), draw_offset(definition.draw_offset) {}

UI_Element::~UI_Element()
{
}

fPoint UI_Element::GetPosition() const
{
	return position;
}

void UI_Element::SetPosition(const fPoint position)
{
	this->position = position;
	UpdateRelativePosition();
}

bool UI_Element::UpdateRelativePosition()
{
	if (parent_object == nullptr)
	{
		return false;
	}

	relative_position = position - parent_object->GetPosition();
	return true;
}

bool UI_Element::SetParent(UI_Element * parent)
{
	if (parent == nullptr)
	{
		LOG("Failed SetParent, parent was nullptr");
		return false;
	}
	// Delete previous parent =====================
	if (parent_object)
	{
		list<UI_Element*> *sons = parent_object->GetSons();
		list<UI_Element*>::iterator find_object = find(sons->begin(), sons->end(), this);
		
		if (find_object != sons->end())
		{
			sons->erase(find_object);
			parent_object = nullptr;
		}
		else
		{
			LOG("Failed SetParent, object as son not found");
			return false;
		}
	}

	// Set Parent =================================
	parent_object = parent;
	relative_position = position - parent_object->position;

	// Add to parent sons =========================
	parent_object->GetSons()->push_back(this);

	return true;
}

void UI_Element::SetPivot(const PivotPos new_pivot)
{
	pivot = new_pivot;
	//switch (new_pivot)
	//{
	//default:
	//	break:
	//}
}

list<UI_Element*>* UI_Element::GetSons() 
{
	return &object_sons;
}

UI_Element * UI_Element::GetParent()
{
	return parent_object;
}

void UI_Element::IsDraggable(const bool is_draggable)
{
	this->is_draggable = is_draggable;
}

void UI_Element::SetState(const ObjectState state)
{
	this->state = state;
}


