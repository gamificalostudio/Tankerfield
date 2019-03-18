#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "Module_UI.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Audio.h"
#include "j1Scene.h"

// UI includes --------------------------
#include "UI_Object.h"
#include "Image.h"
#include "Label.h"
#include "Button_Input.h"
#include "Slider.h"
#include "Checkbox.h"
#include "Text_Panel.h"

Module_UI::Module_UI() : j1Module()
{
	name.assign("Module UI");
	main_object = new UI_Object({ 0,0 }, UI_Object_Definition(), nullptr);
}

// Destructor
Module_UI::~Module_UI()
{}

// Called before render is available
bool Module_UI::Awake(pugi::xml_node& config)
{
	LOG("Loading Module UI");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Module_UI::Start()
{
	atlas = App->tex->Load("textures/ui/atlas.png");
	return true;
}

// Called before quitting
bool Module_UI::CleanUp()
{
	LOG("Freeing all UI objects");

	App->tex->UnLoad(atlas);
	atlas = nullptr;

	list<UI_Object*>::iterator object;
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
bool Module_UI::PreUpdate()
{
	int x_mouse = 0, y_mouse = 0;
	App->input->GetMousePosition(x_mouse, y_mouse);
	mouse_position = { (float)x_mouse ,(float)y_mouse };
	
	// Debug ===================================================
	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
		debug = !debug;

	// Hover States ============================================

	SDL_Rect object_rect;

	for (list<UI_Object*>::iterator item = objects_list.begin(); item != objects_list.end(); ++item)
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
	
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		SelectClickedObject();
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT && selected_object)
	{
		click_state = ClickState::Repeat;
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP && selected_object)
	{
		click_state = ClickState::Out;
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_IDLE && selected_object)
	{
		click_state = ClickState::None;
		selected_object = nullptr;
	}


	return true;
}

bool Module_UI::Update(float dt)
{
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

	for (list<UI_Object*>::iterator item = objects_list.begin(); item != objects_list.end(); ++item)
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

	for (list<UI_Object*>::iterator item = objects_list.begin(); item != objects_list.end(); ++item)
	{
		(*item)->Update(dt);
	}

	return true;
}

// Called after all Updates
bool Module_UI::PostUpdate()
{
	// Draw all UI objects ====================================
	DrawUI(main_object);

	return true;
}

// const getter for atlas
 SDL_Texture* Module_UI::GetAtlas() const 
{
	return atlas;
}

 ClickState Module_UI::GetClickState() const
 {
	 return click_state;
 }

// Creation methods =================================================================

 UI_Object * Module_UI::CreateObject(const fPoint position, UI_Object_Definition definition, Gui_Listener * listener)
 {
	 UI_Object* object = new UI_Object(position, definition, listener);
	 object->SetParent(main_object);
	 objects_list.push_back(object);
	 return object;
 }

 Label* Module_UI::CreateLabel(const fPoint position, const String text,  Label_Definition definition, Gui_Listener* listener)
{
	Label* object = new Label(position, text, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;

}

Image* Module_UI::CreateImage(const fPoint position, Image_Definition definition , Gui_Listener* listener)
{
	Image* object = new Image(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}

Button* Module_UI::CreateButton(const fPoint position, Button_Definition definition, Gui_Listener* listener)
{
	Button* object = new Button(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}

Slider * Module_UI::CreateSlider(const fPoint position, Slider_Definition definition, Gui_Listener * listener)
{
	Slider* object = new Slider(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}

Checkbox * Module_UI::CreateCheckbox(const fPoint position, Checkbox_Definition definition, Gui_Listener * listener)
{
	Checkbox* object = new Checkbox(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}

TextPanel * Module_UI::CreateTextPanel(const fPoint position, TextPanel_Definition definition, Gui_Listener * listener)
{
	TextPanel* object = new TextPanel(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}
// ====================================================================================

UI_Object * Module_UI::GetClickedObject()
{
	return selected_object;
}

UI_Object * Module_UI::GetScreen()
{
	return main_object;
}

bool Module_UI::DeleteObject(UI_Object * object)
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

	list<UI_Object*>::iterator object_to_delete = find(objects_list.begin(), objects_list.end(), object);

	if (object_to_delete == objects_list.end())
	{
		LOG("Object not deleted: Not found");
		return false;
	}

	// Delete object in parent sons list =========================

	if ((*object_to_delete)->parent_object != nullptr)
	{
		list<UI_Object*> *sons_list = (*object_to_delete)->parent_object->GetSons();
		list<UI_Object*>::iterator son_to_delete = find(sons_list->begin(), sons_list->end(), object);

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

void Module_UI::SetStateToBranch(const ObjectState state, UI_Object * branch_root)
{
	if (branch_root == nullptr)
	{
		return;
	}

	branch_root->state = state;

	for (list<UI_Object*>::iterator item = branch_root->object_sons.begin(); item != branch_root->object_sons.end(); ++item)
	{
		SetStateToBranch(state, (*item));
	}

}

fPoint Module_UI::GetMouseOffset() const
{
	return mouse_offset;
}

void Module_UI::SetCursorOffset(const fPoint offset)
{
	mouse_offset = offset;

}

bool Module_UI::SelectClickedObject()
{
	list<UI_Object*> clicked_objects;

	for (list<UI_Object*>::iterator item = objects_list.begin(); item != objects_list.end(); ++item)
	{
		if ((*item)->hover_state != HoverState::None  && (*item)->state == ObjectState::visible && (*item)->is_interactive == true)
		{
			clicked_objects.push_back((*item));
		}
	}

	// Select nearest object -------------------------------
	if ( ! clicked_objects.empty())
	{
		UI_Object* nearest_object = nullptr;
		int nearest_object_position = -1;

		for ( list<UI_Object*>::iterator item = clicked_objects.begin(); item != clicked_objects.end() ; ++item)
		{
			int count = 0;
			for (UI_Object* iterator = (*item); iterator != nullptr ; iterator = iterator->parent_object)
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

void Module_UI::DrawUI(UI_Object * object)
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

		if (object->hover_state != HoverState::None )
		{
			App->render->DrawQuad(rect, 255, 0, 0, 100, true, true);
		}
		else
		{
			App->render->DrawQuad(rect, 255, 100, 40, 100, true, true);
		}
	}

	for (list<UI_Object*>::iterator item = object->object_sons.begin();  item != object->object_sons.end(); ++item)
	{
		DrawUI((*item));
	}
}

void Module_UI::UpdateGuiPositions(UI_Object * object, fPoint cumulated_position)
{
	if (object == nullptr)
	{
		return;
	}

	cumulated_position += object->relative_position;
	object->position = cumulated_position;

	for (list<UI_Object*>::iterator item = object->object_sons.begin() ; item != object->object_sons.end(); ++item)
	{
		UpdateGuiPositions((*item), cumulated_position);
	}
}
