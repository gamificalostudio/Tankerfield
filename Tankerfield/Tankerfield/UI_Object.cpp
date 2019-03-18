#include "Module_UI.h"
#include "UI_Object.h"
#include "j1App.h"
#include "p2Log.h"

UI_Object::UI_Object(const fPoint position, UI_Object_Definition definition, Gui_Listener *listener) : position(position), listener(listener)
{
	section.x = section.y = 0;
	section.w = definition.section_mesures.w;
	section.h = definition.section_mesures.h;
	draw_offset = definition.draw_offset;
}

UI_Object::~UI_Object()
{
}

fPoint UI_Object::GetPosition() const
{
	return position;
}

void UI_Object::SetPosition(const fPoint position)
{
	this->position = position;
	UpdateRelativePosition();
}

bool UI_Object::UpdateRelativePosition()
{
	if (parent_object == nullptr)
	{
		return false;
	}

	relative_position = position - parent_object->GetPosition();
	return true;
}

bool UI_Object::SetParent(UI_Object * parent)
{
	if (parent == nullptr)
	{
		LOG("Failed SetParent, parent was nullptr");
		return false;
	}
	// Delete previous parent =====================
	if (parent_object)
	{
		list<UI_Object*> *sons = parent_object->GetSons();
		list<UI_Object*>::iterator find_object = find(sons->begin(), sons->end(), this);
		
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

list<UI_Object*>* UI_Object::GetSons() 
{
	return &object_sons;
}

UI_Object * UI_Object::GetParent()
{
	return parent_object;
}

void UI_Object::IsDraggable(const bool is_draggable)
{
	this->is_draggable = is_draggable;
}

void UI_Object::SetState(const ObjectState state)
{
	this->state = state;
}


