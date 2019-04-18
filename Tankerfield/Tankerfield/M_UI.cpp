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
#include "M_AnimationBank.h"

#include "Player_GUI.h"

// UI includes --------------------------
#include "UI_Element.h"
#include "UI_Image.h"
#include "UI_Label.h"
#include "UI_Button.h"
#include "UI_Slider.h"
#include "UI_Checkbox.h"
#include "UI_TextPanel.h"
#include "UI_InGameElement.h"
#include "UI_Bar.h"


M_UI::M_UI() : Module()
{
	name.assign("ui");
	main_object = DBG_NEW UI_Element({ 0,0 }, UI_ElementDef(), nullptr);
}

// Destructor
M_UI::~M_UI()
{
	if (main_object != nullptr)
	{
		delete main_object;
		main_object = nullptr;
	}
}

// Called before render is available
bool M_UI::Awake(pugi::xml_node& config)
{
	LOG("Loading Module UI");
	bool ret = true;

	arrow_anim.frames = app->anim_bank->LoadFrames(config.child("animations").child("arrow"));

	return ret;
}

// Called before the first frame
bool M_UI::Start()
{
	atlas = app->tex->Load("textures/ui/atlas.png");

	// Position ======================================
	fRect full_screen =  app->win->GetWindowRect();


	// HUD ===========================================
	hud_player_1 = DBG_NEW Player_GUI(Player_GUI::TYPE::PLAYER_1, app->scene->tank_1);
	hud_player_2 = DBG_NEW Player_GUI(Player_GUI::TYPE::PLAYER_2, app->scene->tank_2);
	hud_player_3 = DBG_NEW Player_GUI(Player_GUI::TYPE::PLAYER_3, app->scene->tank_3);
	hud_player_4 = DBG_NEW Player_GUI(Player_GUI::TYPE::PLAYER_4, app->scene->tank_4);

	UI_InGameElementDef test_def;
	test_def.object = (Object*)app->scene->tank_2;
	test_def.player_gui = hud_player_1;

	app->ui->CreateInGameElement({0.f, 0.f}, test_def);

	UI_ImageDef image_def;

	// General 4 players =========================================================
	image_def.sprite_section = { 170, 10, 50, 50 };
	UI_Image* lt_round = CreateImage({ full_screen.w * .5f ,  full_screen.h * .5f }, image_def);
	lt_round->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::BOTTOM);

	image_def.sprite_section = { 220, 10, 50, 50 };
	UI_Image* rt_round = CreateImage({ full_screen.w * .5f ,  full_screen.h * .5f }, image_def);
	rt_round->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::BOTTOM);

	image_def.sprite_section = { 170, 60, 50, 50 };
	UI_Image* lb_round = CreateImage({ full_screen.w * .5f ,  full_screen.h * .5f }, image_def);
	lb_round->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::TOP);

	image_def.sprite_section = { 220, 60, 50, 50 };
	UI_Image* rb_round = CreateImage({ full_screen.w * .5f ,  full_screen.h * .5f }, image_def);
	rb_round->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::TOP);

	image_def.sprite_section = { 10, 160, 50, 530 };
	UI_Image* left_tank_life = CreateImage({ 0.f ,  full_screen.h * .5f }, image_def);
	left_tank_life->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::CENTER);

	image_def.sprite_section = { 60, 160, 50, 530 };
	UI_Image* right_tank_life = CreateImage({ full_screen.w ,  full_screen.h * .5f }, image_def);
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
			continue;
		}

		switch ((*item)->hover_state)
		{
		case HoverState::ENTER:
			(*item)->listener->OnHover((*item));
			break;
		case HoverState::REPEAT:
			(*item)->listener->OnHover((*item));
			break;
		case HoverState::EXIT:
			(*item)->listener->OutHover((*item));
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
	fRect full_screen = app->win->GetWindowRect();

	app->render->DrawQuad({(int) (full_screen.w * .5f) - 3,  0, 6, (int)full_screen.h }, 150, 150, 150, 255, true, false);
	app->render->DrawQuad({ 0 ,(int)(full_screen.h * .5f) - 3, (int)full_screen.w, 6 }, 150, 150, 150, 255, true, false);

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

 UI_Element * M_UI::CreateObject(const fPoint position, const UI_ElementDef definition, UI_Listener * listener)
 {
	 UI_Element* object = DBG_NEW UI_Element(position, definition, listener);
	 object->SetParent(main_object);
	 objects_list.push_back(object);
	 return object;
 }

 UI_Label* M_UI::CreateLabel(const fPoint position, const String text,  UI_LabelDef definition, UI_Listener* listener)
{
	UI_Label* object = DBG_NEW UI_Label(position, text, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;

}

UI_Image* M_UI::CreateImage(const fPoint position, const UI_ImageDef definition , UI_Listener* listener)
{
	UI_Image* object = DBG_NEW UI_Image(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}

UI_Button* M_UI::CreateButton(const fPoint position, const UI_ButtonDef definition, UI_Listener* listener)
{
	UI_Button* object = DBG_NEW UI_Button(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}

UI_Slider * M_UI::CreateSlider(const fPoint position, const UI_SliderDef definition, UI_Listener * listener)
{
	UI_Slider* object = DBG_NEW UI_Slider(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}

UI_Checkbox * M_UI::CreateCheckbox(const fPoint position, const UI_CheckboxDef definition, UI_Listener * listener)
{
	UI_Checkbox* object = DBG_NEW UI_Checkbox(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}

UI_TextPanel * M_UI::CreateTextPanel(const fPoint position, const UI_TextPanelDef definition, UI_Listener * listener)
{
	UI_TextPanel* object = DBG_NEW UI_TextPanel(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}
UI_Bar * M_UI::CreateBar(const fPoint position, const UI_BarDef definition, UI_Listener * listener)
{
	UI_Bar* object = DBG_NEW UI_Bar(position, definition, listener);
	object->SetParent(main_object);
	objects_list.push_back(object);
	return object;
}

UI_InGameElement*  M_UI::CreateInGameElement(const fPoint position, const UI_InGameElementDef definition, UI_Listener * listener)
{

	UI_InGameElement* object = DBG_NEW UI_InGameElement(position, definition, listener);
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
