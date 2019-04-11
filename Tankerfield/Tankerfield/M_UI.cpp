#include "Brofiler/Brofiler.h"

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
#include "UI_Bar.h"


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


	// Position ======================================

	uint u_screen_width = 0.f, u_screen_height = 0.f;
	app->win->GetWindowSize(u_screen_width, u_screen_height);
	float screen_width = (float)u_screen_width, screen_height = (float)u_screen_height;

	fPoint margin = { 30.f, 30.f };
	fRect full_screen;
	full_screen.create(0.f, 0.f, screen_width, screen_height);
	fRect splited_screen;
	splited_screen.create(0.f, 0.f, screen_width, screen_height);

	// HUD ===========================================

	UI_ImageDef image_def;


	// Individual player ========================================================

	image_def.sprite_section = { 100, 10, 50, 50 };
	UI_Image* basic_weapon_frame = CreateImage({ splited_screen.GetLeft() + margin.x , splited_screen.GetTop() + margin.y}, image_def, this);
	basic_weapon_frame->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::UP);

	image_def.sprite_section = { 100, 70, 70, 70 };
	UI_Image* item_frame = CreateImage({ splited_screen.GetLeft() + margin.x + 25.f ,splited_screen.GetTop() + margin.y + 90.f }, image_def, this);
	item_frame->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);

	image_def.sprite_section = { 10, 70, 50, 20 };
	UI_Image* ammo_image = CreateImage({ splited_screen.GetRight() - 24.f - margin.x , splited_screen.GetTop() + 50.f + margin.y }, image_def, this);
	ammo_image->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::UP);

	image_def.sprite_section = { 10, 10, 50, 50 };
	UI_Image* special_weapon_frame = CreateImage({ splited_screen.GetRight() - 24.f - margin.x ,splited_screen.GetTop() + margin.y }, image_def, this);
	special_weapon_frame->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::UP);

	UI_BarDef ammo_bar_def(UI_Bar::DIR::DOWN, 0.8f, { 180, 160, 0, 255 }, { 150, 150, 150, 255 });
	ammo_bar_def.section_width = 12.f;
	ammo_bar_def.section_height = 128.f;
	ammo_bar_def.section_offset = { 6.f ,6.f };
	ammo_bar_def.sprite_section = { 70, 10, 24, 140 };

	UI_Bar* ammo_bar = CreateBar({ splited_screen.GetRight() - margin.x , splited_screen.GetTop() + margin.y }, ammo_bar_def, this);
	ammo_bar->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::UP);

	UI_BarDef life_bar_def (UI_Bar::DIR::UP, 0.8f, { 0, 160, 0, 255 }, { 150, 150, 150, 255 });
	life_bar_def.section_width = 20.f;
	life_bar_def.section_height = 234.f;

	UI_Bar* life_bar = CreateBar({ splited_screen.GetLeft() + 10.f, splited_screen.GetBottom() - 21.f }, life_bar_def, this);
	life_bar->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::DOWN);

	// General 4 players =========================================================

	image_def.sprite_section = { 10, 160, 50, 530 };
	UI_Image* left_tank_life = CreateImage({ full_screen.GetLeft() , full_screen.GetBottom() }, image_def, this);
	left_tank_life->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::CENTER);

	image_def.sprite_section = { 60, 160, 50, 530 };
	UI_Image* right_tank_life = CreateImage({ full_screen.GetRight() , full_screen.GetBottom() }, image_def, this);
	right_tank_life->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::CENTER);

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
	BROFILER_CATEGORY("M_UIPreupdate", Profiler::Color::Brown)
	int x_mouse = 0, y_mouse = 0;
	app->input->GetMousePosition(x_mouse, y_mouse);
	mouse_position = { (float)x_mouse ,(float)y_mouse };
	
	// Debug ===================================================
	if (app->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
		debug = !debug;

	// Hover States ============================================

	fRect section;

	for (list<UI_Element*>::iterator item = objects_list.begin(); item != objects_list.end(); ++item)
	{
		if ((*item)->state != ELEMENT_STATE::VISIBLE || (*item)->section_width == 0.f || (*item)->section_height == 0.f)
		{
			continue;
		}

		section = (*item)->GetSection();
 
		if (mouse_position.x >= section.GetLeft() && mouse_position.x <= section.GetRight() && mouse_position.y >= section.GetTop() && mouse_position.y <= section.GetBottom())
		{
			if ((*item)->hover_state == HoverState::NONE)
			{
				(*item)->hover_state = HoverState::ENTER;
			}
			else
			{
				(*item)->hover_state = HoverState::REPEAT;
			}
		}
		else
		{
			if ((*item)->hover_state == HoverState::ENTER || (*item)->hover_state == HoverState::REPEAT)
			{
				(*item)->hover_state = HoverState::EXIT;
			}
			else
			{
				(*item)->hover_state = HoverState::NONE;
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
		click_state = ClickState::REPEAT;
	}
	else if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && selected_object)
	{
		click_state = ClickState::EXIT;
	}
	else if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_IDLE && selected_object)

	{
		click_state = ClickState::NONE;

		selected_object = nullptr;
	}


	return true;
}

bool M_UI::Update(float dt)
{
	BROFILER_CATEGORY("M_UIUpdate", Profiler::Color::Brown)
	// Draggable ================================================
	if (selected_object && selected_object->is_draggable)
	{
		switch (click_state)
		{
		case ClickState::ENTER:
			mouse_offset = mouse_position - selected_object->position;
			break;
		case ClickState::REPEAT:
			selected_object->position  = mouse_position - mouse_offset;
			selected_object->UpdateRelativePosition();
			break;
		case ClickState::EXIT:
			mouse_offset = { 0,0 };
			break;
		}
	}
	// Click Callbacks =============================================

	if (selected_object && selected_object->listener)
	{
		switch (click_state)
		{
		case ClickState::ENTER:
			selected_object->listener->OnClick(selected_object);
			break;
		case ClickState::REPEAT:
			selected_object->listener->RepeatClick(selected_object);
			break;
		case ClickState::EXIT:
			if (selected_object->hover_state != HoverState::NONE)
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
		case HoverState::ENTER:
			(*item)->listener->OnHover((*item));
			break;
		case HoverState::EXIT:
			(*item)->listener->OutHover((*item));
			break;
		case HoverState::REPEAT:
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

	// Debug Positions  =======================================
	if (debug)
	{
		for (list<UI_Element*>::iterator item = objects_list.begin(); item != objects_list.end(); ++item)
		{
			if ((*item) != main_object)
			{
				app->render->DrawQuad({ (int)(*item)->position.x - 3 , (int)(*item)->position.y - 3,  6, 6 }, 255, 0, 0, 255, true, false);
			}
		}
	}
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

 UI_Element * M_UI::CreateObject(const fPoint position, const UI_ElementDefinition definition, UI_Listener * listener)
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

UI_Image* M_UI::CreateImage(const fPoint position, const UI_ImageDef definition , UI_Listener* listener)
{
	UI_Image* object = new UI_Image(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}

UI_Button* M_UI::CreateButton(const fPoint position, const UI_ButtonDef definition, UI_Listener* listener)
{
	UI_Button* object = new UI_Button(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}

UI_Slider * M_UI::CreateSlider(const fPoint position, const UI_SliderDef definition, UI_Listener * listener)
{
	UI_Slider* object = new UI_Slider(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}

UI_Checkbox * M_UI::CreateCheckbox(const fPoint position, const UI_CheckboxDef definition, UI_Listener * listener)
{
	UI_Checkbox* object = new UI_Checkbox(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}

UI_TextPanel * M_UI::CreateTextPanel(const fPoint position, const UI_TextPanelDef definition, UI_Listener * listener)
{
	UI_TextPanel* object = new UI_TextPanel(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}
UI_Bar * M_UI::CreateBar(const fPoint position, const UI_BarDef definition, UI_Listener * listener)
{
	UI_Bar* object = new UI_Bar(position, definition, listener);
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

void M_UI::SetStateToBranch(const ELEMENT_STATE state, UI_Element * branch_root)
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

bool M_UI::SelectClickedObject()
{
	list<UI_Element*> clicked_objects;

	for (list<UI_Element*>::iterator item = objects_list.begin(); item != objects_list.end(); ++item)
	{
		if ((*item)->hover_state != HoverState::NONE  && (*item)->state == ELEMENT_STATE::VISIBLE && (*item)->is_interactive == true)
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
		click_state = ClickState::ENTER;
	}

	return true;
}

void M_UI::DrawUI(UI_Element * object)
{
	if (object == nullptr)
	{
		return;
	}

	if (object->state != ELEMENT_STATE::HIDDEN)
	{
		object->Draw();
	}
	
	if (debug && object->state != ELEMENT_STATE::HIDDEN && object->is_interactive == true)
	{
		SDL_Rect rect = (SDL_Rect)object->GetSection();

		if (selected_object == object)
		{
			app->render->DrawQuad(rect, 255, 233, 15, 100, true, false);
		}
		else if (object->hover_state != HoverState::NONE )
		{
			app->render->DrawQuad(rect, 255, 0, 0, 100, true, false);
		}
		else
		{
			app->render->DrawQuad(rect, 255, 100, 40, 100, true, false);
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
