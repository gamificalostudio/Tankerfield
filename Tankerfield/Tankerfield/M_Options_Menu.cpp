#include "App.h"
#include "Log.h"
#include "M_Options_Menu.h"
#include "M_Render.h"
#include "M_Textures.h"
#include "M_Fonts.h"
#include "M_SceneManager.h"
#include "M_Window.h"
#include "M_Audio.h"
#include "M_Scene.h"
#include "Log.h"

#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_Label.h"
#include "UI_InteractiveGroup.h"

bool M_Options_Menu::Start()
{
	background_texture = app->tex->Load("textures/ui/main_menu_background.png");

	button_enter_sfx = app->audio->LoadFx("audio/Fx/main_menu/button_enter.wav", 20);
	button_select_sfx = app->audio->LoadFx("audio/Fx/main_menu/button_select.wav", 35);
	button_error_sfx = app->audio->LoadFx("audio/Fx/main_menu/button_error.wav", 35);
	selection_finished_sfx = app->audio->LoadFx("audio/Fx/main_menu/selection_finished.wav", 40);


	// Create UI Elements ====================================

	fRect screen = app->win->GetWindowRect();
	fPoint screen_center = { screen.w * 0.5f, screen.h * 0.5f };
	camera = app->render->CreateCamera(iPoint(0, 0), (SDL_Rect)screen);

	// Controll helper ------------------------

	control_helper_image = app->ui->CreateImage(screen_center + fPoint(-40.f, 400.f), UI_ImageDef(app->ui->button_sprites[(int)CONTROLLER_BUTTON::A]));
	control_helper_image->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	control_helper_label = app->ui->CreateLabel(screen_center + fPoint(10.f, 400.f), UI_LabelDef("Accept", app->font->label_font_24));
	control_helper_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	// Global Options Menu

	UI_InteractiveGroupDef options_panel_def;
	options_panel_def.columns = 1;
	options_panel_def.rows = 5;

	global_options_panel = app->ui->CreateIntearctiveGroup(screen_center, options_panel_def, this);

	// Set values ==========================================

	SetState(MENU_STATE::GLOBAL_OPTIONS);
	SDL_ShowCursor(SDL_ENABLE);

	return true;
}



bool M_Options_Menu::CleanUp()
{
	app->tex->UnLoad(background_texture);
	app->render->DestroyCamera(camera);

	for (int i = 0; i < 4; ++i)
	{
		players[i].tank = nullptr;
	}

	return true;
}

bool M_Options_Menu::PreUpdate()
{
	if (menu_state != MENU_STATE::CHANGE_SCENE)
	{
		InputNavigate();
		InputSelect();
	}

	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		if (players[i].controller == nullptr)
		{
			players[i].controller = app->input->GetAbleController();
		}
	}

	return true;
}

bool M_Options_Menu::Update(float dt)
{

	return true;
}

bool M_Options_Menu::PostUpdate(float dt)
{
	// Blit background ===================================

	SDL_RenderCopy(app->render->renderer, background_texture, NULL, &(SDL_Rect)app->win->GetWindowRect());

	float scale = 2.f;
	SDL_Rect section = { 256, 400, 160, 96 };

	return true;
}

bool M_Options_Menu::OnHoverEnter(UI_Element * element)
{
	app->audio->PlayFx(button_enter_sfx);
	return true;
}

void M_Options_Menu::SetState(MENU_STATE new_state)
{
	fRect screen = app->win->GetWindowRect();
	fPoint screen_center = { screen.w * 0.5f, screen.h * 0.5f };

	switch (new_state)
	{
	case MENU_STATE::GLOBAL_OPTIONS:
		control_helper_label->SetPos(screen_center + fPoint(30, 450));
		control_helper_label->SetText("Accept");
		control_helper_image->SetPos(screen_center + fPoint(-30, 450));

		global_options_panel->SetStateToBranch(ELEMENT_STATE::VISIBLE);

		break;
	}
	menu_state = new_state;

}

void M_Options_Menu::InputNavigate()
{

	int player_num = -1;
	UI_InteractiveGroup* panel = nullptr;

	if (menu_state == MENU_STATE::GLOBAL_OPTIONS)
	{
		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
			if (players[i].controller != nullptr)
			{
				if (global_options_panel->HandleControllerINavigation((*players[i].controller)))
				{
					app->audio->PlayFx(button_enter_sfx);
				}
			}
		}

		if (global_options_panel->HandleKeyboardNavigation())
		{
			app->audio->PlayFx(button_enter_sfx);
		}


	}
}

void M_Options_Menu::InputSelect()
{
	bool input_select_controller = false;

	if (menu_state == MENU_STATE::GLOBAL_OPTIONS)
	{
		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
			if (players[i].controller != nullptr && (*players[i].controller)->GetButtonState(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A) == KEY_DOWN)
			{
				input_select_controller = true;
			}
		}
	}

	if (app->input->GetMouseButton(1) == KEY_UP)
	{
		input_select_controller = true;
	}

	if (input_select_controller == true)
	{
		if (menu_state == MENU_STATE::GLOBAL_OPTIONS && global_options_panel->GetFocusedElement() != nullptr)
		{
			UI_Element*  menu_element = global_options_panel->GetFocusedElement();

			//if (menu_element == multi_player_button)
			//{
			//	SetState(MENU_STATE::SELECTION);
			//	app->audio->PlayFx(button_select_sfx);
			//}
			//else if (menu_element == leaderboard_menu_button)
			//{
			//	SetState(MENU_STATE::OPTIONS);
			//	app->audio->PlayFx(button_select_sfx);
			//}
			//else if (menu_element == options_menu_button)
			//{
			//	SetState(MENU_STATE::OPTIONS);
			//	app->audio->PlayFx(button_select_sfx);
			//}
			//else if (menu_element == exit_button)
			//{
			//	exit_game = true;
			//	app->audio->PlayFx(button_select_sfx);
			//}
		}

	}

}


bool M_Options_Menu::SetPlayerProperties()
{
	UI_Element* element_focused = global_options_panel->GetFocusedElement();

	if (element_focused == nullptr)
	{
		return false;
	}

	return true;
}
