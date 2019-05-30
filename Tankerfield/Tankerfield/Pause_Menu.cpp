#include "App.h"
#include "Log.h"
#include "Pause_Menu.h"
#include "M_Render.h"
#include "M_Textures.h"
#include "M_Fonts.h"
#include "M_SceneManager.h"
#include "M_Window.h"
#include "M_Audio.h"
#include "M_Scene.h"
#include "Log.h"
#include <string.h>
#include "M_MainMenu.h"
#include "Options_Menu.h"

#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_Label.h"
#include "UI_InteractiveGroup.h"

Pause_Menu::Pause_Menu()
{
	button_enter_sfx = app->audio->LoadFx("audio/Fx/main_menu/button_enter.wav", 20);
	button_select_sfx = app->audio->LoadFx("audio/Fx/main_menu/button_select.wav", 35);
	button_error_sfx = app->audio->LoadFx("audio/Fx/main_menu/button_error.wav", 35);
	selection_finished_sfx = app->audio->LoadFx("audio/Fx/main_menu/selection_finished.wav", 40);

	options = new Options_Menu();
	options->HideOptionsMenu();
	// Create UI Elements ====================================

	fRect screen = app->win->GetWindowRect();
	fPoint screen_center = { screen.w * 0.5f, screen.h * 0.5f };
	camera = app->render->CreateCamera(iPoint(0, 0), (SDL_Rect)screen);

	// Controll helper ------------------------

	control_helper_image = app->ui->CreateImage(screen_center + fPoint(-40.f, 400.f), UI_ImageDef(app->ui->button_sprites[(int)CONTROLLER_BUTTON::A]));
	control_helper_image->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	control_helper_label = app->ui->CreateLabel(screen_center + fPoint(10.f, 400.f), UI_LabelDef("Accept", app->font->label_font_24));
	control_helper_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	// Global Pause Menu

	panel_background = app->ui->CreateImage({ screen.w * 0.5f,screen.h * 0.5f }, UI_ImageDef({ 1075,395,606,771 }), this);
	panel_background->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	pause_menu = app->ui->CreateLabel({ screen.w*0.5f, 250 }, UI_LabelDef("Pause", app->font->label_font_38));
	pause_menu->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	pause_menu->SetParent(panel_background);

	main_menu = app->ui->CreateButton({ screen.w * 0.5f,650 }, UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	main_menu->SetLabel({ 0.f,2.f }, UI_LabelDef("Main Menu", app->font->button_font_22, { 50, 50, 50, 255 }));

	options_menu = app->ui->CreateButton({screen.w*0.5f,500}, UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	options_menu->SetLabel({ 0.f,2.f }, UI_LabelDef("Options Menu", app->font->button_font_22, { 50, 50, 50, 255 }));

	UI_InteractiveGroupDef pause_panel_def;
	pause_panel_def.columns = 1;
	pause_panel_def.rows = 4;


	global_navigation_panel = app->ui->CreateIntearctiveGroup(screen_center, pause_panel_def, this);
	global_navigation_panel->SetElement(options_menu, iPoint(0, 1));
	global_navigation_panel->SetElement(main_menu, iPoint(0, 2));
	// Set values ==========================================
	SDL_ShowCursor(SDL_ENABLE);
}

bool Pause_Menu::OnHoverEnter(UI_Element * element)
{
	app->audio->PlayFx(button_enter_sfx);
	return true;
}

void Pause_Menu::ShowPauseMenu()
{
	fRect screen = app->win->GetWindowRect();
	fPoint screen_center = { screen.w * 0.5f, screen.h * 0.5f };

	control_helper_label->SetPos(screen_center + fPoint(30, 450));
	control_helper_label->SetText("Accept");
	control_helper_image->SetPos(screen_center + fPoint(-30, 450));

	panel_background->SetStateToBranch(ELEMENT_STATE::VISIBLE);
	global_navigation_panel->SetStateToBranch(ELEMENT_STATE::VISIBLE);
}

void Pause_Menu::HidePauseMenu()
{
	panel_background->SetStateToBranch(ELEMENT_STATE::HIDDEN);
	global_navigation_panel->SetStateToBranch(ELEMENT_STATE::HIDDEN);
}


void Pause_Menu::InputNavigate()
{
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		if (app->main_menu->players[i].controller != nullptr)
		{
			if (global_navigation_panel->HandleControllerINavigation(app->main_menu->players[i].controller))
			{
				app->audio->PlayFx(button_enter_sfx);
			}
		}
	}

	if (global_navigation_panel->HandleKeyboardNavigation())
	{
		app->audio->PlayFx(button_enter_sfx);
	}
}

void Pause_Menu::InputSelect()
{
	UI_Element*  menu_element = global_navigation_panel->GetFocusedElement();

	if (menu_element == options_menu)
	{
		HidePauseMenu();
		options->ShowOptionsMenu();
	}
	else if (menu_element == main_menu)
	{
		HidePauseMenu();
		app->scmanager->FadeToBlack(app->scene, app->main_menu, 2.f, 2.f);
	}

	app->audio->PlayFx(button_select_sfx);
}
