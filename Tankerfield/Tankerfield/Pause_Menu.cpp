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

#include "Controllers_Settings.h"

Pause_Menu::Pause_Menu()
{
	button_enter_sfx = app->audio->LoadFx("audio/Fx/main_menu/button_enter.wav", 20);
	button_select_sfx = app->audio->LoadFx("audio/Fx/main_menu/button_select.wav", 35);
	button_error_sfx = app->audio->LoadFx("audio/Fx/main_menu/button_error.wav", 35);
	selection_finished_sfx = app->audio->LoadFx("audio/Fx/main_menu/selection_finished.wav", 40);

	// Create UI Elements ====================================

	fRect screen = app->win->GetWindowRect();
	fPoint screen_center = { screen.w * 0.5f, screen.h * 0.5f };

	// Global Pause Menu

	panel_panel = app->ui->CreateImage({ screen.w * 0.5f,screen.h * 0.5f }, UI_ImageDef({ 1075,395,606,771 }), this);
	panel_panel->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	/* Labels */
	pause_menu = app->ui->CreateLabel({ screen.w*0.5f, 250 }, UI_LabelDef("Pause", app->font->label_font_38));
	pause_menu->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	pause_menu->SetParent(panel_panel);

	/* Buttons */
	continue_button = app->ui->CreateButton({ screen.w*0.5f,350 }, UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	continue_button->SetLabel({ 0.f,2.f }, UI_LabelDef("Continue", app->font->button_font_22, { 50, 50, 50, 255 }));

	options_menu = app->ui->CreateButton({ screen.w*0.5f,500 }, UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	options_menu->SetLabel({ 0.f,2.f }, UI_LabelDef("Options Menu", app->font->button_font_22, { 50, 50, 50, 255 }));

	main_menu = app->ui->CreateButton({ screen.w * 0.5f,650 }, UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	main_menu->SetLabel({ 0.f,2.f }, UI_LabelDef("Main Menu", app->font->button_font_22, { 50, 50, 50, 255 }));


	UI_InteractiveGroupDef pause_panel_def(-1, nullptr);

	pause_navigation = app->ui->CreateIntearctiveGroup(screen_center, pause_panel_def, this);
	pause_navigation->AddElement(continue_button);
	pause_navigation->AddElement(options_menu);
	pause_navigation->AddElement(main_menu);

	controllers_setting[0] = DBG_NEW Controllers_Settings(fPoint(0, 0), 0, MENU_TYPE::PAUSE_MENU);
	controllers_setting[1] = DBG_NEW Controllers_Settings(fPoint(screen.w*0.5f, 0), 1, MENU_TYPE::PAUSE_MENU);
	controllers_setting[2] = DBG_NEW Controllers_Settings(fPoint(0, screen.h*0.5f), 2, MENU_TYPE::PAUSE_MENU);
	controllers_setting[3] = DBG_NEW Controllers_Settings(fPoint(screen.w*0.5f, screen.h*0.5f), 3, MENU_TYPE::PAUSE_MENU);

	HidePauseMenu();
}

bool Pause_Menu::UI_OnHoverEnter(UI_Element * element)
{
	app->audio->PlayFx(button_enter_sfx);
	return true;
}

void Pause_Menu::ShowPauseMenu()
{
	fRect screen = app->win->GetWindowRect();
	fPoint screen_center = { screen.w * 0.5f, screen.h * 0.5f };

	pause_navigation->Active();

	for (uint i = 0; i < 4; ++i)
	{
		controllers_setting[i]->ShowControllerSettings();
	}

	panel_panel->SetStateToBranch(ELEMENT_STATE::VISIBLE);
	pause_navigation->SetStateToBranch(ELEMENT_STATE::VISIBLE);
}

void Pause_Menu::HidePauseMenu()
{
	pause_navigation->Desactive();

	for (uint i = 0; i < 4; ++i)
	{
		controllers_setting[i]->HideControllersSettings();
	}

	panel_panel->SetStateToBranch(ELEMENT_STATE::HIDDEN);
	pause_navigation->SetStateToBranch(ELEMENT_STATE::HIDDEN);
}


bool Pause_Menu::UI_Selected(UI_Element * element)
{
	if (element == options_menu)
	{
		app->scene->SetMenuState(MENU_STATE::OPTIONS);
	}
	else if (element == main_menu)
	{
		app->scmanager->FadeToBlack(app->scene, app->main_menu, 2.f, 2.f);
		app->scene->SetMenuState(MENU_STATE::NO_TYPE);
	}
	else if (element == continue_button)
	{
		app->scene->SetMenuState(MENU_STATE::NO_TYPE);
	}

	app->audio->PlayFx(button_select_sfx);

	return true;
}
