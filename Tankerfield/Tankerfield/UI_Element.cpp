#include "M_UI.h"
#include "UI_Element.h"
#include "M_Render.h"
#include "App.h"
#include "Log.h"

UI_Element::UI_Element(const fPoint position, const UI_ElementDef definition, UI_Listener *listener) 
	: position(position), listener(listener), section_width(definition.section_width), section_height(definition.section_height), pivot(definition.pivot), section_offset(definition.section_offset), sprite_section(definition.sprite_section){}

UI_Element::~UI_Element()
{
}

bool UI_Element::UpdateRelativePosition()
{
	if (parent_object == nullptr)
	{
		return false;
	}

	relative_position = position - parent_object->position;

	return true;
}

bool UI_Element::Draw()
{
	SDL_Rect draw_rect = GetDrawRect();
	app->render->BlitUI(app->ui->GetAtlas(), draw_rect.x, draw_rect.y, &sprite_section);

	return true;
}

void UI_Element::SetPos(const fPoint pos)
{
	position = pos;
	UpdateRelativePosition();
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

void UI_Element::SetPivot(const Pivot::POS_X x, const Pivot::POS_Y y)
{
	pivot.pos_x = x;
	pivot.pos_y = y;
	
}

fRect UI_Element::GetSection()
{
	fPoint pos;
	fRect  ret;

	if (sprite_section.w == 0 || sprite_section.h == 0)
	{
		switch (pivot.pos_x)
		{
		case Pivot::POS_X::CENTER:
			pos.x = position.x -section_width * .5f + section_offset.x;
			break;
		case Pivot::POS_X::LEFT:
			pos.x = position.x + section_offset.x;
			break;
		case Pivot::POS_X::RIGHT:
			pos.x = position.x - section_width + section_offset.x;
			break;
		}

		switch (pivot.pos_y)
		{
		case Pivot::POS_Y::CENTER:
			pos.y = position.y - section_height * .5f + section_offset.y;
			break;
		case Pivot::POS_Y::TOP:
			pos.y = position.y + section_offset.y;
			break;
		case Pivot::POS_Y::BOTTOM:
			pos.y = position.y - section_height + section_offset.y;
			break;
		}
	}
	else
	{
		switch (pivot.pos_x)
		{
		case Pivot::POS_X::CENTER:
			pos.x = position.x - (float)sprite_section.w * .5f + section_offset.x;
			break;
		case Pivot::POS_X::LEFT:
			pos.x = position.x + section_offset.x;
			break;
		case Pivot::POS_X::RIGHT:
			pos.x = position.x - (float)sprite_section.w + section_offset.x;
			break;
		}

		switch (pivot.pos_y)
		{
		case Pivot::POS_Y::CENTER:
			pos.y = position.y - (float)sprite_section.h * .5f + section_offset.y;
			break;
		case Pivot::POS_Y::TOP:
			pos.y = position.y + section_offset.y;
			break;
		case Pivot::POS_Y::BOTTOM:
			pos.y = position.y - (float)sprite_section.h + section_offset.y;
			break;
		}
	}

	ret.create(pos.x, pos.y, section_width, section_height);

	return ret;
}

SDL_Rect UI_Element::GetDrawRect()
{
	fPoint pos;
	SDL_Rect  ret;

	switch (pivot.pos_x)
	{
	case Pivot::POS_X::CENTER:
		pos.x = position.x - (float)sprite_section.w * .5f;
		break;
	case Pivot::POS_X::LEFT:
		pos.x = position.x;
		break;
	case Pivot::POS_X::RIGHT:
		pos.x = position.x - (float)sprite_section.w;
		break;
	}

	switch (pivot.pos_y)
	{
	case Pivot::POS_Y::CENTER:
		pos.y = position.y - (float)sprite_section.h * .5f;
		break;
	case Pivot::POS_Y::TOP:
		pos.y = position.y;
		break;
	case Pivot::POS_Y::BOTTOM:
		pos.y = position.y - (float)sprite_section.h;
		break;
	}

	ret = { (int)pos.x, (int) pos.y, sprite_section.w, sprite_section.h };

	return ret;
}

list<UI_Element*>* UI_Element::GetSons() 
{
	return &object_sons;
}

UI_Element * UI_Element::GetParent()
{
	return parent_object;
}
