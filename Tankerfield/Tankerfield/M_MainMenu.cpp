#include "App.h"
#include "Log.h"
#include "M_MainMenu.h"
#include "M_Render.h"
#include "M_Textures.h"
#include "M_Fonts.h"
#include "M_SceneManager.h"
#include "M_Window.h"
#include "M_Audio.h"
#include "M_Scene.h"

#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_Label.h"
#include "UI_InteractiveGroup.h"

#include "Obj_Tank_MainMenu.h"

bool M_MainMenu::Start()
{
	// Load assets ===========================================

	background_texture = app->tex->Load("textures/ui/main_menu_background.png");
	app->audio->PlayMusic("audio/Music/menu_music.ogg");

	// Create UI Elements ====================================

	fRect screen = app->win->GetWindowRect();
	fPoint screen_center = { screen.w * 0.5f, screen.h * 0.5f };
	camera = app->render->CreateCamera( iPoint(0,0), (SDL_Rect)screen);

	// Controll helper ------------------------

	control_helper_image = app->ui->CreateImage(screen_center + fPoint(-40.f, 400.f), UI_ImageDef(app->ui->button_sprites[(int)CONTROLLER_BUTTON::A]));
	control_helper_image->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	control_helper_label = app->ui->CreateLabel(screen_center + fPoint(10.f, 400.f), UI_LabelDef("Acept", app->font->label_font_24));
	control_helper_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	// Main menu ------------------------------

	logo_image = app->ui->CreateImage(screen_center + fPoint( - 350.f, -200.f), UI_ImageDef({10, 710, 915, 260}));
	logo_image->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	multi_player_button = app->ui->CreateButton(screen_center + fPoint(-350.f, 40), UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	multi_player_button->SetLabel({ 0.f,2.f }, UI_LabelDef("Play", app->font->button_font, { 50, 50, 50, 255 }));

	exit_button = app->ui->CreateButton(screen_center + fPoint(-350.f, 150.f), UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	exit_button->SetLabel({ 0.f,2.f }, UI_LabelDef("Exit", app->font->button_font, { 50, 50, 50, 255 }));

	version_label = app->ui->CreateLabel({ screen.GetRight() - 40.f, screen.GetBottom() - 40.f }, UI_LabelDef("v .0.5.7", app->font->label_font_38, {255,255,255,180}));
	version_label->SetPivot(Pivot::X::RIGHT, Pivot::Y::BOTTOM);

	UI_InteractiveGroupDef menu_panel_def;
	menu_panel_def.columns = 1;
	menu_panel_def.rows = 2;

	menu_panel = app->ui->CreateIntearctiveGroup(screen_center, menu_panel_def, this);
	menu_panel->SetElement(multi_player_button, iPoint(0,0));
	menu_panel->SetElement(exit_button, iPoint(0, 1));

	// Selection screen ------------------------

	float element_side = 126;
	UI_InteractiveGroupDef selection_panel_def;
	selection_panel_def.columns = DEFAULT_PANEL_COLUMNS;
	selection_panel_def.rows = DEFAULT_PANEL_ROWS;
	selection_panel_def.focus_indicator = app->ui->CreateImage({ 0.F,0.F }, UI_ImageDef({ 255, 265, 126,126 }));;

	float offset = DEFAULT_PANEL_COLUMNS * element_side * 0.5f;

	selection_panel = app->ui->CreateIntearctiveGroup(screen_center, selection_panel_def, this);

	float color_value = 0.f;
	float color_sum = 1.f / (DEFAULT_PANEL_COLUMNS * DEFAULT_PANEL_ROWS + 1);

	for (int y = 0; y < DEFAULT_PANEL_ROWS; ++y)
	{
		for (int x = 0; x < DEFAULT_PANEL_COLUMNS; ++x)
		{
			UI_Element* element = app->ui->CreateImage(fPoint(screen_center.x - offset + x * element_side, screen_center.y - offset + y * element_side), UI_ImageDef({ 120 ,265 ,126 ,126 }));
			colors[x][y] = GetColor(color_value);
			element->color_mod = colors[x][y];
			color_value += color_sum;
			selection_panel->SetElement(element, iPoint(x, y));
		}
	}

	selection_panel->SetFocusImage( iPoint(0, 0) );

	fPoint tank_offset = { 280.f, 280.f };

	for (int i = 0; i < 4; ++i)
	{
		switch (i)
		{
		case 0:
			players[i].tank_pos = { screen.GetLeft() + tank_offset.x, screen.GetTop() + tank_offset.y };
			break;
		case 1:
			players[i].tank_pos = { screen.GetRight() - tank_offset.x, screen.GetTop() + tank_offset.y };
			break;
		case 2:
			players[i].tank_pos = { screen.GetLeft() + tank_offset.x, screen.GetBottom() - tank_offset.y };
			break;
		case 3:
			players[i].tank_pos = { screen.GetRight() - tank_offset.x, screen.GetBottom() - tank_offset.y };
			break;
		}

		players[i].tank = (Obj_Tank_MainMenu*)app->objectmanager->CreateObject(ObjectType::TANK_MAIN_MENU, players[i].tank_pos);
		players[i].tank->is_isometric = false;
	}

	
	// Set values ==========================================

	SetState(MENU_STATE::INIT_MENU);
	SDL_ShowCursor(SDL_ENABLE);
	
	return true;
}

bool M_MainMenu::CleanUp()
{
	app->tex->UnLoad(background_texture);
	app->audio->PauseMusic(2);
	app->render->DestroyCamera(camera);

	for (int i = 0; i < 4; ++i)
	{
		players[i].tank = nullptr;
	}

	return true;
}

bool M_MainMenu::PreUpdate()
{
	if ( app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || exit_game == true)
	{
		return false;
	}

	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		if (players[i].controller == nullptr)
		{
			players[i].controller = app->input->GetAbleController();
			players[i].tank->SetController(players[i].controller);
		}
	}

	InputNavigate();
	InputSelect();

	return true;
}

bool M_MainMenu::Update(float dt)
{
	if (menu_state == MENU_STATE::SELECTION)
	{
		players[current_player].tank->SetColor(selection_panel->GetFocusedElement()->color_mod);
	}

	return true;
}

bool M_MainMenu::PostUpdate(float dt)
{
	LOG("Current  player : %i",current_player );
	// Blit background ===================================

	SDL_RenderCopy(app->render->renderer, background_texture, NULL, &(SDL_Rect)app->win->GetWindowRect());

	float scale = 2.f;
	SDL_Rect section = { 256, 400, 160, 96 };

	if (menu_state == MENU_STATE::SELECTION || menu_state == MENU_STATE::CHANGE_SCENE)
	{
		app->render->DrawQuad((SDL_Rect)app->win->GetWindowRect(), 0, 0, 0, 180);

		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
			app->render->BlitScaled(app->ui->GetAtlas(), players[i].tank_pos.x - section.w * 0.5f * scale, players[i].tank_pos.y - section.h * 0.5f * scale + 8.f, camera, &section, scale, scale);
		}
	}

	return true;
}


bool M_MainMenu::Reset()
{
	app->objectmanager->Reset();
	app->ui->Reset();
	return true;
}

bool M_MainMenu::OnHoverEnter(UI_Element * element)
{
	return true;
}

void M_MainMenu::SetState(MENU_STATE new_state)
{
	fRect screen = app->win->GetWindowRect();
	fPoint screen_center = { screen.w * 0.5f, screen.h * 0.5f };

	switch (new_state)
	{
	case MENU_STATE::INIT_MENU:

		menu_panel->SetStateToBranch(ELEMENT_STATE::VISIBLE);
		logo_image->SetState(ELEMENT_STATE::VISIBLE);

		selection_panel->SetStateToBranch(ELEMENT_STATE::HIDDEN);
		control_helper_label->SetPos(screen_center + fPoint(-330, 260));
		control_helper_label->SetText("Acept");
		control_helper_image->SetPos(screen_center + fPoint(-380, 260));

		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
			players[i].tank->active = false;
		}

		break;

	case MENU_STATE::SELECTION:
		//Set Values -------------------------------------------

		current_player = 0;
		ResetPanelColors();

		// Set visible elements --------------------------------
		menu_panel->SetStateToBranch(ELEMENT_STATE::HIDDEN);
		logo_image->SetState(ELEMENT_STATE::HIDDEN);

		selection_panel->SetStateToBranch(ELEMENT_STATE::VISIBLE);
		control_helper_label->SetPos(screen_center + fPoint( 40, 350));
		control_helper_label->SetText("Select Color");
		control_helper_image->SetPos(screen_center + fPoint(-50, 350));

		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
			players[i].tank->active = true;
		}

		break;
	}

	menu_state = new_state;

}

void M_MainMenu::InputNavigate()
{

	int player_num = -1;
	UI_InteractiveGroup* panel = nullptr;

	if (menu_state == MENU_STATE::INIT_MENU)
	{
		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
			if (players[i].controller != nullptr)
			{
				menu_panel->AndleControllerINavigation((*players[i].controller));
			}
		}

		menu_panel->AndleKeyboardNavigation();

	}
	else if (menu_state == MENU_STATE::SELECTION)
	{
		if (players[current_player].controller != nullptr)
		{
			selection_panel->AndleControllerINavigation((*players[current_player].controller));
		}

		selection_panel->AndleKeyboardNavigation();
	}
}

void M_MainMenu::InputSelect()
{
	bool input_select_controller = false;

	if (menu_state == MENU_STATE::INIT_MENU)
	{
		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
			if (players[i].controller != nullptr && (*players[i].controller)->GetButtonState(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A) == KEY_DOWN)
			{
				input_select_controller = true;
			}
		}
	}
	else if (menu_state == MENU_STATE::SELECTION)
	{
		if (players[current_player].controller != nullptr &&  (*players[current_player].controller)->GetButtonState(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A) == KEY_DOWN)
		{
			input_select_controller = true;
		}
	}

	if (app->input->GetMouseButton(1) == KEY_UP)
	{
		input_select_controller = true;
	}

	if (input_select_controller == true)
	{
		if (menu_state == MENU_STATE::SELECTION && selection_panel->GetFocusedElement() != nullptr )
		{
			SetPlayerProperties();
		}

		if ( menu_state == MENU_STATE::INIT_MENU && app->ui->GetFocusedElement() != nullptr)
		{
			UI_Element*  menu_element = menu_panel->GetFocusedElement();

			if (menu_element == multi_player_button)
			{
				SetState(MENU_STATE::SELECTION);
			}
			else if (menu_element == exit_button)
			{
				exit_game = true;
			}
		}
	}

}


void M_MainMenu::SetPlayerProperties()
{
	UI_Element* element_focused = selection_panel->GetFocusedElement();

	if (element_focused == nullptr)
	{
		return;
	}

	SDL_Color color_1 = element_focused->color_mod;
	SDL_Color color_2 = { 255,255,255,255 };

	if (color_1.r == color_2.r && color_1.g == color_2.g && color_1.b == color_2.b )
	{
		return;
	}

	app->scene->tank_colors[current_player] = element_focused->color_mod;
	element_focused->color_mod = color_2;

	current_player += 1;

	if (current_player == MAX_PLAYERS)
	{
		app->scmanager->FadeToBlack(this, app->scene, 2.f, 2.f);
		menu_state = MENU_STATE::CHANGE_SCENE;
	}
}

void M_MainMenu::ResetPanelColors()
{
	if (selection_panel == nullptr)
	{
		return;
	}

	UI_Element* aux = nullptr;

	for (int y = 0; y < DEFAULT_PANEL_ROWS; ++y)
	{
		for (int x = 0; x < DEFAULT_PANEL_COLUMNS; ++x)
		{
			aux = selection_panel->GetElement(iPoint(x, y));
			aux->color_mod = colors[x][y];
		}
	}

	selection_panel->SetFocusImage(iPoint(0, 0));
}

SDL_Color M_MainMenu::GetColor(float value)
{
	if (value > 1.f)
		value = 1.f;
	else if (value < 0.f)
		value = 0.f;

	float array_pos = 6.f * value;
	float int_part = 0;
	float float_part = modf(array_pos, &int_part);

	int   src = (int)int_part;
	int   target = src + 1;

	if (target > 5)
	{
		target = 0;
	}

	float r_value = lerp(R_Color[src], R_Color[target], float_part);
	float g_value = lerp(G_Color[src], G_Color[target], float_part);
	float b_value = lerp(B_Color[src], B_Color[target], float_part);

	SDL_Color ret = { (uint)r_value,  (uint)g_value, (uint)b_value, 255u};

	return ret;

}
