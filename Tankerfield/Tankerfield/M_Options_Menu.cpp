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
#include <string.h>

#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_Label.h"
#include "UI_InteractiveGroup.h"

bool M_Options_Menu::Start()
{
	background_texture = app->tex->Load("textures/ui/main_menu_background.png");
	app->audio->PlayMusic("audio/Music/menu_music.ogg");

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

	options_title = app->ui->CreateLabel({ screen.w/2, 225 }, UI_LabelDef("Options", app->font->label_font_38, { 255,255,255,180 }));
	options_title->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

		//Master Volume

	master_volume_label=app->ui->CreateLabel({ screen.w * 0.5f -100, 325 }, UI_LabelDef("Master Volume", app->font->label_font_38, { 255,255,255,180 }));
	master_volume_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	master_volume_L = app->ui->CreateButton({ screen.w * 0.5f + 50, 325 },UI_ButtonDef({ 310,510,33,35 }, { 310, 550,33,35 }, { 495,970,50 ,10 }, { 785 ,970,50,10 }), this);
	master_volume_L->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	master_volume_R = app->ui->CreateButton({ screen.w * 0.5f + 170, 325 }, UI_ButtonDef({ 350,510,33,35 }, { 350, 550	,33,35 }, { 495,970,10 ,10 }, { 785 ,970,10,10 }), this);
	master_volume_R->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	master_multiplier_string = std::to_string(int(app->audio->master_volume * 100));

	master_volume_value = app->ui->CreateLabel({ screen.w * 0.5f + 110, 325 }, UI_LabelDef(master_multiplier_string,app->font->label_font_38), this);
	master_volume_value->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

		//Music Volume

	music_volume_label = app->ui->CreateLabel({ screen.w * 0.5f - 100, 500 }, UI_LabelDef("Music Volume", app->font->label_font_38, { 255,255,255,180 }));
	music_volume_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	music_volume_L = app->ui->CreateButton({ screen.w * 0.5f + 50, 500 }, UI_ButtonDef({ 310,510,33,35 }, { 310, 550,33,35 }, { 495,970,50 ,10 }, { 785 ,970,50,10 }), this);
	music_volume_L->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	music_volume_R = app->ui->CreateButton({ screen.w * 0.5f + 170, 500 }, UI_ButtonDef({ 350,510,33,35 }, { 350, 550	,33,35 }, { 495,970,10 ,10 }, { 785 ,970,10,10 }), this);
	music_volume_R->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	music_volume_string = std::to_string(app->audio->GetMusicVolume());

	music_volume_value = app->ui->CreateLabel({ screen.w* 0.5f + 110, 500 }, UI_LabelDef(music_volume_string, app->font->label_font_38), this);
	music_volume_value->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

		// Sfx Volume

	sfx_volume_label = app->ui->CreateLabel({ screen.w * 0.5f - 100, 675 }, UI_LabelDef("SFX Volume", app->font->label_font_38, { 255,255,255,180 }));
	sfx_volume_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	sfx_volume_L = app->ui->CreateButton({ screen.w * 0.5f + 50, 675 }, UI_ButtonDef({ 310,510,33,35 }, { 310, 550,33,35 }, { 495,970,50 ,10 }, { 785 ,970,50,10 }), this);
	sfx_volume_L->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	sfx_volume_R = app->ui->CreateButton({ screen.w * 0.5f + 170, 675 }, UI_ButtonDef({ 350,510,33,35 }, { 350, 550	,33,35 }, { 495,970,10 ,10 }, { 785 ,970,10,10 }), this);
	sfx_volume_R->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	sfx_volume_string=std::to_string(app->audio->GetSfxVolume());

	sfx_volume_value = app->ui->CreateLabel({ screen.w * 0.5f + 110, 675 }, UI_LabelDef(sfx_volume_string, app->font->label_font_38), this);
	sfx_volume_value->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

		// Else

	individual_settings = app->ui->CreateButton({ screen.w * 0.5f,800 }, UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	individual_settings->SetLabel({ 0.f,2.f }, UI_LabelDef("Settings", app->font->button_font_22, { 50, 50, 50, 255 }));


		// Navigation Matrix

	UI_InteractiveGroupDef options_panel_def;
	options_panel_def.columns = 2;
	options_panel_def.rows = 6;


	global_navigation_panel = app->ui->CreateIntearctiveGroup(screen_center, options_panel_def, this);
	global_navigation_panel->SetElement(master_volume_L, iPoint(0, 0));
	global_navigation_panel->SetElement(master_volume_R, iPoint(1, 0));
	global_navigation_panel->SetElement(music_volume_L, iPoint(0, 1));
	global_navigation_panel->SetElement(music_volume_R, iPoint(1, 1));
	global_navigation_panel->SetElement(sfx_volume_L, iPoint(0, 2));
	global_navigation_panel->SetElement(sfx_volume_R, iPoint(1, 2));
	global_navigation_panel->SetElement(individual_settings, iPoint(0, 5));
	global_navigation_panel->SetElement(individual_settings, iPoint(1, 5));

		// Labels Matrix

	UI_InteractiveGroupDef labels_panel_def;
	labels_panel_def.columns = 2;
	labels_panel_def.rows = 6;

	labels_panel = app->ui->CreateIntearctiveGroup(screen_center, labels_panel_def, this);
	labels_panel->SetElement(master_volume_label, iPoint(0, 0));
	labels_panel->SetElement(master_volume_value, iPoint(1, 0));
	labels_panel->SetElement(music_volume_label, iPoint(0, 1));
	labels_panel->SetElement(music_volume_value, iPoint(1, 1));
	labels_panel->SetElement(sfx_volume_label, iPoint(0, 2));
	labels_panel->SetElement(sfx_volume_value, iPoint(1, 2));

	// Set values ==========================================

	SetState(OPTIONS_STATE::GLOBAL_OPTIONS);
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
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		return false;
	}

	if (options_state != OPTIONS_STATE::CHANGE_SCENE)
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

void M_Options_Menu::SetState(OPTIONS_STATE new_state)
{
	fRect screen = app->win->GetWindowRect();
	fPoint screen_center = { screen.w * 0.5f, screen.h * 0.5f };

	switch (new_state)
	{
	case OPTIONS_STATE::GLOBAL_OPTIONS:
		control_helper_label->SetPos(screen_center + fPoint(30, 450));
		control_helper_label->SetText("Accept");
		control_helper_image->SetPos(screen_center + fPoint(-30, 450));

		global_navigation_panel->SetStateToBranch(ELEMENT_STATE::VISIBLE);
		labels_panel->SetStateToBranch(ELEMENT_STATE::VISIBLE);

		break;

	case OPTIONS_STATE::INDIVIDUAL_OPTIONS:
		control_helper_label->SetPos(screen_center + fPoint(30, 450));
		control_helper_label->SetText("Accept");
		control_helper_image->SetPos(screen_center + fPoint(-30, 450));

		global_navigation_panel->SetStateToBranch(ELEMENT_STATE::HIDDEN);
		labels_panel->SetStateToBranch(ELEMENT_STATE::HIDDEN);

		break;
	}
	options_state = new_state;

}

void M_Options_Menu::InputNavigate()
{

	int player_num = -1;
	UI_InteractiveGroup* panel = nullptr;

	if (options_state == OPTIONS_STATE::GLOBAL_OPTIONS)
	{
		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
			if (players[i].controller != nullptr)
			{
				if (global_navigation_panel->HandleControllerINavigation((*players[i].controller)))
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
}

void M_Options_Menu::InputSelect()
{
	bool input_select_controller = false;

	if (options_state == OPTIONS_STATE::GLOBAL_OPTIONS)
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
		if (options_state == OPTIONS_STATE::GLOBAL_OPTIONS && global_navigation_panel->GetFocusedElement() != nullptr)
		{
			UI_Element*  menu_element = global_navigation_panel->GetFocusedElement();

			if (menu_element == individual_settings)
			{
				SetState(OPTIONS_STATE::INDIVIDUAL_OPTIONS);
				
			}
			else if (menu_element == master_volume_L)
			{
				master_multiplier_string = std::to_string(int((app->audio->master_volume -= 0.05)*100));
				master_volume_value->SetText(master_multiplier_string);
				app->audio->SetMasterVolume(app->audio->master_volume);
				LOG("%f", app->audio->master_volume);
			}
			else if (menu_element == master_volume_R)
			{
				master_multiplier_string = std::to_string(int((app->audio->master_volume += 0.05) * 100));
				master_volume_value->SetText(master_multiplier_string);
				app->audio->SetMasterVolume(app->audio->master_volume);
				LOG("%f", app->audio->master_volume);
			}
			else if (menu_element == music_volume_L)
			{
				music_volume_string = std::to_string(app->audio->music_volume -= 5);
				music_volume_value->SetText(music_volume_string);
				app->audio->SetMusicVolume(app->audio->music_volume);
			}
			else if (menu_element == music_volume_R)
			{
				music_volume_string = std::to_string(app->audio->music_volume += 5);
				music_volume_value->SetText(music_volume_string);
				app->audio->SetMusicVolume(app->audio->music_volume);
			}
			else if (menu_element == sfx_volume_L)
			{
				sfx_volume_string=std::to_string(app->audio->sfx_volume -= 5);
				sfx_volume_value->SetText(sfx_volume_string);
			}
			else if (menu_element == sfx_volume_R)
			{
				sfx_volume_string = std::to_string(app->audio->sfx_volume += 5);
				sfx_volume_value->SetText(sfx_volume_string);
			}

			app->audio->PlayFx(button_select_sfx);
		}

	}

}


bool M_Options_Menu::SetPlayerProperties()
{
	UI_Element* element_focused = global_navigation_panel->GetFocusedElement();

	if (element_focused == nullptr)
	{
		return false;
	}

	return true;
}
