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
#include "Obj_Tank.h"

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

	arrow_anim.frames = app->anim_bank->LoadFrames(config.child("animations").child("arrow"));

	button_sprites[(int)GAMEPAD_BUTTON::A] = { 440,10 ,50 ,50 };
	button_sprites[(int)GAMEPAD_BUTTON::B] = { 390,60 ,50 ,50 };
	button_sprites[(int)GAMEPAD_BUTTON::Y] = { 440,60 ,50 ,50 };
	button_sprites[(int)GAMEPAD_BUTTON::X] = { 390,10 ,50 ,50 };
	button_sprites[(int)GAMEPAD_BUTTON::LT] = { 280,10 ,50 ,50 };
	button_sprites[(int)GAMEPAD_BUTTON::LB] = { 280,60 ,50 ,50 };
	button_sprites[(int)GAMEPAD_BUTTON::RT] = { 330,10 ,50 ,50 };
	button_sprites[(int)GAMEPAD_BUTTON::RB] = { 330,60 ,50 ,50 };

	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_DOUBLE_MISSILE] = { 500,500 ,34 ,34 };
	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_FLAMETHROWER] = { 540,500 ,34 ,34 };
	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_HEALING_SHOT] = { 390,500 ,34 ,34 };
	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_LASER] = { 390,500 ,34 ,34 };

	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_DOUBLE_MISSILE] = { 500,595,44 ,44 };
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_FLAMETHROWER] = { 550,595,44 ,44 };
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_HEALING_SHOT] = { 390,595,44 ,44 };
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_LASER] = { 390,595,44 ,44 };

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

	atlas = app->tex->Load("textures/ui/atlas.png");
	font_open_sants_bold_12 = app->font->Load("fonts/open_sans/OpenSans-Bold.ttf");

	UI_ImageDef image_def;
	fRect full_screen = app->win->GetWindowRect();

	// HUD ===========================================
	player_1_gui = DBG_NEW Player_GUI(Player_GUI::TYPE::PLAYER_1, app->scene->tank_1);
	app->scene->tank_1->SetGui(player_1_gui);
	players_guis.push_back(player_1_gui);

	player_2_gui = DBG_NEW Player_GUI(Player_GUI::TYPE::PLAYER_2, app->scene->tank_2);
	app->scene->tank_2->SetGui(player_2_gui);
	players_guis.push_back(player_2_gui);

	player_3_gui = DBG_NEW Player_GUI(Player_GUI::TYPE::PLAYER_3, app->scene->tank_3);
	app->scene->tank_3->SetGui(player_3_gui);
	players_guis.push_back(player_3_gui);

	player_4_gui = DBG_NEW Player_GUI(Player_GUI::TYPE::PLAYER_4, app->scene->tank_4);
	app->scene->tank_4->SetGui(player_4_gui);
	players_guis.push_back(player_4_gui);

	// General 4 HUD players =========================================================
	image_def.sprite_section = { 170 , 10, 105, 105 };
	round_element = CreateImage({ full_screen.w * .5f ,  full_screen.h * .5f }, image_def);
	round_element->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);

	image_def.sprite_section = { 120, 515, 179, 179 };
	round_fx = CreateImage({ full_screen.w * .5f ,  full_screen.h * .5f }, image_def);
	round_fx->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	round_fx->alpha = 0;

	image_def.sprite_section = { 10, 160, 50, 530 };
	left_tank_life = CreateImage({ 0.f ,  full_screen.h * .5f }, image_def);
	left_tank_life->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::CENTER);

	image_def.sprite_section = { 60, 160, 50, 530 };
	right_tank_life = CreateImage({ full_screen.w ,  full_screen.h * .5f }, image_def);
	right_tank_life->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::CENTER);
	
	return true;
}

// Called before quitting
bool M_UI::CleanUp()
{
	LOG("Freeing all UI objects");

	app->tex->UnLoad(atlas);
	app->font->Unload(font_open_sants_bold_12);

	font_open_sants_bold_12 = nullptr;
	atlas = nullptr;

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

	return true;
}


// Update all guis
bool M_UI::PreUpdate()
{
	BROFILER_CATEGORY("M_UI_Preupdate", Profiler::Color::Brown);

	int x_mouse = 0, y_mouse = 0;
	app->input->GetMousePosition(x_mouse, y_mouse);
	mouse_position = { (float)x_mouse ,(float)y_mouse };
	
	// Debug ===================================================
	if (app->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
		debug = !debug;

	// Hover States ============================================

	fRect section;

	for (list<UI_Element*>::iterator item = elements_list.begin(); item != elements_list.end(); ++item)
	{
		if ((*item)->state != ELEMENT_STATE::VISIBLE || (*item)->section_width == 0.f || (*item)->section_height == 0.f || (*item)->to_destroy == true)
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
	else if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && selected_element)
	{
		click_state = ClickState::REPEAT;
	}
	else if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && selected_element)
	{
		click_state = ClickState::EXIT;
	}
	else if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_IDLE && selected_element)

	{
		click_state = ClickState::NONE;
		selected_element = nullptr;
	}


	return true;
}

bool M_UI::Update(float dt)
{
	BROFILER_CATEGORY("M_UI_Update", Profiler::Color::Brown);


	//ax += dt * ratetime;
	//
	//if (round_fx->alpha == target_value)
	//{
	//	swap(init_value, target_value);
	//	ax = 0.f;
	//}

	//round_fx->alpha = lerp(init_value, target_value, ax);

	//ax += dt * ratetime;

	////if (app->scene->tank_1->GetLife() == target_value)
	////{
	////	swap(init_value, target_value);
	////	ax = 0.f;
	////}

	//app->scene->tank_1->SetLife( lerp(init_value, target_value, ax));

	for (list<Player_GUI*>::iterator gui = players_guis.begin(); gui != players_guis.end(); ++gui)
	{
		(*gui)->Update(dt);
	}

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
			++element;
		}
	}

	// Draggable ================================================
	if (selected_element && selected_element->is_draggable)
	{
		switch (click_state)
		{
		case ClickState::ENTER:
			mouse_offset = mouse_position - selected_element->position;
			break;
		case ClickState::REPEAT:
			selected_element->position  = mouse_position - mouse_offset;
			selected_element->UpdateRelativePosition();
			break;
		case ClickState::EXIT:
			mouse_offset = { 0,0 };
			break;
		}
	}
	// Click Callbacks =============================================

	if (selected_element && selected_element->listener)
	{
		switch (click_state)
		{
		case ClickState::ENTER:
			selected_element->listener->OnClick(selected_element);
			break;
		case ClickState::REPEAT:
			selected_element->listener->RepeatClick(selected_element);
			break;
		case ClickState::EXIT:
			if (selected_element->hover_state != HoverState::NONE)
			{
				selected_element->listener->OutClick(selected_element);
			}
			break;
		}
	}

	for (list<UI_Element*>::iterator item = elements_list.begin(); item != elements_list.end();)
	{
		if ((*item)->to_destroy == true)
		{
			// Delete ui element on parent sons list =========================

			if ((*item)->parent_element != nullptr)
			{
				list<UI_Element*> *sons_list = (*item)->parent_element->GetSons();
				list<UI_Element*>::iterator son_to_delete = find(sons_list->begin(), sons_list->end(), (*item));

				if (son_to_delete == sons_list->end())
				{
					LOG("Object not deleted: Not found");
					return false;
				}

				sons_list->erase(son_to_delete);
			}

			// Delete ui element ============================================

			LOG("UI Object deleted");
			RELEASE((*item));
			item = elements_list.erase(item);
		}
		else
		{
			(*item)->Update(dt);
			++item;
		}
	}


	// Hover Callbacks =============================================

	for (list<UI_Element*>::iterator item = elements_list.begin(); item != elements_list.end(); ++item)
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

	UpdateGuiPositions(main_ui_element, fPoint(0, 0));

	return true;
}

// Called after all Updates
bool M_UI::PostUpdate(float dt)
{
	BROFILER_CATEGORY("M_UI_PostUpdate", Profiler::Color::Brown);

	fRect full_screen = app->win->GetWindowRect();

	//Split Lines ==========================================================================================================

	app->render->DrawQuad({(int) (full_screen.w * .5f) - 3,  0, 6, (int)full_screen.h }, 150, 150, 150, 255, true, false);
	app->render->DrawQuad({ 0 ,(int)(full_screen.h * .5f) - 3, (int)full_screen.w, 6 }, 150, 150, 150, 255, true, false);

	// Draw all In Game elements ====================================

	for (list<Player_GUI*>::iterator gui = players_guis.begin(); gui != players_guis.end(); ++gui)
	{
		current_gui = (*gui);
		current_camera = current_gui->player->camera_player;

		for (list<UI_Element*>::iterator element = ig_elements_list.begin(); element != ig_elements_list.end(); ++element)
		{
			(*element)->PostUpdate();
		}

		UpdateGuiPositions(main_in_game_element, fPoint(0, 0));

		DrawUI(main_in_game_element);

		/*app->render->DrawQuad(current_gui->viewport_with_margin, 255, 255, 255, 255, false, false);*/
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

 UI_Label* M_UI::CreateLabel(const fPoint position,  UI_LabelDef definition, UI_Listener* listener)
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

UI_Image* M_UI::CreateImage(const fPoint position, const UI_ImageDef definition , UI_Listener* listener)
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

UI_Element * M_UI::GetClickedObject()
{
	return selected_element;
}

UI_Element * M_UI::GetScreen()
{
	return main_ui_element;
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

	for (list<UI_Element*>::iterator item = elements_list.begin(); item != elements_list.end(); ++item)
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
