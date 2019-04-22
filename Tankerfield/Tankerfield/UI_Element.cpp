#include "M_UI.h"
#include "UI_Element.h"
#include "M_Render.h"
#include "App.h"
#include "Log.h"
#include "M_Map.h"

UI_Element::UI_Element(const fPoint position, const UI_ElementDef definition, UI_Listener *listener) 
	: position(position), listener(listener), section_width(definition.section_width), section_height(definition.section_height),
	pivot(definition.pivot), section_offset(definition.section_offset), sprite_section(definition.sprite_section), is_in_game(definition.is_in_game),
	screen_offset(definition.screen_offset), single_camera(definition.single_camera), not_in_camera(definition.not_in_camera){}
	
UI_Element::~UI_Element()
{
}

bool UI_Element::UpdateRelativePosition()
{
	if (parent_element == nullptr)
	{
		return false;
	}

	relative_position = position - parent_element->position;

	return true;
}

bool UI_Element::Draw()
{
	SDL_Rect draw_rect = GetDrawRect();

	app->render->BlitUI(app->ui->GetAtlas(), (float)draw_rect.x, (float)draw_rect.y, &sprite_section, app->ui->current_camera, (int)alpha);

	return true;
}

void UI_Element::Destroy()
{
	to_destroy = true;
}

void UI_Element::SetPos(const fPoint pos)
{
	position = pos;
	UpdateRelativePosition();
}

void UI_Element::SetParent(UI_Element * new_parent)
{
	if (new_parent == nullptr)
	{
		LOG("Failed SetParent, parent was nullptr");
		return;
	}
	// Delete previous parent =====================
	if (parent_element != nullptr)
	{
		list<UI_Element*> *sons = parent_element->GetSons();
		list<UI_Element*>::iterator find_object = find(sons->begin(), sons->end(), this);
		
		if (find_object != sons->end())
		{
			sons->erase(find_object);
			parent_element = nullptr;
		}
		else
		{
			LOG("Failed SetParent, object as son not found");
			return;
		}
	}

	// Set Parent =================================
	parent_element = new_parent;
	relative_position = position - parent_element->position;

	// Add to parent sons =========================
	parent_element->GetSons()->push_back(this);
}

void UI_Element::SetState(ELEMENT_STATE new_state)
{
	state = new_state;
}

void UI_Element::SetStateToBranch(ELEMENT_STATE new_state)
{
	app->ui->SetStateToBranch(new_state, this);
}

//ELEMENT_STATE UI_Element::GetState()
//{
//	return ;
//}

void UI_Element::SetPivot(const Pivot::POS_X x, const Pivot::POS_Y y)
{
	pivot.pos_x = x;
	pivot.pos_y = y;
	
}

void UI_Element::SetFX(UI_Fade_FX::FX_TYPE type, float seconds, float loops, float init_value, float target_value)
{
	app->ui->AddFX(type, seconds, this, loops,init_value, target_value);
}

void UI_Element::FinishFX()
{
	if (element_fx != nullptr)
	{
		element_fx->Destroy();
	}
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
	fPoint mod_pos = { 0.f,0.f };
	fPoint rect_pos;
	SDL_Rect  ret;

	if (is_in_game == true)
	{
		mod_pos = app->map->MapToCamera(position, app->ui->current_camera);
	}
	else
	{
		mod_pos = position;
	}

	switch (pivot.pos_x)
	{
	case Pivot::POS_X::CENTER:
		rect_pos.x = mod_pos.x - (float)sprite_section.w * .5f;
		break;
	case Pivot::POS_X::LEFT:
		rect_pos.x = mod_pos.x;
		break;
	case Pivot::POS_X::RIGHT:
		rect_pos.x = mod_pos.x - (float)sprite_section.w;
		break;
	}

	switch (pivot.pos_y)
	{
	case Pivot::POS_Y::CENTER:
		rect_pos.y = mod_pos.y - (float)sprite_section.h * .5f;
		break;
	case Pivot::POS_Y::TOP:
		rect_pos.y = mod_pos.y;
		break;
	case Pivot::POS_Y::BOTTOM:
		rect_pos.y = mod_pos.y - (float)sprite_section.h;
		break;
	}

	ret = { (int)rect_pos.x, (int) rect_pos.y, sprite_section.w, sprite_section.h };

	return ret;
}

list<UI_Element*>* UI_Element::GetSons() 
{
	return &element_sons;
}

UI_Element * UI_Element::GetParent()
{
	return parent_element;
}


