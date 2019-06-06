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
#include "M_Debug.h"

#include "Player_GUI.h"

// UI includes --------------------------
#include "UI_Element.h"
#include "UI_Image.h"
#include "UI_Label.h"
#include "UI_Button.h"
#include "UI_Slider.h"
#include "UI_Checkbox.h"
#include "UI_Table.h"
#include "UI_TextPanel.h"
#include "UI_InteractiveGroup.h"
#include "UI_InGameElement.h"
#include "UI_Bar.h"
#include "UI_Quad.h"
#include "UI_InputText.h"

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

	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_DOUBLE_MISSILE] =	{ 500, 500 ,34 ,34 };
	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_FLAMETHROWER] =		{ 540, 500 ,34 ,34 };
	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_HEALING_SHOT] =		{ 620, 500 ,34 ,34 };
	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_LASER] =				{ 580, 500 ,34 ,34 };
	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_OIL_SHOT] =			{ 700, 500 ,34 ,34 };
	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_ELECTRO_SHOT] =		{ 740, 500 ,34 ,34 };
																						   
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_DOUBLE_MISSILE] =		{ 500, 595,44 ,44 };
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_FLAMETHROWER] =		{ 550, 595,44 ,44 };
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_LASER] =				{ 600, 595,44 ,44 };
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_HEALING_SHOT] =		{ 650, 595,44 ,44 };
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_BASIC] =				{ 700, 595,44 ,44 };
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_OIL_SHOT] =			{ 750, 595,44 ,44 };
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_ELECTRO_SHOT] =		{ 800, 595,44 ,44 };

	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::ITEM_HEALTH_BAG] = { 500,545 ,40 ,40 };
	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::ITEM_HAPPY_HOUR] = { 590,545 ,40 ,40 };
	icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::ITEM_INSTANT_HELP] = { 635,545 ,40 ,40 };

	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::ITEM_HEALTH_BAG] = { 500,650 ,47 ,47 };
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::ITEM_HAPPY_HOUR] = { 600,650 ,47 ,47 };
	icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::ITEM_INSTANT_HELP] = { 650,650 ,47 ,47 };

	return ret;
}

// Called before the first frame
bool M_UI::Start()
{
	// Assets ========================================

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
	focused_element = nullptr;

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
	focused_element = nullptr;
	reset = true;

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

	reset = false;

	return true;
}

bool M_UI::PreUpdate()
{
	BROFILER_CATEGORY("M_UI_Preupdate", Profiler::Color::Brown);

	int x = 0, y = 0;
	app->input->GetMousePosition(x,y);
	mouse_position = fPoint( x,y );

	return true;
}

bool M_UI::Update(float dt)
{
	BROFILER_CATEGORY("M_UI_Update", Profiler::Color::Brown);

	// Select input type ====================================================

	SelectInputType();

	// Update navigation and selection ======================================

	if (current_interactive_group != nullptr)
	{
		if (input_type == UI_INPUT_TYPE::MOUSE)
		{
			MouseSelection();
			MouseNavigation();
		}
		else if (input_type == UI_INPUT_TYPE::CONTROLLERS)
		{
			ControllerSelection();
			ControllersNavigation();
		}
	}

	// Update elements and in game elements =================================

	UpdateElements(dt);

	// Update player guis ====================================================

	for (std::list<Player_GUI*>::iterator iter = players_guis.begin(); iter != players_guis.end(); ++iter)
	{
		(*iter)->Update(dt);
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

	if (app->debug->debug_ui)
	{
		for (list<UI_Element*>::iterator item = elements_list.begin(); item != elements_list.end(); ++item)
		{
			if ((*item) != main_ui_element)
			{
				app->render->DrawQuad((*item)->GetDrawRect(), 255, 0, 0, 255, true, false);
			}
		}
	}
	return true;
}

void M_UI::MouseNavigation()
{
	fRect section;

	bool mouse_is_focusing = false;

	for (list<UI_Element*>::iterator iter = current_interactive_group->group_elements_list.begin(); iter != current_interactive_group->group_elements_list.end(); ++iter)
	{
		if ((*iter)->state != ELEMENT_STATE::VISIBLE)
		{
			continue;
		}

		section = (*iter)->GetSection();

		if (mouse_position.x >= section.GetLeft() && mouse_position.x <= section.GetRight() && mouse_position.y >= section.GetTop() && mouse_position.y <= section.GetBottom() && (*iter)->listener != nullptr)
		{
			mouse_is_focusing = true;

			if (focused_element != (*iter))
			{
				if (focused_element != nullptr )
				{
					focused_element->listener->UI_OnHoverExit((*iter));
				}

				focused_element = (*iter);

				if (focused_element != nullptr )
				{
					focused_element->listener->UI_OnHoverEnter((*iter));
				}
			}
			else
			{
				if (focused_element != nullptr )
				{
					focused_element->listener->UI_OnHoverRepeat((*iter));
				}
			}
			break;
		}
	}

	if (mouse_is_focusing = false && focused_element != nullptr)
	{
		focused_element->listener->UI_OnHoverExit(focused_element);
		focused_element = nullptr;
	}
}

void M_UI::ControllerSelection()
{
	if (focused_element != nullptr)
	{
		if (app->input->GetMouseButton(1) == KEY_UP)
		{
			focused_element->listener->UI_Selected(focused_element);
		}
	}
}

void M_UI::MouseSelection()
{

}

void M_UI::ControllersNavigation()
{

}

void M_UI::SelectInputType()
{

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
			UI_Element* parent = (*element)->element_parent;
			std::list<UI_Element*> * sons_list = (*element)->GetSons();

			// Merge its sons to its parent

			for (list < UI_Element*> ::iterator son = sons_list->begin(); son != sons_list->end(); ++son)
			{
				(*son)->element_parent = parent;
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

		UpdateHerarchyPositions(main_in_game_element, fPoint(0, 0));

	}

	// UI Elements Update =====================================================

	for (list<UI_Element*>::iterator element = elements_list.begin(); element != elements_list.end();)
	{
		if ((*element)->to_destroy == true)
		{
			UI_Element* parent = (*element)->element_parent;
			std::list<UI_Element*> * sons_list = (*element)->GetSons();

			// Merge its sons to its parent

			for (list < UI_Element*> ::iterator son = sons_list->begin(); son != sons_list->end(); ++son)
			{
				(*son)->element_parent = parent;
				parent->element_sons.push_back((*son));
			}

			// Delete it self from its parent

			parent->GetSons()->erase(std::find(parent->GetSons()->begin(), parent->GetSons()->end(), (*element)));

			// Check if it is teh current selected element

			if (focused_element == (*element))
			{
				focused_element = nullptr;
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

	UpdateHerarchyPositions(main_ui_element, fPoint(0, 0));

}

Player_GUI * M_UI::AddPlayerGUI(Obj_Tank * player)
{
	Player_GUI* player_gui = DBG_NEW Player_GUI(player);
	player->SetGui(player_gui);
	players_guis.push_back(player_gui);
	return player_gui;
}


 SDL_Texture* M_UI::GetAtlas() const 
{
	return atlas;
}

 FocusState M_UI::GetClickState() const
 {
	 return focus_state;
 }

 void M_UI::SetFocusedElement(UI_Element * element)
 {
	 focused_element = element;
 }

 UI_Element * M_UI::GetFocusedElement()
{
	return focused_element;
}

UI_INPUT_TYPE M_UI::GetInputType()
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
	
	if (app->debug->debug_ui && object->state != ELEMENT_STATE::HIDDEN)
	{
		SDL_Rect rect = (SDL_Rect)object->GetSection();

		if (focused_element == object)
		{
			app->render->DrawQuad(rect, 255, 233, 15, 100, true, false);
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

void M_UI::UpdateHerarchyPositions(UI_Element * object, fPoint cumulated_position)
{
	if (object == nullptr)
	{
		return;
	}

	cumulated_position += object->relative_position;
	object->position = cumulated_position;

	for (list<UI_Element*>::iterator item = object->element_sons.begin() ; item != object->element_sons.end(); ++item)
	{
		UpdateHerarchyPositions((*item), cumulated_position);
	}
}

void M_UI::HideAllUI()
{
	app->ui->SetStateToBranch(ELEMENT_STATE::HIDDEN, main_ui_element);
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

	if (definition.is_in_game)
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

	if (definition.is_in_game)
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

	if (definition.is_in_game)
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

	if (definition.is_in_game)
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

	if (definition.is_in_game)
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

	if (definition.is_in_game)
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

	if (definition.is_in_game)
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

	if (definition.is_in_game)
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

	if (definition.is_in_game)
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

UI_InteractiveGroup * M_UI::CreateIntearctiveGroup(const fPoint position, const UI_InteractiveGroupDef definition, UI_Listener * listener)
{
	UI_InteractiveGroup* object = DBG_NEW UI_InteractiveGroup(position, definition, listener);
	object->SetParent(main_ui_element);
	elements_list.push_back(object);
	return object;
}

UI_InputText * M_UI::CreateInputText(const fPoint position, const UI_InputTextDef definition, UI_Listener * listener)
{
	UI_InputText* object = DBG_NEW UI_InputText(position, definition, listener);
	object->SetParent(main_ui_element);
	elements_list.push_back(object);
	return object;
}

UI_Table * M_UI::CreateTable(const fPoint position, const UI_TableDef definition,  int * widths, int * heights,  UI_Listener * listener)
{
	UI_Table* object = DBG_NEW UI_Table(position, definition, widths, heights, listener);
	object->SetParent(main_ui_element);
	elements_list.push_back(object);
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


