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
#include "M_Fonts.h"
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
#include "UI_Quad.h"

#include <vector>


M_UI::M_UI() : Module()
{
	name.assign("ui");
	main_ui_element = DBG_NEW UI_Element({ 0,0 }, UI_ElementDef(), nullptr);
	main_in_game_element = DBG_NEW UI_Element({ 0,0 }, UI_ElementDef(), nullptr);
}

// Destructor
M_UI::~M_UI()
{
	if (main_ui_element != nullptr)
	{
		delete main_ui_element;
		main_ui_element = nullptr;
	}

	if (main_in_game_element != nullptr)
	{
		delete main_in_game_element;
		main_in_game_element = nullptr;
	}
}

// Called before render is available
bool M_UI::Awake(pugi::xml_node& config)
{
	LOG("Loading Module UI");
	bool ret = true;

	green_arrow_anim.frames = app->anim_bank->LoadFrames(config.child("animations").child("green_arrow"));
	pink_arrow_anim.frames = app->anim_bank->LoadFrames(config.child("animations").child("pink_arrow"));
	blue_arrow_anim.frames = app->anim_bank->LoadFrames(config.child("animations").child("blue_arrow"));
	orange_arrow_anim.frames = app->anim_bank->LoadFrames(config.child("animations").child("orange_arrow"));

	button_sprites[(int)CONTROLLER_BUTTON::A] =  { 440,10 ,50 ,50 };
	button_sprites[(int)CONTROLLER_BUTTON::B] =  { 390,60 ,50 ,50 };
	button_sprites[(int)CONTROLLER_BUTTON::Y] =  { 440,60 ,50 ,50 };
	button_sprites[(int)CONTROLLER_BUTTON::X] =  { 390,10 ,50 ,50 };
	button_sprites[(int)CONTROLLER_BUTTON::L] =  { 495,10 ,52 ,52 };
	button_sprites[(int)CONTROLLER_BUTTON::LT] = { 280,10 ,50 ,50 };
	button_sprites[(int)CONTROLLER_BUTTON::LB] = { 280,60 ,50 ,50 };
	button_sprites[(int)CONTROLLER_BUTTON::R] =  { 495,65 ,52 ,52 };
	button_sprites[(int)CONTROLLER_BUTTON::RT] = { 330,10 ,50 ,50 };
	button_sprites[(int)CONTROLLER_BUTTON::RB] = { 330,60 ,50 ,50 };

	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_DOUBLE_MISSILE] = { 500,500 ,34 ,34 };
	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_FLAMETHROWER] =   { 540,500 ,34 ,34 };
	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_HEALING_SHOT] =   { 620,500 ,34 ,34 };
	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_LASER] =          { 580,500 ,34 ,34 };

	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_DOUBLE_MISSILE] = { 500,595,44 ,44 };
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_FLAMETHROWER] =   { 550,595,44 ,44 };
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_HEALING_SHOT] =   { 650,595,44 ,44 };
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_LASER] =          { 600,595,44 ,44 };
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_BASIC] =          { 700,595,44 ,44 };

	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::ITEM_HEALTH_BAG] = { 500,545 ,40 ,40 };
	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::ITEM_HAPPY_HOUR] = { 545,545 ,40 ,40 };
	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::ITEM_INSTANT_HELP] = { 390,545 ,40 ,40 };

	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::ITEM_HEALTH_BAG] = { 500,650 ,47 ,47 };
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::ITEM_HAPPY_HOUR] = { 550,650 ,47 ,47 };
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::ITEM_INSTANT_HELP] = { 390,650 ,47 ,47 };

	return ret;
}

// Called before the first frame
bool M_UI::Start()
{
	// Assets ========================================
	btw_focus_timer.Start();
	atlas = app->tex->Load("textures/ui/atlas.png");

	if (main_in_game_element == nullptr)
	{
		main_in_game_element = DBG_NEW UI_Element({ 0,0 }, UI_ElementDef(), nullptr);
	}
	if (main_ui_element == nullptr)
	{
		main_ui_element = DBG_NEW UI_Element({ 0,0 }, UI_ElementDef(), nullptr);
	}

	return true;
}

// Called before quitting
bool M_UI::CleanUp()
{
	LOG("Freeing all UI objects");

	app->tex->UnLoad(atlas);
	atlas = nullptr;
	selected_element = nullptr;
	interactive_elements.clear();

	for (list < Player_GUI*> ::iterator gui = players_guis.begin(); gui != players_guis.end(); ++gui)
	{
		RELEASE((*gui));
	}
	players_guis.clear();

	for (list < UI_Element*> ::iterator element = ig_elements_list.begin(); element != ig_elements_list.end(); ++element)
	{
		RELEASE((*element));
	}
	ig_elements_list.clear();

	for (list < UI_Element*> ::iterator element = elements_list.begin(); element != elements_list.end(); ++element)
	{
		RELEASE((*element));
	}
	elements_list.clear();

	for (std::list<UI_Fade_FX*>::iterator iter = active_fxs.begin(); iter != active_fxs.end(); ++iter)
	{
		RELEASE((*iter));
	}

	active_fxs.clear();

	return true;
}

bool M_UI::Reset()
{
	selected_element = nullptr;

	interactive_elements.clear();

	for (list < Player_GUI*> ::iterator gui = players_guis.begin(); gui != players_guis.end(); ++gui)
	{
		RELEASE((*gui));
	}
	players_guis.clear();

	for (std::list<UI_Fade_FX*>::iterator iter = active_fxs.begin(); iter != active_fxs.end(); ++iter)
	{
		RELEASE((*iter));
	}
	active_fxs.clear();

	for (list < UI_Element*> ::iterator element = ig_elements_list.begin(); element != ig_elements_list.end(); ++element)
	{
		if ((*element) != main_in_game_element)
		{
			(*element)->Destroy();
		}
	}

	for (list < UI_Element*> ::iterator element = elements_list.begin(); element != elements_list.end(); ++element)
	{
		if ((*element) != main_ui_element)
		{
			(*element)->Destroy();
		}
	}

	return true;
}

Player_GUI * M_UI::AddPlayerGUI(GUI_TYPE type, Obj_Tank * player)
{
	Player_GUI* player_gui = DBG_NEW Player_GUI(type, player);
	player->SetGui(player_gui);
	players_guis.push_back(player_gui);
	return player_gui;
}

bool M_UI::PreUpdate()
{
	BROFILER_CATEGORY("M_UI_Preupdate", Profiler::Color::Brown);

	// Debug ===================================================
	if (app->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
	{
		debug = !debug;
	}

	return true;
}

bool M_UI::Update(float dt)
{
	BROFILER_CATEGORY("M_UI_Update", Profiler::Color::Brown);

	int x_mouse = 0, y_mouse = 0;
	fPoint last_mouse_position = mouse_position;
	app->input->GetMousePosition(x_mouse, y_mouse);
	mouse_position = { (float)x_mouse ,(float)y_mouse };


	UpdateElements(dt);

	if (interactive_elements.size() < 0)
	{
		return true;
	}

	UI_INPUT_TYPE new_type = UI_INPUT_TYPE::NO_TYPE;

	if ( input_type != UI_INPUT_TYPE::MOUSE && mouse_position != last_mouse_position)
	{
		new_type = UI_INPUT_TYPE::MOUSE;
	}
	else if (input_type != UI_INPUT_TYPE::CONTROLLER && app->input->controllers.size() > 0 && (*app->input->controllers.begin())->GetJoystick(Joystick::LEFT) != iPoint(0.f, 0.f))
	{
		new_type = UI_INPUT_TYPE::CONTROLLER;
	}

	if (new_type != UI_INPUT_TYPE::NO_TYPE)
	{
		input_type = new_type;
		selected_element = nullptr;
	}

	switch (input_type)
	{
	case UI_INPUT_TYPE::CONTROLLER:
		FocusController();

	if (selected_element != nullptr && (*app->input->controllers.begin())->GetButtonState(SDL_CONTROLLER_BUTTON_A) == KEY_DOWN)
		{
			if (selected_element->listener != nullptr)
			{
				selected_element->listener->OutClick(selected_element);
			}
		}

		break;
	case UI_INPUT_TYPE::MOUSE:
		FocusMouse();
	}

	return true;
}

// Called after all Updates
bool M_UI::PostUpdate(float dt)
{
	BROFILER_CATEGORY("M_UI_PostUpdate", Profiler::Color::Brown);

	fRect full_screen = app->win->GetWindowRect();

	// Draw all In Game elements ====================================

	for (list<Player_GUI*>::iterator gui = players_guis.begin(); gui != players_guis.end(); ++gui)
	{
		current_gui = (*gui);
		current_camera = current_gui->player->camera_player;
		DrawUI(main_in_game_element);
	}

	current_camera = nullptr;

	// Draw all UI elements ====================================

	DrawUI(main_ui_element);

	// Debug Positions  =======================================

	if (debug)
	{
		for (list<UI_Element*>::iterator item = elements_list.begin(); item != elements_list.end(); ++item)
		{
			if ((*item) != main_ui_element)
			{
				app->render->DrawQuad({ (int)(*item)->position.x - 3 , (int)(*item)->position.y - 3,  6, 6 }, 255, 0, 0, 255, true, false);
			}
		}
	}
	return true;
}

void M_UI::FocusMouse()
{
	fRect section;

	// Click States ============================================

	if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		SelectClickedObject();
	}
	else if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && selected_element)
	{
		click_state = FocusState::REPEAT;
	}
	else if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && selected_element)
	{
		click_state = FocusState::EXIT;
	}
	else if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_IDLE && selected_element)

	{
		click_state = FocusState::NONE;
		selected_element = nullptr;
	}

	// Click Callbacks =============================================

	if (selected_element != nullptr && selected_element->listener != nullptr)
	{
		switch (click_state)
		{
		case FocusState::ENTER:
			selected_element->listener->OnClick(selected_element);
			break;
		case FocusState::REPEAT:
			selected_element->listener->RepeatClick(selected_element);
			break;
		case FocusState::EXIT:
			if (selected_element->hover_state != HoverState::NONE)
			{
				selected_element->listener->OutClick(selected_element);
			}
			break;
		}
	}

	// Draggable ================================================
	if (selected_element && selected_element->is_draggable)
	{
		switch (click_state)
		{
		case FocusState::ENTER:
			mouse_offset = mouse_position - selected_element->position;
			break;
		case FocusState::REPEAT:
			selected_element->position = mouse_position - mouse_offset;
			selected_element->UpdateRelativePosition();
			break;
		case FocusState::EXIT:
			mouse_offset = { 0,0 };
			break;
		}
	}

	// Hover States ============================================

	for (list<UI_Element*>::iterator item = interactive_elements.begin(); item != interactive_elements.end(); ++item)
	{
		if ((*item)->state != ELEMENT_STATE::VISIBLE || (*item)->section_width == 0.f || (*item)->section_height == 0.f || (*item)->is_interactive == false)
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

	// Hover Callbacks =============================================

	for (list<UI_Element*>::iterator item = interactive_elements.begin(); item != interactive_elements.end(); ++item)
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
}

void M_UI::FocusController()
{
	Controller* controller = (*app->input->controllers.begin());
	fPoint joy_stick_dir = (fPoint)controller->GetJoystick(Joystick::LEFT);

	if (abs(joy_stick_dir.x) < UI_DEAD_ZONE || abs(joy_stick_dir.y) < UI_DEAD_ZONE)
	{
		return;
	}

	float angle = atan2(joy_stick_dir.y, joy_stick_dir.x) * RADTODEG;

	CONTROLLER_DIR dir = CONTROLLER_DIR::NO_DIR;

	if (able_axis == FOCUS_AXIS::Y)
	{

		if (angle > 0)
		{
			dir = CONTROLLER_DIR::DOWN;
		}
		else 
		{
			dir = CONTROLLER_DIR::UP;
		}
	}
	else
	{
		if (angle == 0)
		{
			dir = CONTROLLER_DIR::NO_DIR;
		}
		else if (angle <= 45.f && angle > -45)
		{
			dir = CONTROLLER_DIR::RIGHT;
		}
		else if (angle > 45 && angle <= 135)
		{
			dir = CONTROLLER_DIR::DOWN;

		}
		else if ((angle > 135 && angle <= 180) || angle >= -180 && angle <= -135)
		{
			dir = CONTROLLER_DIR::LEFT;
		}
		else if (angle > -135 && angle <= -45)
		{
			dir = CONTROLLER_DIR::UP;
		}
	}
	

	if (selected_element == nullptr && dir != CONTROLLER_DIR::NO_DIR && interactive_elements.size()>0)
	{
		selected_element = (*interactive_elements.begin());
	}
	else if (selected_element != nullptr && btw_focus_timer.Read() > BTW_FOCUS_TIME && dir != CONTROLLER_DIR::NO_DIR)
	{
		selected_element = GetNearestElement(selected_element, dir);
	}
}

void M_UI::UpdateElements(float dt)
{
	// Update FX ===================================================

	for (std::list<UI_Fade_FX*>::iterator iter = active_fxs.begin(); iter != active_fxs.end(); )
	{
		if ((*iter)->element->to_destroy == true || (*iter)->finished == true)
		{
			if ((*iter)->element->element_fx == (*iter))
			{
				(*iter)->element->element_fx = nullptr;
			}

			RELEASE(*iter);
			iter = active_fxs.erase(iter);
		}
		else
		{
			(*iter)->Update(dt);
			++iter;
		}
	}

	// In Game Elements Update ============================================

	for (list < UI_Element*> ::iterator element = ig_elements_list.begin(); element != ig_elements_list.end(); )
	{
		if ((*element)->to_destroy == true)
		{
			UI_Element* parent = (*element)->parent_element;
			std::list<UI_Element*> * sons_list = (*element)->GetSons();

			// Merge its sons to its parent

			for (list < UI_Element*> ::iterator son = sons_list->begin(); son != sons_list->end(); ++son)
			{
				(*son)->parent_element = parent;
				parent->element_sons.push_back((*son));
			}

			// Delete it self from its parent

			parent->GetSons()->erase(std::find(parent->GetSons()->begin(), parent->GetSons()->end(), (*element)));

			// Delete from UI list

			RELEASE((*element));
			element = ig_elements_list.erase(element);
		}
		else
		{
			(*element)->Update(dt);
			++element;
		}

		UpdateGuiPositions(main_in_game_element, fPoint(0, 0));

	}

	// UI Elements Update =====================================================

	for (list<UI_Element*>::iterator element = elements_list.begin(); element != elements_list.end();)
	{
		if ((*element)->to_destroy == true)
		{
			UI_Element* parent = (*element)->parent_element;
			std::list<UI_Element*> * sons_list = (*element)->GetSons();

			// Merge its sons to its parent

			for (list < UI_Element*> ::iterator son = sons_list->begin(); son != sons_list->end(); ++son)
			{
				(*son)->parent_element = parent;
				parent->element_sons.push_back((*son));
			}

			// Delete it self from its parent

			parent->GetSons()->erase(std::find(parent->GetSons()->begin(), parent->GetSons()->end(), (*element)));

			// Check if it is teh current selected element

			if (selected_element == (*element))
			{
				selected_element = nullptr;
			}

			// Delete from UI list
			if ((*element)->is_interactive == true)
			{
				std::list<UI_Element*>::iterator found_element = std::find(interactive_elements.begin(), interactive_elements.end(), (*element));

				if (found_element != interactive_elements.end())
				{
					interactive_elements.erase(found_element);
				}
			}

			RELEASE((*element));
			element = elements_list.erase(element);
		}
		else
		{
			(*element)->Update(dt);
			++element;
		}
	}

	UpdateGuiPositions(main_ui_element, fPoint(0, 0));

}

 SDL_Texture* M_UI::GetAtlas() const 
{
	return atlas;
}

 FocusState M_UI::GetClickState() const
 {
	 return click_state;
 }

UI_Element * M_UI::GetSelectedElement()
{
	return selected_element;
}

UI_Element * M_UI::GetScreen()
{
	return main_ui_element;
}

UI_INPUT_TYPE M_UI::GetInputType() const
{
	return input_type;
}

void M_UI::SetStateToBranch(const ELEMENT_STATE state, UI_Element * branch_root)
{
	if (branch_root == nullptr)
	{
		return;
	}

	branch_root->state = state;

	for (list<UI_Element*>::iterator item = branch_root->element_sons.begin(); item != branch_root->element_sons.end(); ++item)
	{
		SetStateToBranch(state, (*item));
	}

}

bool M_UI::SelectClickedObject()
{
	list<UI_Element*> clicked_objects;

	for (list<UI_Element*>::iterator item = interactive_elements.begin(); item != interactive_elements.end(); ++item)
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
			for (UI_Element* iterator = (*item); iterator != nullptr ; iterator = iterator->parent_element)
			{
				++count;
			}

			if (count > nearest_object_position)
			{
				nearest_object_position = count;
				nearest_object = (*item);
			}
		}
		selected_element = nearest_object;
		click_state = FocusState::ENTER;
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
		if (object->alpha == 0.f)
		{
			
		}
	    else if (object->single_camera == nullptr && object->not_in_camera == nullptr)
		{
			object->Draw();
		}
		else if (object->not_in_camera == current_camera)
		{

		}
		else if ( object->single_camera == current_camera)
		{
			object->Draw();
		}
	}
	
	if (debug && object->state != ELEMENT_STATE::HIDDEN && object->is_interactive == true)
	{
		SDL_Rect rect = (SDL_Rect)object->GetSection();

		if (selected_element == object)
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

	for (list<UI_Element*>::iterator item = object->element_sons.begin();  item != object->element_sons.end(); ++item)
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

	for (list<UI_Element*>::iterator item = object->element_sons.begin() ; item != object->element_sons.end(); ++item)
	{
		UpdateGuiPositions((*item), cumulated_position);
	}
}

void M_UI::AddInteractiveElement(UI_Element* element)
{
	if (element != nullptr && element->is_in_game == false)
	{
		element->is_interactive = true;
		interactive_elements.push_back(element);
	}
}

void M_UI::AddFX(UI_Fade_FX::FX_TYPE type, const float seconds,  UI_Element * element, const float loops , const float init_value, const float target_value)
{
	UI_Fade_FX* new_fx = DBG_NEW  UI_Fade_FX(type, seconds, element, loops,  init_value, target_value);

	if (element->element_fx != nullptr)
	{
		element->element_fx->Destroy();
	}

	element->element_fx = new_fx;

	active_fxs.push_back(new_fx);
}

// Creation methods =================================================================

UI_Element * M_UI::CreateElement(const fPoint position, const UI_ElementDef definition, UI_Listener * listener)
{
	UI_Element* object = DBG_NEW UI_Element(position, definition, listener);

	if (definition.is_in_game == true)
	{
		object->SetParent(main_in_game_element);
		ig_elements_list.push_back(object);
	}
	else
	{
		object->SetParent(main_ui_element);
		elements_list.push_back(object);
	}
	return object;
}

UI_Quad * M_UI::CreateQuad(const fPoint position, const UI_QuadDef definition, UI_Listener * listener)
{
	UI_Quad* object = DBG_NEW UI_Quad(position, definition, listener);

	if (definition.is_in_game == true)
	{
		object->SetParent(main_in_game_element);
		ig_elements_list.push_back(object);
	}
	else
	{
		object->SetParent(main_ui_element);
		elements_list.push_back(object);
	}
	return object;
}

UI_Label* M_UI::CreateLabel(const fPoint position, UI_LabelDef definition, UI_Listener* listener)
{

	UI_Label* object = DBG_NEW UI_Label(position, definition, listener);

	if (definition.is_in_game == true)
	{
		object->SetParent(main_in_game_element);
		ig_elements_list.push_back(object);
	}
	else
	{
		object->SetParent(main_ui_element);
		elements_list.push_back(object);
	}

	return object;

}

UI_Image* M_UI::CreateImage(const fPoint position, const UI_ImageDef definition, UI_Listener* listener)
{
	UI_Image* object = DBG_NEW UI_Image(position, definition, listener);

	if (definition.is_in_game == true)
	{
		object->SetParent(main_in_game_element);
		ig_elements_list.push_back(object);
	}
	else
	{
		object->SetParent(main_ui_element);
		elements_list.push_back(object);
	}

	return object;
}

UI_Button* M_UI::CreateButton(const fPoint position, const UI_ButtonDef definition, UI_Listener* listener)
{
	UI_Button* object = DBG_NEW UI_Button(position, definition, listener);

	if (definition.is_in_game == true)
	{
		object->SetParent(main_in_game_element);
		ig_elements_list.push_back(object);
	}
	else
	{
		object->SetParent(main_ui_element);
		elements_list.push_back(object);
	}

	return object;
}

UI_Slider * M_UI::CreateSlider(const fPoint position, const UI_SliderDef definition, UI_Listener * listener)
{
	UI_Slider* object = DBG_NEW UI_Slider(position, definition, listener);

	if (definition.is_in_game == true)
	{
		object->SetParent(main_in_game_element);
		ig_elements_list.push_back(object);
	}
	else
	{
		object->SetParent(main_ui_element);
		elements_list.push_back(object);
	}

	return object;
}

UI_Checkbox * M_UI::CreateCheckbox(const fPoint position, const UI_CheckboxDef definition, UI_Listener * listener)
{
	UI_Checkbox* object = DBG_NEW UI_Checkbox(position, definition, listener);

	if (definition.is_in_game == true)
	{
		object->SetParent(main_in_game_element);
		ig_elements_list.push_back(object);
	}
	else
	{
		object->SetParent(main_ui_element);
		elements_list.push_back(object);
	}

	return object;
}

UI_TextPanel * M_UI::CreateTextPanel(const fPoint position, const UI_TextPanelDef definition, UI_Listener * listener)
{
	UI_TextPanel* object = DBG_NEW UI_TextPanel(position, definition, listener);

	if (definition.is_in_game == true)
	{
		object->SetParent(main_in_game_element);
		ig_elements_list.push_back(object);
	}
	else
	{
		object->SetParent(main_ui_element);
		elements_list.push_back(object);
	}

	return object;
}
UI_Bar * M_UI::CreateBar(const fPoint position, const UI_BarDef definition, UI_Listener * listener)
{
	UI_Bar* object = DBG_NEW UI_Bar(position, definition, listener);

	if (definition.is_in_game == true)
	{
		object->SetParent(main_in_game_element);
		ig_elements_list.push_back(object);
	}
	else
	{
		object->SetParent(main_ui_element);
		elements_list.push_back(object);
	}

	return object;
}


UI_InGameElement*  M_UI::CreateInGameElement(const fPoint position, const UI_InGameElementDef definition)
{
	UI_InGameElement* object = DBG_NEW UI_InGameElement(position, definition);
	object->SetParent(main_in_game_element);
	ig_elements_list.push_back(object);
	return object;
}

UI_IG_Weapon * M_UI::CreateInGameWeapon(const fPoint position, const UI_InGameElementDef definition)
{
	UI_IG_Weapon* object = DBG_NEW UI_IG_Weapon(position, definition);
	object->SetParent(main_in_game_element);
	ig_elements_list.push_back(object);
	return object;
}

UI_IG_Item * M_UI::CreateInGameItem(const fPoint position, const UI_InGameElementDef definition)
{
	UI_IG_Item* object = DBG_NEW UI_IG_Item(position, definition);
	object->SetParent(main_in_game_element);
	ig_elements_list.push_back(object);
	return object;
}

UI_IG_Helper * M_UI::CreateInGameHelper(const fPoint position, const UI_InGameElementDef definition)
{
	UI_IG_Helper* object = DBG_NEW UI_IG_Helper(position, definition);
	object->SetParent(main_in_game_element);
	ig_elements_list.push_back(object);
	return object;
}
// ====================================================================================


UI_Fade_FX::UI_Fade_FX( const FX_TYPE type, const float seconds, UI_Element * element, float loops,  const float init_value, const float target_value): element(element), max_loops(loops), type(type)
{
	ratetime = 1.f / seconds;

	if (init_value == -1.f || target_value == -1.f)
	{
		switch (type)
		{
		case FX_TYPE::FADE_ON:
			this->init_value = 0.f;
			this->target_value = 255.f;
			break;
		case FX_TYPE::FADE_OUT:
			this->init_value = 255.f;
			this->target_value = 0.f;
			break;
		case FX_TYPE::INTERMITTENT:
			this->init_value = 0.f;
			this->target_value = 255.f;
			break;
		}

		element->alpha = this->init_value;
	}
	else
	{
		this->init_value = init_value;
		this->target_value = target_value;
	}

}

bool UI_Fade_FX::Update(float dt)
{
	if (type == UI_Fade_FX::FX_TYPE::FADE_ON || type == UI_Fade_FX::FX_TYPE::FADE_OUT)
	{
		ax += dt * ratetime;
		element->alpha = lerp(init_value, target_value, ax);

		if (element->alpha == target_value)
		{
			finished = true;
		}
	}

	else if (type == UI_Fade_FX::FX_TYPE::INTERMITTENT)
	{
		ax += dt * ratetime;
		element->alpha = lerp(init_value, target_value, ax);

		if (element->alpha == target_value)
		{
			swap(init_value, target_value);
			loops_count += 0.5f;
			ax = 0.f;
		}

		if ( max_loops != -1.f && loops_count >= max_loops)
		{
			finished = true;
		}
	}

	return true;
}

void UI_Fade_FX::Destroy()
{
	finished = true;
}

UI_Element* M_UI::GetNearestElement(UI_Element* element, CONTROLLER_DIR dir)
{
	UI_Element* nearest_element = nullptr;
	float nearest_dis = 0;

	for (std::list<UI_Element*>::iterator iter = interactive_elements.begin(); iter != interactive_elements.end(); ++iter)
	{
		if (element == (*iter))
		{
			continue;
		}

		switch (dir)
		{
		case CONTROLLER_DIR::UP:

			if ((*iter)->position.y < element->position.y)
			{
				if (nearest_dis == 0)
				{
					nearest_element = (*iter);
					nearest_dis = element->position.y - (*iter)->position.y;
				}
				else if (nearest_dis > element->position.y - (*iter)->position.y)
				{
					nearest_element = (*iter);
					nearest_dis = element->position.y - (*iter)->position.y;
				}
			}
			break;

		case CONTROLLER_DIR::DOWN:

			if ((*iter)->position.y > element->position.y)
			{
				if (nearest_dis == 0)
				{
					nearest_element = (*iter);
					nearest_dis = (*iter)->position.y - element->position.y;
				}
				else if (nearest_dis > (*iter)->position.y - element->position.y)
				{
					nearest_element = (*iter);
					nearest_dis = (*iter)->position.y - element->position.y;
				}
			}
			break;

		case CONTROLLER_DIR::RIGHT:

			if ((*iter)->position.x > element->position.x)
			{
				if (nearest_dis == 0)
				{
					nearest_element = (*iter);
					nearest_dis = element->position.x - (*iter)->position.x;
				}
				else if (nearest_dis > element->position.x - (*iter)->position.x)
				{
					nearest_element = (*iter);
					nearest_dis = element->position.x - (*iter)->position.x;
				}
			}
			break;

		case CONTROLLER_DIR::LEFT:

			if ((*iter)->position.x < element->position.x)
			{
				if (nearest_dis == 0)
				{
					nearest_element = (*iter);
					nearest_dis = element->position.x - (*iter)->position.x;
				}
				else if (nearest_dis > element->position.x - (*iter)->position.x)
				{
					nearest_element = (*iter);
					nearest_dis = element->position.x - (*iter)->position.x;
				}
			}
			break;
		}
	}

	if (nearest_element == nullptr)
	{
		return element;
	}
	else
	{
		btw_focus_timer.Start();
		return nearest_element;
	}
}
