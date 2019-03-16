#include "Module_UI.h"
#include "UI_Object.h"
#include "j1App.h"
#include "p2Log.h"

UI_Object::UI_Object(const iPoint position, Gui_Listener *listener)
{
	this->position = position;
	this->listener = listener;

	section.x = section.y = 0;
	section.w = DEFAULT_MARGIN_SECTION;
	section.h = DEFAULT_MARGIN_SECTION;
}

UI_Object::~UI_Object()
{
}

iPoint UI_Object::GetPosition() const
{
	return position;
}

void UI_Object::SetPosition(const iPoint position) 
{
	this->position = position;
	UpdateRelativePosition();
}

bool UI_Object::UpdateRelativePosition()
{
	if (anchor_parent == nullptr)
	{
		return false;
	}

	relative_position = position - anchor_parent->GetPosition();
	return true;
}

bool UI_Object::SetAnchor(UI_Object * anchor)
{
	if (anchor == nullptr)
	{
		LOG("Failed SetAnchor, anchor was nullptr");
		return false;
	}
	// Delete previous parent =====================
	if (anchor_parent)
	{
		list<UI_Object*> *sons = anchor_parent->GetAnchorSons();
		list<UI_Object*>::iterator find_object = find(sons->begin(), sons->end(), this);
		
		if (find_object != sons->end())
		{
			sons->erase(find_object);
			anchor_parent = nullptr;
		}
		else
		{
			LOG("Failed SetAnchor, object as son not found");
			return false;
		}
	}

	// Set Parent =================================
	anchor_parent = anchor;

	relative_position = position - anchor_parent->position;

	// Add to parent sons =========================
	anchor_parent->GetAnchorSons()->push_back(this);

	return true;
}

list<UI_Object*>* UI_Object::GetAnchorSons() 
{
	return &anchor_sons;
}

UI_Object * UI_Object::GetAnchorParent()
{
	return anchor_parent;
}

void UI_Object::IsDraggable(const bool is_draggable)
{
	this->is_draggable = is_draggable;
}

void UI_Object::SetState(const ObjectState state)
{
	this->state = state;
}


