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
	name = "Module_UI";
	screen = new UI_Object({ 0,0 }, nullptr);
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
	cursor_rect = { 0, 165, 16, 24 };
	SDL_ShowCursor(SDL_DISABLE);
	return true;
}

// Called before quitting
bool Module_UI::CleanUp()
{
	LOG("Freeing all GUI objects");

	App->tex->UnLoad(atlas);
	atlas = nullptr;

	list<UI_Object*>::iterator object;
	object = objects_list.begin();

	while (object != objects_list.end())
	{
		(*object)->anchor_sons.clear();
		RELEASE((*object));
		++object;
	}
	objects_list.clear();

	return true;
}


// Update all guis
bool Module_UI::PreUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
		debug = !debug;

	 App->input->GetMousePosition(cursor_position.x, cursor_position.y);

	// Hover States ============================================
	SDL_Rect rect;

	for (list<UI_Object*>::iterator item = objects_list.begin(); item != objects_list.end(); ++item)
	{
		if ((*item)->state != ObjectState::visible)
		{
			continue;
		}

		rect.x = (*item)->position.x - (*item)->section.w * 0.5f;
		rect.y = (*item)->position.y - (*item)->section.h * 0.5f;
		rect.w = (*item)->section.w;
		rect.h = (*item)->section.h;

		if (cursor_position.x > rect.x && cursor_position.x < rect.x + rect.w && cursor_position.y > rect.y && cursor_position.y < rect.y + rect.h)
		{
			if ((*item)->hover_state == HoverState::None)
				(*item)->hover_state = HoverState::On;
			else
				(*item)->hover_state = HoverState::Repeat;
		}
		else
		{
			if ((*item)->hover_state == HoverState::On || (*item)->hover_state == HoverState::Repeat)
				(*item)->hover_state = HoverState::Out;
			else
				(*item)->hover_state = HoverState::None;
		}

		(*item)->PreUpdate();
	}

	// Click States ============================================
	
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		SelectClickedObject();
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT && clicked_object)
	{
		click_state = ClickState::Repeat;
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP && clicked_object)
	{
		click_state = ClickState::Out;
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_IDLE && clicked_object)
	{
		click_state = ClickState::None;
		clicked_object = nullptr;
	}


	return true;
}

bool Module_UI::Update(float dt)
{
	// Draggable ================================================
	if (clicked_object && clicked_object->is_draggable)
	{
		switch (click_state)
		{
		case ClickState::On:
			SetCursorOffset(cursor_position - clicked_object->GetPosition());
			break;
		case ClickState::Repeat:
			clicked_object->SetPosition(cursor_position -GetCursorOffset());
			clicked_object->UpdateRelativePosition();
			break;
		case ClickState::Out:
			SetCursorOffset({ 0,0 });
			break;
		}
	}
	// Click Callbacks =============================================

	if (clicked_object && clicked_object->listener)
	{
		switch (click_state)
		{
		case ClickState::On:
			clicked_object->listener->OnClick(clicked_object);
			break;
		case ClickState::Repeat:
			clicked_object->listener->RepeatClick(clicked_object);
			break;
		case ClickState::Out:
			if (clicked_object->hover_state != HoverState::None)
			{
				clicked_object->listener->OutClick(clicked_object);
			}
			break;
		}
	}

	// Hover Callbacks =============================================

	for (list<UI_Object*>::iterator item = objects_list.begin(); item != objects_list.end(); ++item)
	{
		if ((*item)->listener == nullptr)
		{
			if ((*item) != screen)
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

	UpdateGuiPositions(screen, iPoint(0, 0));

	
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
	DrawUI(screen);

	// Cursor =================================================
	App->input->GetMousePosition(cursor_position.x, cursor_position.y);

	if(show_cursor)
		App->render->Blit(atlas, cursor_position.x, cursor_position.y, &cursor_rect, false, 0.0F);
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

 UI_Object * Module_UI::CreateObject(iPoint position, Gui_Listener * listener)
 {
	 UI_Object* object = new UI_Object(position, listener);
	 object->SetAnchor(screen);
	 objects_list.push_back(object);
	 return object;
 }

 Label* Module_UI::CreateLabel(iPoint position, String text, _TTF_Font* font, Gui_Listener* listener, SDL_Color color )
{
	Label* object = new Label(position, text, font, color, listener);
	object->SetAnchor(screen);
	objects_list.push_back(object);
	return object;

}

Image* Module_UI::CreateImage(iPoint position, Animation animation, Gui_Listener* listener)
{
	Image* object = new Image(position, animation, atlas, listener);
	object->SetAnchor(screen);
	objects_list.push_back(object);
	return object;
}

Button* Module_UI::CreateButton(iPoint position, Button_Definition definition, Gui_Listener* listener)
{
	Button* object = new Button(position, definition, atlas, listener);
	object->SetAnchor(screen);
	objects_list.push_back(object);
	return object;
}

Slider * Module_UI::CreateSlider(iPoint position, Slider_Definition definition, Gui_Listener * listener)
{
	Slider* object = new Slider(position, definition, atlas, listener);
	object->SetAnchor(screen);
	objects_list.push_back(object);
	return object;
}

Checkbox * Module_UI::CreateCheckbox(iPoint position, Checkbox_Definition definition, Gui_Listener * listener)
{
	Checkbox* object = new Checkbox(position, definition, atlas, listener);
	object->SetAnchor(screen);
	objects_list.push_back(object);
	return object;
}

TextPanel * Module_UI::CreateTextPanel(const iPoint position, TextPanel_Definition definition, Gui_Listener * listener)
{
	TextPanel* object = new TextPanel(position, definition, listener);
	object->SetAnchor(screen);
	objects_list.push_back(object);
	return object;
}
// ====================================================================================

UI_Object * Module_UI::GetClickedObject()
{
	return clicked_object;
}

UI_Object * Module_UI::GetScreen()
{
	return screen;
}

bool Module_UI::DeleteObject(UI_Object * object)
{
	if (objects_list.empty())
	{
		return true;
	}

	if (object == nullptr)
	{
		LOG("Object not deleted: Pointing to nullptr");
		return false;
	}
	list<UI_Object*>::iterator object_to_delete;

	for (list<UI_Object*>::iterator  item = objects_list.begin(); item != objects_list.end(); ++item)
	{
		if (object == (*item))
		{
			object_to_delete = item;
		}
	}

	if ( (*object_to_delete) == nullptr)
	{
		LOG("Object not deleted: Not found");
		return false;
	}

	if ((*object_to_delete)->anchor_parent != nullptr)
	{
		list<UI_Object*> *sons_list = (*object_to_delete)->anchor_parent->GetAnchorSons();
		list<UI_Object*>::iterator item = sons_list->begin();

		while ((*item) != nullptr)
		{
			if (object == (*item))
			{
				list<UI_Object*>::iterator iterator = ++item;
				sons_list->erase(item);
				item = iterator;
			}
			else
			{
				++item;
			}
		}
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

	for (list<UI_Object*>::iterator item = branch_root->anchor_sons.begin(); item != branch_root->anchor_sons.end(); ++item)
	{
		SetStateToBranch(state, (*item));
	}

}

iPoint Module_UI::GetCursorOffset() const
{

	return cursor_offset;
}

void Module_UI::SetCursorOffset(const iPoint offset)
{
	cursor_offset = offset;

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
			for (UI_Object* iterator = (*item); iterator != nullptr ; iterator = iterator->anchor_parent)
			{
				++count;
			}

			if (count > nearest_object_position)
			{
				nearest_object_position = count;
				nearest_object = (*item);
			}
		}
		clicked_object = nearest_object;
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
			App->render->DrawQuad(rect, 255, 0, 0, 100, true, false);
		}
		else
		{
			App->render->DrawQuad(rect, 255, 100, 40, 100, true, false);
		}
	}

	for (list<UI_Object*>::iterator item = object->anchor_sons.begin();  item != object->anchor_sons.end(); ++item)
	{
		DrawUI((*item));
	}
}

void Module_UI::UpdateGuiPositions(UI_Object * object, iPoint cumulated_position)
{
	if (object == nullptr)
	{
		return;
	}

	cumulated_position += object->relative_position;
	object->position = cumulated_position;

	for (list<UI_Object*>::iterator item = object->anchor_sons.begin() ; item != object->anchor_sons.end(); ++item)
	{
		UpdateGuiPositions((*item), cumulated_position);
	}
}
