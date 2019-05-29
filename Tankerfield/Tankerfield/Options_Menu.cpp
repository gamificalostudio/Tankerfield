#include "App.h"
#include "Log.h"
#include "Options_Menu.h"
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

#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_Label.h"
#include "UI_InteractiveGroup.h"

Options_Menu::Options_Menu()
{
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

	panel_background = app->ui->CreateImage({ screen.w * 0.5f,screen.h * 0.5f }, UI_ImageDef({ 1075,395,606,771 }), this);
	panel_background->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	options_title = app->ui->CreateLabel({ screen.w *0.5f, 250 }, UI_LabelDef("Options", app->font->label_font_38, { 255,255,255,180 }));
	options_title->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	options_title->SetParent(panel_background);

		// Fullscreen Options

	fullscreen_label = app->ui->CreateLabel({ screen.w*0.5f-100,350 }, UI_LabelDef("Fullscreen", app->font->label_font_38, { 255,255,255,180 }));
	fullscreen_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	fullscreen_label->SetParent(panel_background);

	if (app->win->fullscreen == true) 
	{
		fullscreen_value = app->ui->CreateLabel({ screen.w * 0.5f + 110, 350 }, UI_LabelDef("Yes", app->font->label_font_38), this);
		fullscreen_value_info = true;
	}
	else if (app->win->fullscreen == false)
	{
		fullscreen_value = app->ui->CreateLabel({ screen.w * 0.5f + 110, 350 }, UI_LabelDef("No", app->font->label_font_38), this);
		fullscreen_value_info = false;
	}
	fullscreen_value->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	fullscreen_value->SetParent(panel_background);

	fullscreen_L = app->ui->CreateButton({ screen.w * 0.5f + 50, 350 }, UI_ButtonDef({ 310,510,33,35 }, { 310, 550,33,35 }, { 310 ,645,46,44 }, { 302 ,592,55,50 }), this);
	fullscreen_L->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	fullscreen_R = app->ui->CreateButton({ screen.w * 0.5f + 170, 350 }, UI_ButtonDef({ 350,510,33,35 }, { 350, 550	,33,35 }, { 365 ,645,53,50 }, { 365 ,590,53,50 }), this);
	fullscreen_R->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

		//Master Volume

	master_volume_label=app->ui->CreateLabel({ screen.w * 0.5f -100, 450 }, UI_LabelDef("Master Volume", app->font->label_font_38, { 255,255,255,180 }));
	master_volume_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	master_volume_label->SetParent(panel_background);

	master_volume_L = app->ui->CreateButton({ screen.w * 0.5f + 50, 450 },UI_ButtonDef({ 310,510,33,35 }, { 310, 550,33,35 }, { 310 ,645,46,44 }, { 302 ,592,55,50 }), this);
	master_volume_L->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	master_volume_R = app->ui->CreateButton({ screen.w * 0.5f + 170, 450 }, UI_ButtonDef({ 350,510,33,35 }, { 350, 550	,33,35 }, { 365 ,645,53,50 }, { 365 ,590,53,50 }), this);
	master_volume_R->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	master_multiplier_string = std::to_string(int(app->audio->master_volume * 100));

	master_volume_value = app->ui->CreateLabel({ screen.w * 0.5f + 110, 450 }, UI_LabelDef(master_multiplier_string,app->font->label_font_38), this);
	master_volume_value->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	master_volume_value->SetParent(panel_background);

		//Music Volume

	music_volume_label = app->ui->CreateLabel({ screen.w * 0.5f - 100, 550 }, UI_LabelDef("Music Volume", app->font->label_font_38, { 255,255,255,180 }));
	music_volume_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	music_volume_label->SetParent(panel_background);

	music_volume_L = app->ui->CreateButton({ screen.w * 0.5f + 50, 550 }, UI_ButtonDef({ 310,510,33,35 }, { 310, 550,33,35 }, { 310 ,645,46,44 }, { 302 ,592,55,50 }), this);
	music_volume_L->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	music_volume_R = app->ui->CreateButton({ screen.w * 0.5f + 170, 550 }, UI_ButtonDef({ 350,510,33,35 }, { 350, 550,33,35 }, { 365 ,645,53,50 }, { 365 ,590,53,50 }), this);
	music_volume_R->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	music_volume_string = std::to_string(app->audio->GetMusicVolume());

	music_volume_value = app->ui->CreateLabel({ screen.w* 0.5f + 110, 550 }, UI_LabelDef(music_volume_string, app->font->label_font_38), this);
	music_volume_value->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	music_volume_value->SetParent(panel_background);

		// Sfx Volume

	sfx_volume_label = app->ui->CreateLabel({ screen.w * 0.5f - 100, 650 }, UI_LabelDef("SFX Volume", app->font->label_font_38, { 255,255,255,180 }));
	sfx_volume_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	sfx_volume_label->SetParent(panel_background);

	sfx_volume_L = app->ui->CreateButton({ screen.w * 0.5f + 50, 650 }, UI_ButtonDef({ 310,510,33,35 }, { 310, 550,33,35 }, { 310 ,645,46,44 }, { 302 ,592,55,50 }), this);
	sfx_volume_L->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	sfx_volume_R = app->ui->CreateButton({ screen.w * 0.5f + 170, 650 }, UI_ButtonDef({ 350,510,33,35 }, { 350, 550	,33,35 }, { 365 ,645,53,50 }, { 365 ,590,53,50 }), this);
	sfx_volume_R->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	sfx_volume_string=std::to_string(app->audio->GetSfxVolume());

	sfx_volume_value = app->ui->CreateLabel({ screen.w * 0.5f + 110, 650 }, UI_LabelDef(sfx_volume_string, app->font->label_font_38), this);
	sfx_volume_value->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	sfx_volume_value->SetParent(panel_background);

		// Else

	individual_settings = app->ui->CreateButton({ screen.w * 0.5f,800 }, UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	individual_settings->SetLabel({ 0.f,2.f }, UI_LabelDef("Settings", app->font->button_font_22, { 50, 50, 50, 255 }));


		// Navigation Matrix

	UI_InteractiveGroupDef options_panel_def;
	options_panel_def.columns = 2;
	options_panel_def.rows = 6;


	global_navigation_panel = app->ui->CreateIntearctiveGroup(screen_center, options_panel_def, this);
	global_navigation_panel->SetElement(fullscreen_L, iPoint(0, 0));
	global_navigation_panel->SetElement(fullscreen_R, iPoint(1, 0));
	global_navigation_panel->SetElement(master_volume_L, iPoint(0, 1));
	global_navigation_panel->SetElement(master_volume_R, iPoint(1, 1));
	global_navigation_panel->SetElement(music_volume_L, iPoint(0, 2));
	global_navigation_panel->SetElement(music_volume_R, iPoint(1, 2));
	global_navigation_panel->SetElement(sfx_volume_L, iPoint(0, 3));
	global_navigation_panel->SetElement(sfx_volume_R, iPoint(1, 3));
	global_navigation_panel->SetElement(individual_settings, iPoint(0, 5));
	global_navigation_panel->SetElement(individual_settings, iPoint(1, 5));

	// Set values ==========================================
	SDL_ShowCursor(SDL_ENABLE);
}

bool Options_Menu::OnHoverEnter(UI_Element * element)
{
	app->audio->PlayFx(button_enter_sfx);
	return true;
}

void Options_Menu::ShowMenu()
{
	fRect screen = app->win->GetWindowRect();
	fPoint screen_center = { screen.w * 0.5f, screen.h * 0.5f };

	control_helper_label->SetPos(screen_center + fPoint(30, 450));
	control_helper_label->SetText("Accept");
	control_helper_image->SetPos(screen_center + fPoint(-30, 450));

	panel_background->SetStateToBranch(ELEMENT_STATE::VISIBLE);
	global_navigation_panel->SetStateToBranch(ELEMENT_STATE::VISIBLE);
}

void Options_Menu::HideMenu()
{
	panel_background->SetStateToBranch(ELEMENT_STATE::HIDDEN);
	global_navigation_panel->SetStateToBranch(ELEMENT_STATE::HIDDEN);
}


void Options_Menu::InputNavigate()
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

void Options_Menu::InputSelect()
{
		UI_Element*  menu_element = global_navigation_panel->GetFocusedElement();

		if (menu_element == fullscreen_L||menu_element==fullscreen_R)
		{
			app->win->SetFullscreen();
			fullscreen_value_info = !fullscreen_value_info;

			if (fullscreen_value_info)
			{
				fullscreen_value->SetText("Yes");
			}
			else
			{
				fullscreen_value->SetText("No");
			}
		}
		else if (menu_element == master_volume_L)
		{
			if (app->audio->master_volume >= 0.04)
			{
				master_multiplier_string = std::to_string(int(round((app->audio->master_volume -= 0.05) * 100)));
				master_volume_value->SetText(master_multiplier_string);
				app->audio->SetMasterVolume(app->audio->master_volume);
				LOG("%f", app->audio->master_volume);
			}
		}
		else if (menu_element == master_volume_R)
		{
			if (app->audio->master_volume <= 0.95)
			{
				master_multiplier_string = std::to_string(int(round((app->audio->master_volume += 0.05) * 100)));
				master_volume_value->SetText(master_multiplier_string);
				app->audio->SetMasterVolume(app->audio->master_volume);
				LOG("%f", app->audio->master_volume);
			}
		}
		else if (menu_element == music_volume_L)
		{
			if (app->audio->music_volume >= 5)
			{
				music_volume_string = std::to_string(app->audio->music_volume -= 5);
				music_volume_value->SetText(music_volume_string);
				app->audio->SetMusicVolume(app->audio->music_volume);
			}
		}
		else if (menu_element == music_volume_R)
		{
			if (app->audio->music_volume <= 95)
			{
				music_volume_string = std::to_string(app->audio->music_volume += 5);
				music_volume_value->SetText(music_volume_string);
				app->audio->SetMusicVolume(app->audio->music_volume);
			}
		}
		else if (menu_element == sfx_volume_L)
		{
			if (app->audio->sfx_volume >= 5)
			{
				sfx_volume_string = std::to_string(app->audio->sfx_volume -= 5);
				sfx_volume_value->SetText(sfx_volume_string);
				app->audio->SetSfxVolume(-5);
			}
		}
		else if (menu_element == sfx_volume_R)
		{
			if (app->audio->sfx_volume <= 95)
			{
				sfx_volume_string = std::to_string(app->audio->sfx_volume += 5);
				sfx_volume_value->SetText(sfx_volume_string);
				app->audio->SetSfxVolume(5);
			}
		}

		app->audio->PlayFx(button_select_sfx);
}