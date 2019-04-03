#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "M_UI.h"
#include "M_Window.h"
#include "M_Render.h"
#include "M_Textures.h"
#include "M_Input.h"
#include "M_Audio.h"
#include "M_Scene.h"

// UI includes --------------------------
#include "UI_Element.h"
#include "UI_Image.h"
#include "UI_Label.h"
#include "UI_Button.h"
#include "UI_Slider.h"
#include "UI_Checkbox.h"
#include "UI_TextPanel.h"

#include "Brofiler/Brofiler.h"

M_UI::M_UI() : Module()
{
	name.assign("Module UI");
	main_object = new UI_Element({ 0,0 }, UI_ElementDefinition(), nullptr);
}

// Destructor
M_UI::~M_UI()
{}

// Called before render is available
bool M_UI::Awake(pugi::xml_node& config)
{
	LOG("Loading Module UI");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool M_UI::Start()
{
	atlas = app->tex->Load("textures/ui/atlas.png");
	return true;
}

// Called before quitting
bool M_UI::CleanUp()
{
	LOG("Freeing all UI objects");

	app->tex->UnLoad(atlas);
	atlas = nullptr;

	list<UI_Element*>::iterator object;
	object = objects_list.begin();

	while (object != objects_list.end())
	{
		(*object)->object_sons.clear();
		RELEASE((*object));
		++object;
	}
	objects_list.clear();

	return true;
}


// Update all guis
bool M_UI::PreUpdate()
{
	//BROFILER_CATEGORY("M_UIPreupdate", Profiler::Color::Brown)
	int x_mouse = 0, y_mouse = 0;
	app->input->GetMousePosition(x_mouse, y_mouse);
	mouse_position = { (float)x_mouse ,(float)y_mouse };
	
	// Debug ===================================================
	if (app->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
		debug = !debug;

	// Hover States ============================================

	SDL_Rect object_rect;

	for (list<UI_Element*>::iterator item = objects_list.begin(); item != objects_list.end(); ++item)
	{
		if ((*item)->state != ObjectState::visible)
		{
			continue;
		}

		object_rect.x = (*item)->position.x - (*item)->section.w * 0.5f;
		object_rect.y = (*item)->position.y - (*item)->section.h * 0.5f;
		object_rect.w = (*item)->section.w;
		object_rect.h = (*item)->section.h;

		if (mouse_position.x >= object_rect.x && mouse_position.x <= object_rect.x + object_rect.w && mouse_position.y >= object_rect.y && mouse_position.y <= object_rect.y + object_rect.h)
		{
			if ((*item)->hover_state == HoverState::None)
			{
				(*item)->hover_state = HoverState::On;
			}
			else
			{
				(*item)->hover_state = HoverState::Repeat;
			}
		}
		else
		{
			if ((*item)->hover_state == HoverState::On || (*item)->hover_state == HoverState::Repeat)
			{
				(*item)->hover_state = HoverState::Out;
			}
			else
			{
				(*item)->hover_state = HoverState::None;
			}
		}
		(*item)->PreUpdate();
	}

	// Click States ============================================
	if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		SelectClickedObject();
	}
	else if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && selected_object)
	{
		click_state = ClickState::Repeat;
	}
	else if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && selected_object)
	{
		click_state = ClickState::Out;
	}
	else if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_IDLE && selected_object)

	{
		click_state = ClickState::None;
		selected_object = nullptr;
	}


	return true;
}

bool M_UI::Update(float dt)
{
	//BROFILER_CATEGORY("M_UIUpdate", Profiler::Color::Brown)
	// Draggable ================================================
	if (selected_object && selected_object->is_draggable)
	{
		switch (click_state)
		{
		case ClickState::On:
			SetCursorOffset(mouse_position - selected_object->GetPosition());
			break;
		case ClickState::Repeat:
			selected_object->SetPosition(mouse_position - GetMouseOffset());
			selected_object->UpdateRelativePosition();
			break;
		case ClickState::Out:
			SetCursorOffset({ 0,0 });
			break;
		}
	}
	// Click Callbacks =============================================

	if (selected_object && selected_object->listener)
	{
		switch (click_state)
		{
		case ClickState::On:
			selected_object->listener->OnClick(selected_object);
			break;
		case ClickState::Repeat:
			selected_object->listener->RepeatClick(selected_object);
			break;
		case ClickState::Out:
			if (selected_object->hover_state != HoverState::None)
			{
				selected_object->listener->OutClick(selected_object);
			}
			break;
		}
	}

	// Hover Callbacks =============================================

	for (list<UI_Element*>::iterator item = objects_list.begin(); item != objects_list.end(); ++item)
	{
		if ((*item)->listener == nullptr)
		{
			if ((*item) != main_object)
			{
				LOG("Object callback failed, listener was nullptr");
			}

			continue;
		}

		switch ((*item)->hover_state)
		{
		case HoverState::On:
			(*item)->listener->OnHover((*item));
			break;
		case HoverState::Out:
			(*item)->listener->OutHover((*item));
			break;
		case HoverState::Repeat:
			(*item)->listener->OnHover((*item));
			break;
		}
	}

	UpdateGuiPositions(main_object, fPoint(0, 0));

	
	// Update objects ==============================================

	for (list<UI_Element*>::iterator item = objects_list.begin(); item != objects_list.end(); ++item)
	{
		(*item)->Update(dt);
	}

	return true;
}

// Called after all Updates
bool M_UI::PostUpdate(float dt)
{
	// Draw all UI objects ====================================
	DrawUI(main_object);

	return true;
}

// const getter for atlas
 SDL_Texture* M_UI::GetAtlas() const 
{
	return atlas;
}

 ClickState M_UI::GetClickState() const
 {
	 return click_state;
 }

// Creation methods =================================================================

 UI_Element * M_UI::CreateObject(const fPoint position, UI_ElementDefinition definition, UI_Listener * listener)
 {
	 UI_Element* object = new UI_Element(position, definition, listener);
	 object->SetParent(main_object);
	 objects_list.push_back(object);
	 return object;
 }

 UI_Label* M_UI::CreateLabel(const fPoint position, const String text,  UI_LabelDef definition, UI_Listener* listener)
{
	UI_Label* object = new UI_Label(position, text, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;

}

UI_Image* M_UI::CreateImage(const fPoint position, UI_ImageDef definition , UI_Listener* listener)
{
	UI_Image* object = new UI_Image(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}

UI_Button* M_UI::CreateButton(const fPoint position, UI_ButtonDef definition, UI_Listener* listener)
{
	UI_Button* object = new UI_Button(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}

UI_Slider * M_UI::CreateSlider(const fPoint position, UI_SliderDef definition, UI_Listener * listener)
{
	UI_Slider* object = new UI_Slider(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}

UI_Checkbox * M_UI::CreateCheckbox(const fPoint position, UI_CheckboxDef definition, UI_Listener * listener)
{
	UI_Checkbox* object = new UI_Checkbox(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}

UI_TextPanel * M_UI::CreateTextPanel(const fPoint position, UI_TextPanelDef definition, UI_Listener * listener)
{
	UI_TextPanel* object = new UI_TextPanel(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}
// ====================================================================================

UI_Element * M_UI::GetClickedObject()
{
	return selected_object;
}

UI_Element * M_UI::GetScreen()
{
	return main_object;
}

bool M_UI::DeleteObject(UI_Element * object)
{
	if (objects_list.empty())
	{
		return false;
	}

	if (object == nullptr)
	{
		LOG("Object not deleted: Pointing to nullptr");
		return false;
	}

	// Find object to delete =====================================

	list<UI_Element*>::iterator object_to_delete = find(objects_list.begin(), objects_list.end(), object);

	if (object_to_delete == objects_list.end())
	{
		LOG("Object not deleted: Not found");
		return false;
	}

	// Delete object in parent sons list =========================

	if ((*object_to_delete)->parent_object != nullptr)
	{
		list<UI_Element*> *sons_list = (*object_to_delete)->parent_object->GetSons();
		list<UI_Element*>::iterator son_to_delete = find(sons_list->begin(), sons_list->end(), object);

		if (son_to_delete == sons_list->end())
		{
			LOG("Object not deleted: Not found");
			return false;
		}

		sons_list->erase(son_to_delete);
	}
	LOG("UI Object deleted");
	RELEASE((*object_to_delete));
	objects_list.erase(object_to_delete);

	return true;
}

void M_UI::SetStateToBranch(const ObjectState state, UI_Element * branch_root)
{
	if (branch_root == nullptr)
	{
		return;
	}

	branch_root->state = state;

	for (list<UI_Element*>::iterator item = branch_root->object_sons.begin(); item != branch_root->object_sons.end(); ++item)
	{
		SetStateToBranch(state, (*item));
	}

}

fPoint M_UI::GetMouseOffset() const
{
	return mouse_offset;
}

void M_UI::SetCursorOffset(const fPoint offset)
{
	mouse_offset = offset;

}

bool M_UI::SelectClickedObject()
{
	list<UI_Element*> clicked_objects;

	for (list<UI_Element*>::iterator item = objects_list.begin(); item != objects_list.end(); ++item)
	{
		if ((*item)->hover_state != HoverState::None  && (*item)->state == ObjectState::visible && (*item)->is_interactive == true)
		{
			clicked_objects.push_back((*item));
		}
	}

	// Select nearest object -------------------------------
	if ( ! clicked_objects.empty())
	{
		UI_Element* nearest_object = nullptr;
		int nearest_object_position = -1;

		for ( list<UI_Element*>::iterator item = clicked_objects.begin(); item != clicked_objects.end() ; ++item)
		{
			int count = 0;
			for (UI_Element* iterator = (*item); iterator != nullptr ; iterator = iterator->parent_object)
			{
				++count;
			}

			if (count > nearest_object_position)
			{
				nearest_object_position = count;
				nearest_object = (*item);
			}
		}
		selected_object = nearest_object;
		click_state = ClickState::On;
	}

	return true;
}

void M_UI::DrawUI(UI_Element * object)
{
	if (object == nullptr)
	{
		return;
	}

	if (object->state != ObjectState::hidden)
	{
		object->Draw();
	}
	
	if (debug && object->state != ObjectState::hidden)
	{
		SDL_Rect rect;
		rect.x = object->position.x - object->section.w / 2;
		rect.y = object->position.y - object->section.h / 2;
		rect.w = object->section.w;
		rect.h = object->section.h;

		if (selected_object == object)
		{
			app->render->DrawQuad(rect, 255, 233, 15, 100, true, true);
		}
		else if (object->hover_state != HoverState::None )
		{
			app->render->DrawQuad(rect, 255, 0, 0, 100, true, true);
		}
		else
		{
			app->render->DrawQuad(rect, 255, 100, 40, 100, true, true);
		}
	}

	for (list<UI_Element*>::iterator item = object->object_sons.begin();  item != object->object_sons.end(); ++item)
	{
		DrawUI((*item));
	}
}

void M_UI::UpdateGuiPositions(UI_Element * object, fPoint cumulated_position)
{
	if (object == nullptr)
	{
		return;
	}

	cumulated_position += object->relative_position;
	object->position = cumulated_position;

	for (list<UI_Element*>::iterator item = object->object_sons.begin() ; item != object->object_sons.end(); ++item)
	{
		UpdateGuiPositions((*item), cumulated_position);
	}
}
